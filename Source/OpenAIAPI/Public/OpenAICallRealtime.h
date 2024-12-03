// OpenAICallRealtime.h

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "OpenAIDefinitions.h"
#include "IWebSocket.h"
#include "Components/AudioComponent.h"
#include "OpenAIAudioCapture.h"
#include "Sound/SoundWaveProcedural.h"
#include "OpenAICallRealtime.generated.h"

// Delegate for receiving text responses
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FOnRealtimeResponseReceivedPin,
    const FString&, Response,
    bool, bSuccess);

// Delegate for receiving audio buffers for lip-sync
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnAudioBufferReceived,
    const TArray<float>&, AudioBuffer);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnAudioDataReceived,
    const TArray<uint8>&, AudioData);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnRealtimeCancelAudioReceivedPin,
    bool, bWasCancelled);

UCLASS()
class OPENAIAPI_API UOpenAICallRealtime : public UBlueprintAsyncActionBase
{
    GENERATED_BODY()

public:
    UOpenAICallRealtime();
    ~UOpenAICallRealtime();

    // Static factory function
    UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "OpenAI")
    static UOpenAICallRealtime* OpenAICallRealtime(
        FString Instructions,
        FString CreateResponseMessage,

        EOAOpenAIVoices Voice,
        float vadThreshold = 0.5,
        int32 SilenceDurationMs = 500,
        int32 PrefixPaddingMs = 300);

    UPROPERTY(BlueprintAssignable, Category = "OpenAI|Realtime")
    FOnAudioDataReceived OnAudioDataReceived;

    // Override Activate function
    virtual void Activate() override;

    // Stop the realtime session
    UFUNCTION(BlueprintCallable, Category = "OpenAI")
    void StopRealtimeSession();
    virtual void BeginDestroy() override;

    // Delegate called when a text response is received
    UPROPERTY(BlueprintAssignable, Category = "OpenAI|Realtime")
    FOnRealtimeResponseReceivedPin OnResponseReceived;

    UPROPERTY(BlueprintAssignable, Category = "OpenAI|Realtime")
    FOnRealtimeCancelAudioReceivedPin OnCancelAudioReceived;

    // Delegate called when an audio buffer is captured
    //UPROPERTY(BlueprintAssignable, Category = "OpenAI|Realtime")
    FOnAudioBufferReceived OnAudioBufferReceived;

private:
    bool bHasSentWavHeader = false;
    int32 numberOfSentAudioBuffers = 0;
    bool bSessionStopped = false;

    // WebSocket connection
    TSharedPtr<IWebSocket> WebSocket;

    // Audio capture component
    UPROPERTY()
    class UOpenAIAudioCapture* AudioCaptureComponent;

    // Procedural sound wave for playing received audio
    UPROPERTY()
    USoundWaveProcedural* GeneratedSoundWave;

    // Audio component for playback
    UPROPERTY()
    UAudioComponent* AudioComponent;

    // Instructions and voice selection
    FString SessionInstructions;
    FString CreateResponseMessage;
    EOAOpenAIVoices SelectedVoice;
    float VadThreshold;
    int32 SilenceDurationMs;
    int32 PrefixPaddingMs;

    // Initialize WebSocket connection
    void InitializeWebSocket();

    // Start the realtime session
    void StartRealtimeSession();

    // WebSocket event handlers
    void OnWebSocketConnected();
    void OnWebSocketConnectionError(const FString& Error);
    void OnWebSocketClosed(int32 StatusCode,
                           const FString& Reason,
                           bool bWasClean);
    void OnWebSocketMessage(const FString& Message);

    // Send event to OpenAI Realtime API
    void SendRealtimeEvent(const TSharedPtr<FJsonObject>& Event, bool isAudioStreamEvent = false);

    // Send audio data to OpenAI API
    void SendAudioDataToAPI(const TArray<float>& AudioBuffer);

    // Handle captured audio buffer
    UFUNCTION()
    void OnAudioBufferCaptured(const TArray<float>& AudioBuffer);

    void CreateWavHeader(const TArray<uint8>& AudioData, TArray<uint8>& OutWavData, uint32 SampleRate = 24000, uint16 NumChannels = 1, uint16 BitsPerSample = 16);

    // Play received audio data
    void PlayAudioData(const TArray<uint8>& AudioData);
};
