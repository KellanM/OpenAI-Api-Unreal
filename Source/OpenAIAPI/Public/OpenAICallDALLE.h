// Copyright Kellan Mythen 2021. All rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "OpenAIDefinitions.h"
#include "HttpModule.h"
#include "OpenAICallDALLE.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnDalleResponseRecievedPin, const TArray<FString>&, generatedImageUrls, const FString&, errorMessage, bool, Success);

/**
 * 
 */
UCLASS()
class OPENAIAPI_API UOpenAICallDALLE : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UOpenAICallDALLE();
	~UOpenAICallDALLE();

	EOAImageSize imageSize = EOAImageSize::LARGE;
	FString prompt = "";
	int32 numImages = 1;
	FGPT3Settings settings;

	UPROPERTY(BlueprintAssignable, Category = "OpenAI")
		FOnDalleResponseRecievedPin Finished;

private:
	OpenAIValueMapping mapping;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "OpenAI")
		static UOpenAICallDALLE* OpenAICallDALLE(EOAImageSize imageSize, FString prompt, int32 numImages);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);
};
