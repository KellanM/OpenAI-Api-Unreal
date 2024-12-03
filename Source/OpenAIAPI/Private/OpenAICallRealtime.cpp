// OpenAICallRealtime.cpp

#include "OpenAICallRealtime.h"
#include "OpenAIUtils.h"
#include "OpenAIAudioCapture.h"
#include "WebSocketsModule.h"
#include "JsonUtilities.h"
#include "Sound/SoundWaveProcedural.h"
#include "AudioDevice.h"
#include "Async/Async.h"
#include "Kismet/GameplayStatics.h"

struct FWavHeader
{
    char ChunkID[4];       // "RIFF"
    uint32 ChunkSize;
    char Format[4];        // "WAVE"
    char Subchunk1ID[4];   // "fmt "
    uint32 Subchunk1Size;  // 16 for PCM
    uint16 AudioFormat;    // 1 for PCM
    uint16 NumChannels;
    uint32 SampleRate;
    uint32 ByteRate;
    uint16 BlockAlign;
    uint16 BitsPerSample;
    char Subchunk2ID[4];   // "data"
    uint32 Subchunk2Size;
};

UOpenAICallRealtime::UOpenAICallRealtime()
{
    AudioCaptureComponent = nullptr;
    AudioComponent = nullptr;
    UE_LOG(LogTemp, Log, TEXT("UOpenAICallRealtime constructed"));
}

UOpenAICallRealtime::~UOpenAICallRealtime()
{
    UE_LOG(LogTemp, Log, TEXT("UOpenAICallRealtime destructor called"));
    // Ensure all references are cleared
    AudioCaptureComponent = nullptr;
    WebSocket.Reset();
}

void UOpenAICallRealtime::BeginDestroy()
{
    UE_LOG(LogTemp, Log, TEXT("UOpenAICallRealtime BeginDestroy called"));
    StopRealtimeSession();
    Super::BeginDestroy();
}

void UOpenAICallRealtime::CreateWavHeader(const TArray<uint8>& AudioData, TArray<uint8>& OutWavData, uint32 SampleRate, uint16 NumChannels, uint16 BitsPerSample)
{
    FWavHeader WavHeader;

    // RIFF Chunk
    FMemory::Memcpy(WavHeader.ChunkID, "RIFF", 4);
    WavHeader.ChunkSize = 36 + AudioData.Num(); // 4 + (8 + Subchunk1Size) + (8 + Subchunk2Size)
    FMemory::Memcpy(WavHeader.Format, "WAVE", 4);

    // fmt Subchunk
    FMemory::Memcpy(WavHeader.Subchunk1ID, "fmt ", 4);
    WavHeader.Subchunk1Size = 16;
    WavHeader.AudioFormat = 1; // PCM
    WavHeader.NumChannels = NumChannels;
    WavHeader.SampleRate = SampleRate;
    WavHeader.BitsPerSample = BitsPerSample;
    WavHeader.ByteRate = WavHeader.SampleRate * WavHeader.NumChannels * WavHeader.BitsPerSample / 8;
    WavHeader.BlockAlign = WavHeader.NumChannels * WavHeader.BitsPerSample / 8;

    // data Subchunk
    FMemory::Memcpy(WavHeader.Subchunk2ID, "data", 4);
    WavHeader.Subchunk2Size = AudioData.Num();

    // Append header to WavData
    OutWavData.Append(reinterpret_cast<uint8*>(&WavHeader), sizeof(FWavHeader));

    // Append audio data
    OutWavData.Append(AudioData);
}

UOpenAICallRealtime* UOpenAICallRealtime::OpenAICallRealtime(
    FString Instructions,
    FString CreateResponseMessage,
    EOAOpenAIVoices Voice,
    float VadThreshold,
    int32 SilenceDurationMs,
    int32 PrefixPaddingMs)
{
    UOpenAICallRealtime* Node = NewObject<UOpenAICallRealtime>();
    Node->SessionInstructions = Instructions;
    Node->CreateResponseMessage = CreateResponseMessage;
    Node->SelectedVoice = Voice;
    Node->VadThreshold = VadThreshold;
    Node->SilenceDurationMs = SilenceDurationMs;
    Node->PrefixPaddingMs = PrefixPaddingMs;
    UE_LOG(LogTemp, Log, TEXT("OpenAICallRealtime created with instructions: %s and voice: %d"), *Instructions, static_cast<int>(Voice));
    return Node;
}

void UOpenAICallRealtime::Activate()
{
    UE_LOG(LogTemp, Log, TEXT("UOpenAICallRealtime::Activate called"));
    StartRealtimeSession();
}

