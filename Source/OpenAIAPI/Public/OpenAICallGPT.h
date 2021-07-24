// Copyright Kellan Mythen 2021. All rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "OpenAIDefinitions.h"
#include "HttpModule.h"
#include "OpenAICallGPT.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnResponseRecievedPin, const TArray<FCompletion>&, completions, const FString&, errorMessage, const FCompletionInfo&, completionInfo, bool, Success);

/**
 * 
 */
UCLASS()
class OPENAIAPI_API UOpenAICallGPT : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()


public:
	UOpenAICallGPT();
	~UOpenAICallGPT();

	EOAEngineType engine = EOAEngineType::DAVINCI;
	FString prompt = "";
	FGPT3Settings settings;

	UPROPERTY(BlueprintAssignable, Category = "OpenAI")
		FOnResponseRecievedPin Finished;

private:
	OpenAIValueMapping mapping;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "OpenAI")
		static UOpenAICallGPT* OpenAICallGPT3(EOAEngineType engine, FString prompt, FGPT3Settings settings);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);
};