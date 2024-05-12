// Copyright (c) 2024 tobenot, See LICENSE in the project root for license information.

// UOpenAIEmbedding.h

#pragma once

#include "CoreMinimal.h"
#include "HttpModule.h"
#include "OpenAIDefinitions.h"
#include "OpenAIEmbedding.generated.h"

DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnEmbeddingResponseReceivedPin, const FEmbeddingResult&, Result, const FString&, ErrorMessage, bool, Success);
DECLARE_DELEGATE_ThreeParams(FOnEmbeddingResponseReceivedF, const FEmbeddingResult&, const FString&, bool);
DECLARE_LOG_CATEGORY_EXTERN(LogEmbedding, Log, All);

/**
 * 
 */
UCLASS(BlueprintType)
class OPENAIAPI_API UOpenAIEmbedding : public UObject
{
	GENERATED_BODY()

public:
	UOpenAIEmbedding();
	~UOpenAIEmbedding();

	UFUNCTION(BlueprintCallable, Category = "OpenAI")
	static UOpenAIEmbedding* CreateEmbeddingInstance();

	void Init(const FEmbeddingSettings& EmbeddingSettings);

	UFUNCTION(BlueprintCallable, Category = "OpenAI")
	void StartEmbedding();
    
	UFUNCTION(BlueprintCallable, Category = "OpenAI")
	void CancelRequest();

	UPROPERTY()
	FOnEmbeddingResponseReceivedPin OnResponseReceived;
    
	FOnEmbeddingResponseReceivedF OnResponseReceivedF;

private:
	FEmbeddingSettings EmbeddingSettings;

	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

public:
	static UOpenAIEmbedding* Embedding(const FEmbeddingSettings& EmbeddingSettings, TFunction<void(const FEmbeddingResult& Result, const FString& ErrorMessage, bool Success)> Callback);

private:
	void HandleRequestProgress(FHttpRequestPtr Request, int32 BytesSent, int32 BytesReceived);

private:
	TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> CurrentRequest;
};