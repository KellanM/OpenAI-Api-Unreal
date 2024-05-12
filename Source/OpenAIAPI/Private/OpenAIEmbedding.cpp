// Copyright (c) 2024 tobenot, See LICENSE in the project root for license information.

#include "OpenAIEmbedding.h"
#include "HttpModule.h"
#include "OpenAIUtils.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Serialization/JsonSerializer.h"
#include "Dom/JsonObject.h"

DEFINE_LOG_CATEGORY(LogEmbedding);

UOpenAIEmbedding::UOpenAIEmbedding()
{
}

UOpenAIEmbedding::~UOpenAIEmbedding()
{
    if (CurrentRequest.IsValid())
    {
        CurrentRequest->CancelRequest();
        CurrentRequest.Reset();
    }
}

UOpenAIEmbedding* UOpenAIEmbedding::CreateEmbeddingInstance()
{
    return NewObject<UOpenAIEmbedding>();
}

void UOpenAIEmbedding::Init(const FEmbeddingSettings& Settings)
{
    EmbeddingSettings = Settings;
}

void UOpenAIEmbedding::StartEmbedding()
{
    UE_LOG(LogEmbedding, Log, TEXT("UOpenAIEmbedding StartEmbedding"));

    FString _apiKey;
    if (UOpenAIUtils::getUseApiKeyFromEnvironmentVars())
        _apiKey = UOpenAIUtils::GetEnvironmentVariable(TEXT("OPENAI_API_KEY"));
    else
        _apiKey = UOpenAIUtils::getApiKey();

    if (_apiKey.IsEmpty())
	{
		OnResponseReceived.ExecuteIfBound({}, TEXT("Api key is not set"), false);
		OnResponseReceivedF.ExecuteIfBound({}, TEXT("Api key is not set"), false);
	}
	else
	{
		auto HttpRequest = FHttpModule::Get().CreateRequest();

		FString apiMethod;
		switch (EmbeddingSettings.model)
		{
		case EEmbeddingEngineType::TEXT_EMBEDDING_3_SMALL:
			apiMethod = "text-embedding-3-small";
			break;
		case EEmbeddingEngineType::TEXT_EMBEDDING_3_LARGE:
			apiMethod = "text-embedding-3-large";
			break;
		case EEmbeddingEngineType::TEXT_EMBEDDING_ADA_002:
			apiMethod = "text-embedding-ada";
			break;
		}
		// TODO: Add additional params to match the ones listed in the curl response in: https://platform.openai.com/docs/api-reference/making-requests
		
		// convert parameters to strings
		FString tempHeader = "Bearer ";
		tempHeader += _apiKey;

		// set headers
		FString url = FString::Printf(TEXT("https://api.openai.com/v1/embeddings"));
		HttpRequest->SetURL(url);
		HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
		HttpRequest->SetHeader(TEXT("Authorization"), tempHeader);

		// build payload
		TSharedPtr<FJsonObject> _payloadObject = MakeShareable(new FJsonObject());
		_payloadObject->SetStringField(TEXT("model"), apiMethod);
		_payloadObject->SetStringField(TEXT("input"),
			EmbeddingSettings.input.Replace(TEXT("\n"), TEXT(" ")));

		// convert payload to string
		FString _payload;
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
		FJsonSerializer::Serialize(_payloadObject.ToSharedRef(), Writer);

		// commit request
		HttpRequest->SetVerb(TEXT("POST"));
		HttpRequest->SetContentAsString(_payload);

		UE_LOG(LogEmbedding, Log, TEXT("UOpenAIEmbedding ProcessHttpRequest"));

		// ensure fast connection, I will retry it
		HttpRequest->SetTimeout(10.f);
		
		HttpRequest->OnRequestProgress().BindUObject(this, &UOpenAIEmbedding::HandleRequestProgress);
		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UOpenAIEmbedding::OnResponse);
		UE_LOG(LogEmbedding, Log, TEXT("UOpenAIEmbedding BindProcessRequestComplete"));

		CurrentRequest = HttpRequest;
		
		if (HttpRequest->ProcessRequest())
		{
			UE_LOG(LogEmbedding, Log, TEXT("UOpenAIEmbedding StartProcessRequest"));
		}
		else
		{
			OnResponseReceived.ExecuteIfBound({}, TEXT("Error sending request"), false);
			OnResponseReceivedF.ExecuteIfBound({}, TEXT("Error sending request"), false);
		}
	}
}

