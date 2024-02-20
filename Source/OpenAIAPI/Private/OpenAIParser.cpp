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

OpenAIParser::OpenAIParser(const FSpeechSettings& settings)
	: speechSettings(settings)
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

FSpeechCompletion OpenAIParser::ParseSpeechCompletion(const FJsonObject&)
{
	/*
*Create speech
POST

https://api.openai.com/v1/audio/speech

Generates audio from the input text.

Request body
model
string
Required
One of the available TTS models: tts-1 or tts-1-hd

input
string
Required
The text to generate audio for. The maximum length is 4096 characters.

voice
string
Required
The voice to use when generating the audio. Supported voices are alloy, echo, fable, onyx, nova, and shimmer.

response_format
string
Optional
Defaults to mp3
The format to audio in. Supported formats are mp3, opus, aac, and flac.

speed
number
Optional
Defaults to 1
The speed of the generated audio. Select a value from 0.25 to 4.0. 1.0 is the default.

Returns
The audio file content.
 */

///	FString res = "";

	// Handle the received audio data
	//TArray<uint8> ReceivedData = Response->GetContent();
            
	// For demonstration purposes, write the data to a file on disk
	//FFileHelper::SaveArrayToFile(ReceivedData, *FPaths::ProjectSavedDir() / TEXT("ReceivedAudio.mp3"));

	return {};
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
