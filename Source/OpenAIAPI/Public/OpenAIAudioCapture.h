#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AudioCapture.h"
#include "Sound/SampleBufferIO.h"
#include "OpenAIAudioCapture.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAudioBufferCaptured, const TArray<float>&, AudioBuffer);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class OPENAIAPI_API UOpenAIAudioCapture : public UActorComponent
{
    GENERATED_BODY()

public:
    UOpenAIAudioCapture();

    virtual void Activate(bool bReset) override;

    void DestroyAudioCapture();

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void StartCapturing();

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void StopCapturing();

    void ProcessAndBroadcastBuffer();

    UPROPERTY(BlueprintAssignable, Category = "Audio")
    FOnAudioBufferCaptured OnAudioBufferCaptured;

private:
    UPROPERTY()
    UAudioCapture* AudioCapture;

    TArray<float> AudioBuffer;

    bool bIsCapturing;
    double LastBroadcastTime;
    static constexpr float MaxBufferTime = 0.4f; // 400 ms
    static constexpr int32 MaxBufferSize = 12000; // Approximately 24000 bytes


    void OnAudioGenerate(const float* InAudio, int32 NumSamples);
};