void UOpenAICallRealtime::StartRealtimeSession()
{
    UE_LOG(LogTemp, Log, TEXT("StartRealtimeSession called"));
    InitializeWebSocket();

    // Create and initialize the audio capture component
    AudioCaptureComponent = NewObject<UOpenAIAudioCapture>(this);
    if (AudioCaptureComponent)
    {
        AudioCaptureComponent->RegisterComponent();
        UE_LOG(LogTemp, Log, TEXT("AudioCaptureComponent created and registered"));

        AudioCaptureComponent->Activate(false);

        // Bind the audio buffer captured event
        AudioCaptureComponent->OnAudioBufferCaptured.AddDynamic(
            this, &UOpenAICallRealtime::OnAudioBufferCaptured);
        UE_LOG(LogTemp, Log, TEXT("OnAudioBufferCaptured event bound"));

        // Start capturing audio
        AudioCaptureComponent->StartCapturing();
        UE_LOG(LogTemp, Log, TEXT("Audio capture started"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create AudioCaptureComponent"));
    }
}

void UOpenAICallRealtime::StopRealtimeSession()
{
    if (bSessionStopped)
    {
        UE_LOG(LogTemp, Warning, TEXT("StopRealtimeSession already called"));
        return;
    }

    bSessionStopped = true;

    UE_LOG(LogTemp, Log, TEXT("StopRealtimeSession called"));

    if (!IsInGameThread())
    {
        UE_LOG(LogTemp, Warning, TEXT("StopRealtimeSession called off GameThread. Executing on GameThread."));
        AsyncTask(ENamedThreads::GameThread, [this]()
        {
            StopRealtimeSession();
        });
        return;
    }

    // Stop capturing audio
    if (IsValid(AudioCaptureComponent))
    {
        AudioCaptureComponent->StopCapturing();
        AudioCaptureComponent->DestroyAudioCapture();
        AudioCaptureComponent->OnAudioBufferCaptured.RemoveDynamic(this, &UOpenAICallRealtime::OnAudioBufferCaptured);
        AudioCaptureComponent->DestroyComponent();
        AudioCaptureComponent = nullptr;
    }

    // Close WebSocket connection
    if (WebSocket.IsValid())
    {
        UE_LOG(LogTemp, Log, TEXT("Closing WebSocket"));
        WebSocket->Close();
        WebSocket.Reset();
        UE_LOG(LogTemp, Log, TEXT("WebSocket closed and reset"));
    }

    UE_LOG(LogTemp, Log, TEXT("Realtime session stopped successfully"));
}

void UOpenAICallRealtime::InitializeWebSocket()
{
    UE_LOG(LogTemp, Log, TEXT("InitializeWebSocket called"));
    FString ApiKey = UOpenAIUtils::getApiKey();
    if (ApiKey.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("API key is not set"));
        OnResponseReceived.Broadcast(TEXT("API key is not set"), false);
        return;
    }

    FString Url = TEXT("wss://api.openai.com/v1/realtime")
                  TEXT("?model=gpt-4o-realtime-preview-2024-10-01");
    UE_LOG(LogTemp, Log, TEXT("WebSocket URL: %s"), *Url);

    // Create a TMap for headers
    TMap<FString, FString> Headers;
    Headers.Add(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *ApiKey));
    Headers.Add(TEXT("OpenAI-Beta"), TEXT("realtime=v1"));

    // Create the WebSocket with headers
    WebSocket = FWebSocketsModule::Get().CreateWebSocket(Url, TEXT(""), Headers);

    if (WebSocket.IsValid())
    {
        UE_LOG(LogTemp, Log, TEXT("WebSocket created successfully"));
        WebSocket->OnConnected().AddUObject(
            this, &UOpenAICallRealtime::OnWebSocketConnected);
        WebSocket->OnConnectionError().AddUObject(
            this, &UOpenAICallRealtime::OnWebSocketConnectionError);
        WebSocket->OnClosed().AddUObject(
            this, &UOpenAICallRealtime::OnWebSocketClosed);
        WebSocket->OnMessage().AddUObject(
            this, &UOpenAICallRealtime::OnWebSocketMessage);

        WebSocket->Connect();
        UE_LOG(LogTemp, Log, TEXT("WebSocket connection initiated"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create WebSocket"));
        OnResponseReceived.Broadcast(TEXT("Failed to create WebSocket"), false);
    }
}

void UOpenAICallRealtime::OnWebSocketConnected()
{
    UE_LOG(LogTemp, Log, TEXT("WebSocket connected"));
     // Clamp the threshold value
    float ClampedThreshold = FMath::Clamp(VadThreshold, 0.0f, 1.0f);

    // Format the threshold with exactly 6 decimal places
    FString FormattedThreshold = FString::Printf(TEXT("%.6f"), ClampedThreshold);

    // Construct the entire message as a string
    FString SessionUpdateEvent = FString::Printf(TEXT(R"({
        "type": "session.update",
        "session": {
            "modalities": ["text", "audio"],
            "instructions": "%s",
            "voice": "%s",
            "input_audio_format": "pcm16",
            "output_audio_format": "pcm16",
            "input_audio_transcription": {
                "model": "whisper-1"
            },
            "turn_detection": {
                "type": "server_vad",
                "threshold": %s,
                "prefix_padding_ms": %d,
                "silence_duration_ms": %d
            },
            "tools": [],
            "tool_choice": "auto"
        }
    })"),
        *SessionInstructions.ReplaceCharWithEscapedChar(),
        *UOpenAIUtils::GetVoiceString(SelectedVoice),
        *FormattedThreshold,
        PrefixPaddingMs,
        SilenceDurationMs
    );

    UE_LOG(LogTemp, Log, TEXT("Sending Session Update Event: %s"), *SessionUpdateEvent);
    WebSocket->Send(SessionUpdateEvent);

    if (!CreateResponseMessage.IsEmpty()) {
        // Construct the modalities array as a string
        FString ModalitiesString = TEXT("\"text\", \"audio\"");  // Adjust if needed

        // Construct the entire response.create event as a string
        FString ResponseCreateEvent = FString::Printf(TEXT(R"({
            "type": "response.create",
            "response": {
                "instructions": "%s",
                "modalities": [%s]
            }
        })"),
            *CreateResponseMessage.ReplaceCharWithEscapedChar(),
            *ModalitiesString
        );

        UE_LOG(LogTemp, Log, TEXT("Sending Response Create Event: %s"), *ResponseCreateEvent);
        WebSocket->Send(ResponseCreateEvent);
        UE_LOG(LogTemp, Log, TEXT("Response create event sent"));
    } else {
        UE_LOG(LogTemp, Log, TEXT("No create response message provided, skipping response create event"));
    }
}

