// Copyright Kellan Mythen 2023. All rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "OpenAIDefinitions.h"
#include "HttpModule.h"
#include "OpenAICallCompletions.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnGptResponseRecievedPin, const TArray<FCompletion>&, completions, const FString&, errorMessage, const FCompletionInfo&, completionInfo, bool, Success);

/**
 * 
 */
UCLASS()
class OPENAIAPI_API UOpenAICallCompletions : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UOpenAICallCompletions();
	~UOpenAICallCompletions();

	EOACompletionsEngineType engine = EOACompletionsEngineType::TEXT_DAVINCI_002;
	FString prompt = "";
	FCompletionSettings settings;

	UPROPERTY(BlueprintAssignable, Category = "OpenAI")
		FOnGptResponseRecievedPin Finished;

private:
	OpenAIValueMapping mapping;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "OpenAI")
		static UOpenAICallCompletions* OpenAICallCompletions(EOACompletionsEngineType engine, FString prompt, FCompletionSettings settings);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);
};