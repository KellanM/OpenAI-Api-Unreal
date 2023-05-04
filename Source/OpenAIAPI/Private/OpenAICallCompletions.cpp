// Copyright Kellan Mythen 2023. All rights Reserved.


#include "OpenAICallCompletions.h"
#include "OpenAIUtils.h"
#include "Http.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "OpenAIParser.h"


UOpenAICallCompletions::UOpenAICallCompletions()
{
}

UOpenAICallCompletions::~UOpenAICallCompletions()
{
}

UOpenAICallCompletions* UOpenAICallCompletions::OpenAICallCompletions(EOACompletionsEngineType engineInput, FString promptInput, FCompletionSettings settingsInput)
{
	UOpenAICallCompletions* BPNode = NewObject<UOpenAICallCompletions>();
	BPNode->engine = engineInput;
	BPNode->prompt = promptInput;
	BPNode->settings = settingsInput;
	return BPNode;
}

void UOpenAICallCompletions::Activate()
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
	} else if (prompt.IsEmpty())
	{
		Finished.Broadcast({}, TEXT("Prompt is empty"), {}, false);
	} else if (settings.bestOf < settings.numCompletions)
	{
		Finished.Broadcast({}, TEXT("bestOf must be greater than numCompletions"), {}, false);
	} else if (settings.maxTokens <= 0 || ( engine != EOACompletionsEngineType::TEXT_DAVINCI_003 && settings.maxTokens >= 2048) || ( engine == EOACompletionsEngineType::TEXT_DAVINCI_003 && settings.maxTokens >= 4000))
	{
		Finished.Broadcast({}, TEXT("maxTokens must be within 0 and 2048. Up to 4096 if using davinci-3."), {}, false);
	} else if (settings.stopSequences.Num() > 4)
	{
		Finished.Broadcast({}, TEXT("You can only include up to 4 Stop Sequences"), {}, false);
	} else if (settings.stopSequences.Contains(""))
	{
		Finished.Broadcast({}, TEXT("One or more Stop Sequences has no value"), {}, false);
	}
	
	auto HttpRequest = FHttpModule::Get().CreateRequest();
	
	FString apiMethod;
	switch (engine)
	{
	case EOACompletionsEngineType::DAVINCI:
			apiMethod = "davinci";
	break;
	case EOACompletionsEngineType::CURIE:
			apiMethod = "curie";
	break;
	case EOACompletionsEngineType::BABBAGE:
			apiMethod = "babbage";
	break;
	case EOACompletionsEngineType::ADA:
			apiMethod = "ada";
	break;
	case EOACompletionsEngineType::TEXT_DAVINCI_002:
		apiMethod = "text-davinci-002";
	break;
	case EOACompletionsEngineType::TEXT_CURIE_001:
		apiMethod = "text-curie-001";
	break;
	case EOACompletionsEngineType::TEXT_BABBAGE_001:
		apiMethod = "text-babbage-001";
		break;
	case EOACompletionsEngineType::TEXT_ADA_001:
		apiMethod = "text-ada-001";
		break;
	case EOACompletionsEngineType::TEXT_DAVINCI_003:
		apiMethod = "text-davinci-003";
		break;
	}

	// convert parameters to strings
	FString tempPrompt = settings.startSequence + prompt + settings.injectStartText;
	FString tempHeader = "Bearer ";
	tempHeader += _apiKey;

	// set headers
	FString url = FString::Printf(TEXT("https://api.openai.com/v1/engines/%s/completions"), *apiMethod);
	HttpRequest->SetURL(url);
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->SetHeader(TEXT("Authorization"), tempHeader);

	//build payload
	TSharedPtr<FJsonObject> _payloadObject = MakeShareable(new FJsonObject());
	_payloadObject->SetStringField(TEXT("prompt"), tempPrompt);
	_payloadObject->SetNumberField(TEXT("max_tokens"), settings.maxTokens);
	_payloadObject->SetNumberField(TEXT("temperature"), FMath::Clamp(settings.temperature, 0.0f, 1.0f));
	_payloadObject->SetNumberField(TEXT("top_p"), FMath::Clamp(settings.topP, 0.0f, 1.0f));
	_payloadObject->SetNumberField(TEXT("n"), settings.numCompletions);
	_payloadObject->SetNumberField(TEXT("best_of"), settings.bestOf);
	if (!(settings.presencePenalty == 0))
		_payloadObject->SetNumberField(TEXT("presence_penalty"), FMath::Clamp(settings.presencePenalty, 0.0f, 1.0f));
	if (!(settings.logprobs == 0)){
		_payloadObject->SetNumberField(TEXT("logprobs"), FMath::Clamp(settings.logprobs, 0, 10));
		_payloadObject->SetNumberField(TEXT("presence_penalty"), FMath::Clamp(settings.presencePenalty, 0.0f, 1.0f));
	}
	if (!(settings.frequencyPenalty == 0))
		_payloadObject->SetNumberField(TEXT("frequency_penalty"), FMath::Clamp(settings.frequencyPenalty, 0.0f, 1.0f));
	if (!(settings.stopSequences.Num() == 0))
	{
		TArray<TSharedPtr<FJsonValue>> StopSequences;
		for (int i = 0; i < settings.stopSequences.Num(); i++)
		{
			StopSequences.Add(MakeShareable(new FJsonValueString(settings.stopSequences[i])));
		}
		_payloadObject->SetArrayField(TEXT("stop"), StopSequences);
	}

	// convert payload to string
	FString _payload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
	FJsonSerializer::Serialize(_payloadObject.ToSharedRef(), Writer);

	// commit request
	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->SetContentAsString(_payload);

	if (HttpRequest->ProcessRequest())
	{
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UOpenAICallCompletions::OnResponse);
	}
	else
	{
		Finished.Broadcast({}, ("Error sending request"), {}, false);
	}
}

void UOpenAICallCompletions::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
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
			FString ResponseString = Response->GetContentAsString();
			UE_LOG(LogTemp, Warning, TEXT("%s"), *ResponseString);
			Finished.Broadcast({}, TEXT("Api error: ") + ResponseString, {}, false);
			return;
		}

		OpenAIParser parser(settings);
		TArray<FCompletion> _out;
		FCompletionInfo _info = parser.ParseGPTCompletionInfo(*responseObject);

		auto CompletionsObject = responseObject->GetArrayField(TEXT("Choices"));
		for (auto& elem : CompletionsObject)
		{
			_out.Add(parser.ParseCompletionsResponse(*elem->AsObject()));
		}

		Finished.Broadcast(_out, "", _info, true);
	}
}