void UOpenAICallRealtime::OnWebSocketConnectionError(const FString& Error)
{
    UE_LOG(LogTemp, Error, TEXT("WebSocket Connection Error: %s"), *Error);
    OnResponseReceived.Broadcast(
        FString::Printf(TEXT("WebSocket Connection Error: %s"), *Error),
        false);
}

void UOpenAICallRealtime::OnWebSocketClosed(int32 StatusCode,
                                            const FString& Reason,
                                            bool bWasClean)
{
    UE_LOG(LogTemp, Log, TEXT("WebSocket Closed: StatusCode=%d, Reason=%s, WasClean=%d"), StatusCode, *Reason, bWasClean);
    OnResponseReceived.Broadcast(
        FString::Printf(TEXT("WebSocket Closed: %s"), *Reason),
        false);
}

void UOpenAICallRealtime::OnWebSocketMessage(const FString& Message)
{
    FString TruncatedMessage = Message.Left(200);
    if (Message.Len() > 200)
    {
        TruncatedMessage += TEXT("...");
    }

    // Parse and handle incoming messages
    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader =
        TJsonReaderFactory<>::Create(Message);

    if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
    {
        FString EventType = JsonObject->GetStringField(TEXT("type"));
        UE_LOG(LogTemp, Log, TEXT("===> Event Type: %s"), *EventType);

        if (EventType == TEXT("response.text.delta"))
        {
            // Extract text delta
            FString TextDelta = JsonObject->GetStringField(TEXT("text"));
            UE_LOG(LogTemp, Log, TEXT("Text Delta: %s"), *TextDelta);
            AsyncTask(ENamedThreads::GameThread, [this, TextDelta]()
            {
                  OnResponseReceived.Broadcast(TextDelta, true);
            });
        }
        else if (EventType == TEXT("response.audio_transcript.delta"))
        {
            FString AudioTranscriptDelta = JsonObject->GetStringField(TEXT("delta"));
            UE_LOG(LogTemp, Log, TEXT("Audio Transcript Delta: %s"), *AudioTranscriptDelta);
        }
        else if (EventType == TEXT("response.audio.delta"))
        {
            FString AudioBase64 = JsonObject->GetStringField(TEXT("delta"));
            TArray<uint8> AudioData;
            FBase64::Decode(AudioBase64, AudioData);
            UE_LOG(LogTemp, Log, TEXT("Audio Delta received, size: %d bytes"), AudioData.Num());

            AsyncTask(ENamedThreads::GameThread, [this, AudioData]()
            {
                PlayAudioData(AudioData);
            });
        }
        else if (EventType == TEXT("input_audio_buffer.speech_started"))
        {
            UE_LOG(LogTemp, Log, TEXT("-------------------------------------------------> Response was cancelled due to turn_detected"));
            // Ensure the delegate is called on the game thread
            AsyncTask(ENamedThreads::GameThread, [this]()
            {
                OnCancelAudioReceived.Broadcast(true);
            });
        }
        else if (EventType == TEXT("error"))
        {
            // Handle error
            TSharedPtr<FJsonObject> ErrorObject =
                JsonObject->GetObjectField(TEXT("error"));
            FString ErrorMessage = ErrorObject->GetStringField(TEXT("message"));
            UE_LOG(LogTemp, Error, TEXT("Error received: %s"), *ErrorMessage);
            OnResponseReceived.Broadcast(ErrorMessage, false);
        }
        // Handle other event types as needed
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to parse WebSocket message"));
    }
}

