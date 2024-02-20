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
	OpenAIParser();
	OpenAIParser(const FCompletionSettings&);
	OpenAIParser(const FChatSettings&);
	OpenAIParser(const FSpeechSettings&);
	~OpenAIParser();

	FCompletionSettings completionSettings;
	
	FChatSettings chatSettings;
	FSpeechSettings speechSettings;

	FCompletion ParseCompletionsResponse(const FJsonObject&);
	FCompletionInfo ParseGPTCompletionInfo(const FJsonObject&);
	FChatCompletion ParseChatCompletion(const FJsonObject&);
	FSpeechCompletion ParseSpeechCompletion (const FJsonObject&);
	FString ParseTranscriptionCompletion(const FJsonObject&);
	FString ParseGeneratedImage(FJsonObject&);
};
