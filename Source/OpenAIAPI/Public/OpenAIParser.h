// Copyright Kellan Mythen 2021. All rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Misc/DateTime.h"
#include "OpenAIDefinitions.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

/**
 * 
 */
class OPENAIAPI_API OpenAIParser
{
public:
	OpenAIParser(const FGPT3Settings&);
	~OpenAIParser();

	FGPT3Settings settings;
	
	FCompletion ParseCompletion(const FJsonObject&);
	FCompletionInfo ParseCompletionInfo(const FJsonObject&);
	FString ParseGeneratedImage(FJsonObject&);
};