void UOpenAICallRealtime::SendRealtimeEvent(
    const TSharedPtr<FJsonObject>& Event, bool isAudioStreamEvent)
{
    FString EventString;
    TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> Writer =
        TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&EventString);
    FJsonSerializer::Serialize(Event.ToSharedRef(), Writer);

    if (!isAudioStreamEvent) {
        UE_LOG(LogTemp, Log, TEXT("Full JSON Event: %s"), *EventString);
    }

    FString TruncatedEventString = EventString.Left(2000);
    if (EventString.Len() > 2000)
    {
        TruncatedEventString += TEXT("...");
    }

    if (!isAudioStreamEvent) {
        UE_LOG(LogTemp, Log, TEXT("Sending Realtime Event: %s"), *TruncatedEventString);
    }

    WebSocket->Send(EventString);
}

void UOpenAICallRealtime::OnAudioBufferCaptured(
    const TArray<float>& AudioBuffer)
{
    //UE_LOG(LogTemp, Log, TEXT("Audio Buffer Captured, size: %d samples"), AudioBuffer.Num());
    // Send the audio data to the OpenAI API
    SendAudioDataToAPI(AudioBuffer);

    // Broadcast the audio buffer to Blueprints
    //OnAudioBufferReceived.Broadcast(AudioBuffer);
}

void UOpenAICallRealtime::SendAudioDataToAPI(
    const TArray<float>& AudioBuffer)
{
    UE_LOG(LogTemp, Log, TEXT("Audio out -> %d samples"), AudioBuffer.Num());
    // Convert float audio data to PCM16
    TArray<uint8> PCM16Data;
    PCM16Data.SetNumUninitialized(AudioBuffer.Num() * sizeof(int16));

    for (int32 i = 0; i < AudioBuffer.Num(); ++i)
    {
        float Sample = FMath::Clamp(AudioBuffer[i], -1.0f, 1.0f);
        int16 IntSample = (int16)(Sample * 32767.0f);
        PCM16Data[i * 2] = IntSample & 0xFF;
        PCM16Data[i * 2 + 1] = (IntSample >> 8) & 0xFF;
    }

    // Base64 encode the PCM16 data
    FString Base64Audio = FBase64::Encode(PCM16Data);
    //UE_LOG(LogTemp, Log, TEXT("Audio data encoded to Base64, length: %d"), Base64Audio.Len());

    // Create the input_audio_buffer.append event
    TSharedPtr<FJsonObject> AudioEvent =
        MakeShareable(new FJsonObject());
    AudioEvent->SetStringField(TEXT("type"),
                               TEXT("input_audio_buffer.append"));
    AudioEvent->SetStringField(TEXT("audio"), Base64Audio);

    SendRealtimeEvent(AudioEvent, true);
    //UE_LOG(LogTemp, Log, TEXT("Audio data sent to API"));
}

void UOpenAICallRealtime::PlayAudioData(const TArray<uint8>& AudioData)
{
    TArray<uint8> ProcessedData;

    //CreateWavHeader(AudioData, ProcessedData);

    // Broadcast the processed data
    //OnAudioDataReceived.Broadcast(ProcessedData);
    OnAudioDataReceived.Broadcast(AudioData);
}
