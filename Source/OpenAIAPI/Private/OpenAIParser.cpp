// Copyright Kellan Mythen 2023. All rights Reserved.

#include "OpenAIParser.h"
#include "OpenAIUtils.h"
#include "Dom/JsonObject.h"


// Constructor
OpenAIParser::OpenAIParser(const FGPT3Settings& settings)
	: completionSettings(settings)
{
}

OpenAIParser::OpenAIParser(const FChatSettings& settings)
	: chatSettings(settings)
{
}

OpenAIParser::OpenAIParser(const FTranscriptionSettings& settings)
	: transcriptionSettings(settings)
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
	res.index = json.GetIntegerField(TEXT("index"));
	json.TryGetStringField(TEXT("finish_reason"), res.finishReason);
	res.message = message;
	
	return res;
}

// parses a single Generated Image.
FString OpenAIParser::ParseGeneratedImage(FJsonObject& json)
{
	FString res = "";
	res = json.GetStringField(TEXT("url"));

	return res;
}

// parses the transcription response info
FTranscriptionInfo OpenAIParser::ParseTranscriptionResponse(const FJsonObject& json)
{
	FTranscriptionInfo res = {};
	res.language = json.GetStringField("language");
	res.duration = json.GetNumberField("duration");
	TArray<TSharedPtr<FJsonValue>> segments = json.GetArrayField("segments");
	for (auto& segment : segments)
	{
		auto& segmentObj = segment->AsObject();
		FTranscriptionSegment newEl;
		newEl.id = segmentObj->GetIntegerField("id");
		newEl.seek = segmentObj->GetIntegerField("seek");
		newEl.start = segmentObj->GetNumberField("start");
		newEl.end = segmentObj->GetNumberField("end");
		newEl.text = segmentObj->GetStringField("text");
		newEl.temperature = segmentObj->GetNumberField("temperature");
		newEl.avg_logprob = segmentObj->GetNumberField("avg_logprob");
		newEl.compression_ratio = segmentObj->GetNumberField("compression_ratio");
		newEl.no_speech_prob = segmentObj->GetNumberField("no_speech_prob");
		newEl.transient = segmentObj->GetBoolField("transient");
		auto tokens = segmentObj->GetArrayField("tokens");
		for (TSharedPtr<FJsonValue>& token : tokens)
		{
			newEl.tokens.Add(token->AsNumber());
		}
		res.segments.Add(newEl);
	}

	return res;
}
