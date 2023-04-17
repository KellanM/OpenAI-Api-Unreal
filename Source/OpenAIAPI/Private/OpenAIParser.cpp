// Copyright Kellan Mythen 2023. All rights Reserved.

#include "OpenAIParser.h"
#include "OpenAIUtils.h"
#include "Dom/JsonObject.h"


// Constructor
OpenAIParser::OpenAIParser(const FCompletionSettings& settings)
	: completionSettings(settings)
{
}

OpenAIParser::OpenAIParser(const FChatSettings& settings)
	: chatSettings(settings)
{
}

//De-constructor
OpenAIParser::~OpenAIParser()
{
}

// parses a single Completion.
FCompletion OpenAIParser::ParseCompletionsResponse(const FJsonObject& json)
{
	FCompletion res = {};
	
	res.text = json.GetStringField(TEXT("text")) + completionSettings.injectRestartText;
	res.index = json.GetIntegerField(TEXT("index"));
	json.TryGetStringField(TEXT("finish_reason"), res.finishReason);
	
	return res;
}

// parses the response info
FCompletionInfo OpenAIParser::ParseGPTCompletionInfo(const FJsonObject& json)
{
	FCompletionInfo res = {};

	res.id = json.GetStringField("id");
	res.object = json.GetStringField("object");
	res.created = FDateTime::FromUnixTimestamp(json.GetNumberField("created"));
	res.model = json.GetStringField("model");

	return res;
}

// parses a single Generated messasge.
FChatCompletion OpenAIParser::ParseChatCompletion(const FJsonObject& json)
{
	FChatCompletion res = {};

	FChatLog message;
	message.role = EOAChatRole::ASSISTANT;
	TArray<TSharedPtr<FJsonValue>> choices = json.GetArrayField("choices");
	TSharedPtr<FJsonValue> choice = choices[0];
	TSharedPtr<FJsonObject> messageObject = choice->AsObject()->GetObjectField("message");
	message.content = messageObject->GetStringField("content");
	//res.index = json.GetIntegerField(TEXT("index"));
	json.TryGetStringField(TEXT("finish_reason"), res.finishReason);
	res.message = message;
	
	return res;
}

FString OpenAIParser::ParseTranscriptionCompletion(const FJsonObject& json)
{
	return json.GetStringField("text");
}

// parses a single Generated Image.
FString OpenAIParser::ParseGeneratedImage(FJsonObject& json)
{
	FString res = "";
	res = json.GetStringField(TEXT("url"));

	return res;
}
