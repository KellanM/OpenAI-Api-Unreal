// Copyright Kellan Mythen 2021. All rights Reserved.


#include "OpenAIUtils.h"
#include "OpenAIDefinitions.h"
#include "OpenAIAPI.h"
#include "Modules/ModuleManager.h"

void UOpenAIUtils::setOpenAIApiKey(FString apiKey)
{
	FOpenAIAPIModule& mod = FModuleManager::Get().LoadModuleChecked<FOpenAIAPIModule>("OpenAIAPI");
	mod._apiKey = apiKey;
}

FString UOpenAIUtils::getApiKey()
{
	FOpenAIAPIModule& mod = FModuleManager::Get().LoadModuleChecked<FOpenAIAPIModule>("OpenAIAPI");
	return mod._apiKey;
}

void UOpenAIUtils::setUseOpenAIApiKeyFromEnvironmentVars(bool bUseEnvVariable)
{
	FOpenAIAPIModule& mod = FModuleManager::Get().LoadModuleChecked<FOpenAIAPIModule>("OpenAIAPI");
	mod._useApiKeyFromEnvVariable = bUseEnvVariable;
}

bool UOpenAIUtils::getUseApiKeyFromEnvironmentVars()
{

	FOpenAIAPIModule& mod = FModuleManager::Get().LoadModuleChecked<FOpenAIAPIModule>("OpenAIAPI");
	return mod._useApiKeyFromEnvVariable;
}

FString UOpenAIUtils::GetEnvironmentVariable(FString key)
{
	FString result;
#if PLATFORM_WINDOWS
	result = FWindowsPlatformMisc::GetEnvironmentVariable(*key);
#endif
#if PLATFORM_MAC
	result = FApplePlatformMisc::GetEnvironmentVariable(*key);
#endif

#if PLATFORM_LINUX
	result = FLinuxPlatformMisc::GetEnvironmentVariable(*key);
#endif
	return result;
}