void UOpenAIEmbedding::CancelRequest()
{
	if (CurrentRequest.IsValid() && CurrentRequest->GetStatus() == EHttpRequestStatus::Processing)
	{
		CurrentRequest->OnRequestProgress().Unbind();
		CurrentRequest->OnProcessRequestComplete().Unbind();
		CurrentRequest->CancelRequest();
		CurrentRequest.Reset();

		// Optionally, trigger the response delegate with a cancelled state.
		OnResponseReceived.ExecuteIfBound({}, TEXT("Request cancelled"), false);
		OnResponseReceivedF.ExecuteIfBound({}, TEXT("Request cancelled"), false);
	}
}

void UOpenAIEmbedding::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful && Response.IsValid())
	{
		TSharedPtr<FJsonObject> JsonResponse;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

		if (FJsonSerializer::Deserialize(Reader, JsonResponse) && JsonResponse.IsValid())
		{
			TArray<float> EmbeddingVector;
			const TArray<TSharedPtr<FJsonValue>>* DataArray;
			if (JsonResponse->TryGetArrayField(TEXT("data"), DataArray))
			{
				const TSharedPtr<FJsonObject>* FirstObject;
				if ((*DataArray)[0]->TryGetObject(FirstObject))
				{
					const TArray<TSharedPtr<FJsonValue>>* EmbeddingsArray;
					if ((*FirstObject)->TryGetArrayField(TEXT("embedding"), EmbeddingsArray))
					{
						for (const TSharedPtr<FJsonValue>& EmbeddingValue : *EmbeddingsArray)
						{
							EmbeddingVector.Add(static_cast<float>(EmbeddingValue->AsNumber()));
						}
					}
				}
			}

			FEmbeddingResult Result{EmbeddingVector};
			OnResponseReceived.ExecuteIfBound(Result, TEXT(""), true);
			OnResponseReceivedF.ExecuteIfBound(Result, TEXT(""), true);
		}
		else
		{
			OnResponseReceived.ExecuteIfBound({}, TEXT("Failed to parse response"), false);
			OnResponseReceivedF.ExecuteIfBound({}, TEXT("Failed to parse response"), false);
		}
	}
	else
	{
		FString ErrorMessage = Response.IsValid() ? TEXT("HTTP request failed: ") + Response->GetContentAsString() : TEXT("HTTP request failed: No response from server");
		OnResponseReceived.ExecuteIfBound({}, ErrorMessage, false);
		OnResponseReceivedF.ExecuteIfBound({}, ErrorMessage, false);
	}
}

UOpenAIEmbedding* UOpenAIEmbedding::Embedding(const FEmbeddingSettings& EmbeddingSettings,
	TFunction<void(const FEmbeddingResult& Result, const FString& ErrorMessage, bool Success)> Callback)
{
	UOpenAIEmbedding* OpenAIEmbeddingInstance = CreateEmbeddingInstance();
	OpenAIEmbeddingInstance->Init(EmbeddingSettings);

	OpenAIEmbeddingInstance->AddToRoot();
	
	// Use Lambda as the callback function
	auto OnResponseCallback = [Callback, OpenAIEmbeddingInstance](const FEmbeddingResult& Message, const FString& ErrorMessage, bool Success)
	{
		// Automatically print ErrorMessage if not successful
		if (!Success)
		{
			UE_LOG(LogEmbedding, Error, TEXT("Embedding request failed. Error: %s"), *ErrorMessage);
		}

		if (Callback)
		{
			// Call the provided callback function
			Callback(Message, ErrorMessage, Success);
		}
		else
		{
			// Handle the case where Callback is null (optional)
			UE_LOG(LogEmbedding, Error, TEXT("Embedding Callback is null."));
		}
		
		OpenAIEmbeddingInstance->RemoveFromRoot();
		
		// Destroy OpenAIEmbeddingInstance after receiving the response
		OpenAIEmbeddingInstance->ConditionalBeginDestroy();
	};

	// Bind the lambda callback function to OnResponseReceived delegate
	OpenAIEmbeddingInstance->OnResponseReceivedF.BindLambda(OnResponseCallback);

	// Start the Embedding
	OpenAIEmbeddingInstance->StartEmbedding();

	return OpenAIEmbeddingInstance;
}

void UOpenAIEmbedding::HandleRequestProgress(FHttpRequestPtr Request, int32 BytesSent, int32 BytesReceived)
{
	UE_LOG(LogEmbedding, Log, TEXT("UOpenAIEmbedding Heartbeat - Sent: %d, Received: %d"), BytesSent, BytesReceived);
}
