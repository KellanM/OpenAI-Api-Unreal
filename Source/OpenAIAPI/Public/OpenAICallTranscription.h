// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "OpenAIDefinitions.h"
#include "HttpModule.h"
#include "OpenAICallTranscription.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnTranscriptionResponseRecievedPin, const FString&, transcription, const FString&, errorMessage, const FTranscriptionInfo&, transcriptionInfo, bool, Success);

/**
 * 
 */
UCLASS()
class OPENAIAPI_API UOpenAICallTranscription : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UOpenAICallTranscription();
	~UOpenAICallTranscription();

	EOATranscriptionEngineType engine = EOATranscriptionEngineType::WHISPER_1;
	USoundWave* audio;
	FTranscriptionSettings settings;

	UPROPERTY(BlueprintAssignable, Category = "OpenAI")
		FOnTranscriptionResponseRecievedPin Finished;

private:
	OpenAIValueMapping mapping;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "OpenAI")
		static UOpenAICallTranscription* OpenAICallTranscription(EOATranscriptionEngineType engine, USoundWave* audio, FTranscriptionSettings settings);

	virtual void Activate() override;
	void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

	TArray<uint8> Conv_SoundWaveToWavBytes(USoundWave*);
	FString AddData(FString Name, FString Value, FString BoundaryBegin);
};


