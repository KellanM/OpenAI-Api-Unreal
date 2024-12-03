#include "OpenAIAudioCapture.h"
#include "Kismet/GameplayStatics.h"

UOpenAIAudioCapture::UOpenAIAudioCapture()
{
    PrimaryComponentTick.bCanEverTick = false;
    AudioCapture = nullptr;
    bIsCapturing = false;
    bAutoActivate = true;
    UE_LOG(LogTemp, Log, TEXT("UOpenAIAudioCapture constructor called"));
}


void UOpenAIAudioCapture::Activate(bool bReset)
{
    UE_LOG(LogTemp, Log, TEXT("UOpenAIAudioCapture Activate called"));
    Super::Activate(bReset);

    // Move initialization logic from BeginPlay to here
    if (!AudioCapture)
    {
        AudioCapture = NewObject<UAudioCapture>(this);
        if (AudioCapture)
        {
            AudioCapture->AddGeneratorDelegate([this](const float* InAudio, int32 NumSamples) {
                this->OnAudioGenerate(InAudio, NumSamples);
            });

            AudioCapture->OpenDefaultAudioStream();
            StartCapturing();
            UE_LOG(LogTemp, Log, TEXT("-------------------> AudioCapture started from Activate on GameThread"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to create AudioCapture object"));
        }
    } else {
        UE_LOG(LogTemp, Log, TEXT("AudioCapture is already created"));
    }
}

void UOpenAIAudioCapture::StartCapturing()
{
    if (AudioCapture && !bIsCapturing)
    {
        if (AudioCapture) {
            UE_LOG(LogTemp, Log, TEXT("AudioCapture is valid"));
            AudioCapture->StartCapturingAudio();
            bIsCapturing = true;
            UE_LOG(LogTemp, Log, TEXT("Audio capture started successfully"));
        } else {
            UE_LOG(LogTemp, Log, TEXT("AudioCapture is null"));
        }
    } else {
        UE_LOG(LogTemp, Log, TEXT("AudioCapture is null or already capturing"));
    }
}

void UOpenAIAudioCapture::StopCapturing()
{
    UE_LOG(LogTemp, Log, TEXT("Attempting to stop audio capture"));
    if (AudioCapture)
    {
        UE_LOG(LogTemp, Log, TEXT("AudioCapture is valid"));
        AudioCapture->StopCapturingAudio();
         bIsCapturing = false;
         ProcessAndBroadcastBuffer(); // Broadcast any remaining data

        UE_LOG(LogTemp, Log, TEXT("Audio capture stopped successfully"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("AudioCapture is null"));
    }
}

void UOpenAIAudioCapture::DestroyAudioCapture() {
    UE_LOG(LogTemp, Log, TEXT("DestroyAudioCapture called"));
    if (AudioCapture)
    {
        AudioCapture->ConditionalBeginDestroy();
        AudioCapture = nullptr;
        UE_LOG(LogTemp, Log, TEXT("AudioCapture destroyed"));
    } else {
        UE_LOG(LogTemp, Log, TEXT("AudioCapture is null"));
    }
}

void UOpenAIAudioCapture::OnAudioGenerate(const float* InAudio, int32 NumSamples)
{
    if (InAudio) {
        if (bIsCapturing)
        {
            const int32 DownsampleFactor = 2; // Factor by which to downsample
            const int32 InterpolatedSamples = NumSamples / DownsampleFactor;

            for (int32 i = 0; i < InterpolatedSamples - 1; i++)
            {
               // Indices for original samples
                int32 index1 = i * DownsampleFactor;
                int32 index2 = index1 + DownsampleFactor;

                // Boundary check to prevent accessing out-of-range samples
                if (index2 >= NumSamples)
                {
                    // Handle the last sample if NumSamples is not perfectly divisible
                    index2 = NumSamples - 1;
                }

                // Original sample values
                float sample1 = InAudio[index1];
                float sample2 = InAudio[index2];

                // Calculate the interpolated sample using linear interpolation
                float interpolatedSample = sample1 + ((sample2 - sample1) * 0.5f);

                // Add the interpolated sample to the AudioBuffer
                AudioBuffer.Add(interpolatedSample);
            }
            double CurrentTime = FPlatformTime::Seconds();
            if (CurrentTime - LastBroadcastTime >= MaxBufferTime || AudioBuffer.Num() >= MaxBufferSize)
            {
                ProcessAndBroadcastBuffer();
                LastBroadcastTime = CurrentTime;
            }

        } else {
            UE_LOG(LogTemp, Log, TEXT("bIsCapturing is false"));
        }
    } else {
        UE_LOG(LogTemp, Log, TEXT("InAudio is null"));
    }
}

void UOpenAIAudioCapture::ProcessAndBroadcastBuffer()
{
    TArray<float> BufferCopy;
    {
        if (AudioBuffer.Num() > 0)
        {
            BufferCopy = AudioBuffer;
            AudioBuffer.Empty();
        }
    }

    if (BufferCopy.Num() > 0)
    {
        // Broadcast the captured audio data
        OnAudioBufferCaptured.Broadcast(BufferCopy);
    }
}