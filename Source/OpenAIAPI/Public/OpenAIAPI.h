// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FOpenAIAPIModule : public IModuleInterface
{
	friend class UOpenAIUtils;
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	FString _apiKey = "";
	bool _useApiKeyFromEnvVariable = false;
};
