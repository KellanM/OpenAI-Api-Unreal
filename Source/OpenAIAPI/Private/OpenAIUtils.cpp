// Copyright Kellan Mythen 2023. All rights Reserved.


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

void UOpenAIUtils::	setUseOpenAIApiKeyFromEnvironmentVars(bool bUseEnvVariable)
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

TArray<uint8> UOpenAIUtils::FStringToUint8(const FString& InString)
{
	TArray<uint8> OutBytes;

	// Handle empty strings
	if (InString.Len() > 0)
	{
		FTCHARToUTF8 Converted(*InString); // Convert to UTF8
		OutBytes.Append(reinterpret_cast<const uint8*>(Converted.Get()), Converted.Length());
	}

	return OutBytes;
}

FString UOpenAIUtils::Uint8ToFstring(const TArray<uint8>& InArray)
{
	FString OutString;
	if (!InArray.IsEmpty())
	{
		//FString UTF8String(InArray.Num(), reinterpret_cast<const UTF8CHAR*>(InArray.GetData()));
		FUTF8ToTCHAR Converted(reinterpret_cast<const UTF8CHAR*>(InArray.GetData()), InArray.Num()); // Convert to TCHAR
		OutString = Converted;
	}
	return OutString;
}