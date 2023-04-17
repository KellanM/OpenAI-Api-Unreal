// Copyright Kellan Mythen 2023. All rights Reserved.

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
	OpenAIParser(const FChatSettings&);
	OpenAIParser(const FTranscriptionSettings&);
	~OpenAIParser();

	FGPT3Settings completionSettings;
	
	FChatSettings chatSettings;

	FTranscriptionSettings transcriptionSettings;

	FCompletion ParseCompletionsResponse(const FJsonObject&);
	FCompletionInfo ParseGPTCompletionInfo(const FJsonObject&);
	FChatCompletion ParseChatCompletion(const FJsonObject&);
	FString ParseGeneratedImage(FJsonObject&);
	FTranscriptionInfo ParseTranscriptionResponse(const FJsonObject&);
};
