// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenAICallTranscription.h"
#include "OpenAIUtils.h"
#include "Http.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "OpenAIParser.h"

UOpenAICallTranscription::UOpenAICallTranscription()
{
}

UOpenAICallTranscription::~UOpenAICallTranscription()
{
}

UOpenAICallTranscription* UOpenAICallTranscription::OpenAICallTranscription(EOATranscriptionEngineType engineInput, USoundWave* audioInput, FTranscriptionSettings settingsInput)
{
	UOpenAICallTranscription* BPNode = NewObject<UOpenAICallTranscription>();
	BPNode->engine = engineInput;
	BPNode->audio = audioInput;
	BPNode->settings = settingsInput;
	return BPNode;
}

void UOpenAICallTranscription::Activate()
{
	FString _apiKey;
	if (UOpenAIUtils::getUseApiKeyFromEnvironmentVars())
		_apiKey = UOpenAIUtils::GetEnvironmentVariable(TEXT("OPENAI_API_KEY"));
	else
		_apiKey = UOpenAIUtils::getApiKey();


	// checking parameters are valid
	if (_apiKey.IsEmpty())
	{
		Finished.Broadcast({}, TEXT("Api key is not set"), {}, false);
	}
	else if (audio->Duration < KINDA_SMALL_NUMBER)
	{
		Finished.Broadcast({}, TEXT("Audio is empty"), {}, false);
	}

	auto HttpRequest = FHttpModule::Get().CreateRequest();

	FString apiMethod;
	switch (engine)
	{
	case EOATranscriptionEngineType::WHISPER_1:
		apiMethod = "whisper-1";
		break;
	}
	
	// convert parameters to strings
	FString tempHeader = "Bearer ";
	tempHeader += _apiKey;

	FString BoundaryLabel = FString();
	FString BoundaryBegin = FString();
	FString BoundaryEnd = FString();
	// Create a boundary label, for the header
	BoundaryLabel = FString(TEXT("e543322540af456f9a3773049ca02529-")) + FString::FromInt(FMath::Rand());
	// boundary label for begining of every payload chunk 
	BoundaryBegin = FString(TEXT("--")) + BoundaryLabel + FString(TEXT("\r\n"));
	// boundary label for the end of payload
	BoundaryEnd = FString(TEXT("\r\n--")) + BoundaryLabel + FString(TEXT("--\r\n"));

	// set headers
	FString url = TEXT("https://api.openai.com/v1/audio/transcriptions");
	HttpRequest->SetURL(url);
	HttpRequest->SetHeader(TEXT("Content-Type"), FString(TEXT("multipart/form-data; boundary=")) + BoundaryLabel);
	HttpRequest->SetHeader(TEXT("Authorization"), tempHeader);

	//build payload
	TArray<uint8> CombinedContent; // This is binary content of the request
	// First, we add the boundary for the file, which is different from text payload
	FString FileBoundaryString = FString(TEXT("\r\n"))
		+ BoundaryBegin
		+ FString(TEXT("Content-Disposition: form-data; name=\"file\"; filename=\""))
		+ TEXT("audio.wav") + "\"\r\n"
		+ "Content-Type: audio/wav"
		+ FString(TEXT("\r\n\r\n"));

	CombinedContent.Append(UOpenAIUtils::FStringToUint8(FileBoundaryString));

	// Append the file data
	TArray<uint8> waveBytes = Conv_SoundWaveToWavBytes(audio);
	CombinedContent.Append(waveBytes);

	// Let's add couple of text values to the payload
	CombinedContent.Append(UOpenAIUtils::FStringToUint8(AddData("model", apiMethod, BoundaryBegin)));
	CombinedContent.Append(UOpenAIUtils::FStringToUint8(AddData("response_format", "verbose_json", BoundaryBegin)));

	// Finally, add a boundary at the end of the payload
	CombinedContent.Append(UOpenAIUtils::FStringToUint8(BoundaryEnd));

	// commit request
	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->SetContent(CombinedContent);
	//UE_LOG(LogTemp, Warning, TEXT("Request text: %s"), *UOpenAIUtils::Uint8ToFstring(HttpRequest->GetContent()));

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UOpenAICallTranscription::OnResponse);
	}
	else
	{
		Finished.Broadcast({}, ("Error sending request"), {}, false);
	}
}

void UOpenAICallTranscription::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	if (!WasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Error processing request. \n%s \n%s"), *Response->GetContentAsString(), *Response->GetURL());
		if (Finished.IsBound())
		{
			Finished.Broadcast({}, *Response->GetContentAsString(), {}, false);
		}

		return;
	}

	TSharedPtr<FJsonObject> responseObject;
	TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	
	if (FJsonSerializer::Deserialize(reader, responseObject))
	{
		bool err = responseObject->HasField("error");

		if (err)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s"), *Response->GetContentAsString());
			Finished.Broadcast({}, TEXT("Api error"), {}, false);
			return;
		}

		OpenAIParser parser(settings);
		FTranscriptionInfo _info = parser.ParseTranscriptionResponse(*responseObject);

		auto text = responseObject->GetStringField(TEXT("text"));

		Finished.Broadcast(text, "", _info, true);
	}
}

TArray<uint8> UOpenAICallTranscription::Conv_SoundWaveToWavBytes(USoundWave* SoundWave)
{
	TArray<uint8> PCMBytes;
	TArray<uint8> WavBytes;

	//memcpy raw data from soundwave, hmm this won't work for procedurals...
	const void* LockedData = SoundWave->RawData.LockReadOnly();
	PCMBytes.SetNumUninitialized(SoundWave->RawData.GetBulkDataSize());
	FMemory::Memcpy(PCMBytes.GetData(), LockedData, PCMBytes.Num());
	SoundWave->RawData.Unlock();

	//add wav header
	SerializeWaveFile(WavBytes, PCMBytes.GetData(), PCMBytes.Num(), SoundWave->NumChannels, SoundWave->GetSampleRateForCurrentPlatform());

	return WavBytes;
}

FString UOpenAICallTranscription::AddData(FString Name, FString Value, FString BoundaryBegin) {
	return FString(TEXT("\r\n"))
		+ BoundaryBegin
		+ FString(TEXT("Content-Disposition: form-data; name=\""))
		+ Name
		+ FString(TEXT("\"\r\n\r\n"))
		+ Value;
}