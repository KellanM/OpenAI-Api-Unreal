// Copyright Kellan Mythen 2023. All rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OpenAIDefinitions.h"

#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

#if PLATFORM_WINDOWS
#include "Runtime/Core/Public/Windows/WindowsPlatformMisc.h"
#endif

#if PLATFORM_MAC
#include "Runtime/Core/Public/Apple/ApplePlatformMisc.h"
#endif

#if PLATFORM_LINUX
#include "Runtime/Core/Public/Linux/LinuxPlatformMisc.h"
#endif

#include "OpenAIUtils.generated.h"

/**
 *
 */
UCLASS()
class OPENAIAPI_API UOpenAIUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()


public:
	UFUNCTION(BlueprintCallable, Category = "OpenAI")
	static void setOpenAIApiKey(FString apiKey);

	static FString getApiKey();

	static FString GetVoiceString(EOAOpenAIVoices Voice);

	UFUNCTION(BlueprintCallable, Category = "OpenAI")
	static void setUseOpenAIApiKeyFromEnvironmentVars(bool bUseEnvVariable);

	static bool getUseApiKeyFromEnvironmentVars();

	static FString GetEnvironmentVariable(FString key);

public:

	UFUNCTION(BlueprintCallable, Category = "OpenAI", meta = (DisplayName = "OpenAI Realtime Call"))
	static UOpenAICallRealtime* OpenAICallRealtime(FString Instructions, FString CreateResponseMessage, EOAOpenAIVoices Voice);

	UFUNCTION(BlueprintCallable, Category = "OpenAI")
	static float HDVectorDotProductSIMD(const FHighDimensionalVector& A, const FHighDimensionalVector& B);

	UFUNCTION(BlueprintCallable, Category = "OpenAI")
	static float HDVectorLengthSIMD(const FHighDimensionalVector& Vector);

	UFUNCTION(BlueprintCallable, Category = "OpenAI")
	static float HDVectorCosineSimilaritySIMD(const FHighDimensionalVector& A, const FHighDimensionalVector& B);

	UFUNCTION(BlueprintCallable, Category = "OpenAI")
	static float HDVectorDotProduct(const FHighDimensionalVector& A, const FHighDimensionalVector& B);

	UFUNCTION(BlueprintCallable, Category = "OpenAI")
	static float HDVectorLength(const FHighDimensionalVector& Vector);

	UFUNCTION(BlueprintCallable, Category = "OpenAI")
	static float HDVectorCosineSimilarity(const FHighDimensionalVector& A, const FHighDimensionalVector& B);
};
