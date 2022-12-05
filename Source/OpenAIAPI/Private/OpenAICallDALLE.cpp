// Copyright Kellan Mythen 2021. All rights Reserved.


#include "OpenAICallDALLE.h"
#include "OpenAIUtils.h"
#include "Http.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "OpenAIParser.h"


UOpenAICallDALLE::UOpenAICallDALLE()
{
}

UOpenAICallDALLE::~UOpenAICallDALLE()
{
}

UOpenAICallDALLE* UOpenAICallDALLE::OpenAICallDALLE(EOAImageSize imageSizeInput, FString promptInput, int32 numImagesInput)
{
	UOpenAICallDALLE* BPNode = NewObject<UOpenAICallDALLE>();
	BPNode->imageSize = imageSizeInput;
	BPNode->prompt = promptInput;
	BPNode->numImages = numImagesInput;
	return BPNode;
}

void UOpenAICallDALLE::Activate()
{
	FString _apiKey;
	if (UOpenAIUtils::getUseApiKeyFromEnvironmentVars())
		_apiKey = UOpenAIUtils::GetEnvironmentVariable(TEXT("OPENAI_API_KEY"));
	else
		_apiKey = UOpenAIUtils::getApiKey();


	// checking parameters are valid
	if (_apiKey.IsEmpty())
	{
		Finished.Broadcast({}, TEXT("Api key is not set"), false);
	} else if (prompt.IsEmpty())
	{
		Finished.Broadcast({}, TEXT("Prompt is empty"), false);
	} else if (numImages < 1 || numImages > 10)
	{
		Finished.Broadcast({}, TEXT("NumImages must be set to a value between 1 and 10"), false);
	}
	
	auto HttpRequest = FHttpModule::Get().CreateRequest();
	
	FString imageResolution;
	switch (imageSize)
	{
	case EOAImageSize::SMALL:
			imageResolution = "256x256";
	break;
	case EOAImageSize::MEDIUM:
			imageResolution = "512x512";
	break;
	case EOAImageSize::LARGE:
			imageResolution = "1024x1024";
	break;
	}

	// convert parameters to strings
	FString tempHeader = "Bearer ";
	tempHeader += _apiKey;

	// set headers
	FString url = FString::Printf(TEXT("https://api.openai.com/v1/images/generations"));
	HttpRequest->SetURL(url);
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->SetHeader(TEXT("Authorization"), tempHeader);

	// build payload
	TSharedPtr<FJsonObject> _payloadObject = MakeShareable(new FJsonObject());
	_payloadObject->SetStringField(TEXT("prompt"), prompt);
	_payloadObject->SetNumberField(TEXT("n"), numImages);
	_payloadObject->SetStringField(TEXT("size"), imageResolution);

	// convert payload to string
	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadObject.ToSharedRef(), Writer);

	// commit request
	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UOpenAICallDALLE::OnResponse);
	}
	else
	{
		Finished.Broadcast({}, ("Error sending request"), false);
	}
}

void UOpenAICallDALLE::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
	if (!WasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Error processing request. \n%s \n%s"), *Response->GetContentAsString(), *Response->GetURL());
		if (Finished.IsBound())
		{
			Finished.Broadcast({}, *Response->GetContentAsString(), false);
		}
		return;
	}

	TSharedPtr<FJsonObject> responseObject;
	TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(reader, responseObject))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Response->GetContentAsString());
		bool err = responseObject->HasField("error");
		
		if (err)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s"), *Response->GetContentAsString());
			Finished.Broadcast({}, TEXT("Api error"), false);
			return;
		}

		OpenAIParser parser(settings);
		TArray<FString> _out;

		auto GeneratedImagesObject = responseObject->GetArrayField(TEXT("data"));
		for (auto& elem : GeneratedImagesObject)
		{
		_out.Add(parser.ParseGeneratedImage(*elem->AsObject()));
		}

		Finished.Broadcast(_out, "", true);
	}
}

