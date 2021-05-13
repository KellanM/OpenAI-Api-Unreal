// Copyright Kellan Mythen 2021. All rights Reserved.

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

	UFUNCTION(BlueprintCallable, Category = "OpenAI")
	static void setUseOpenAIApiKeyFromEnvironmentVars(bool bUseEnvVariable);

	static bool getUseApiKeyFromEnvironmentVars();

	static FString GetEnvironmentVariable(FString key);
	
};
