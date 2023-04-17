// Copyright Kellan Mythen 2023. All rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "HttpModule.h"
#include "OpenAICallTranscriptions.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnTranscriptionResponseRecievedPin, const FString, Transcription, const FString&, errorMessage, bool, Success);
/**
 * 
 */
UCLASS()
class OPENAIAPI_API UOpenAICallTranscriptions : public UBlueprintAsyncActionBase
{

	GENERATED_BODY()

public:
	UOpenAICallTranscriptions();
	~UOpenAICallTranscriptions();

	FString fileName;
	
	UPROPERTY(BlueprintAssignable, Category = "OpenAI")
	FOnTranscriptionResponseRecievedPin Finished;

private:
	
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "OpenAI")
		static UOpenAICallTranscriptions* OpenAICallTranscriptions(FString fileName);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);
	
};
