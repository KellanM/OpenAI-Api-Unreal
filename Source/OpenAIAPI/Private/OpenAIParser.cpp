// Copyright Kellan Mythen 2021. All rights Reserved.


#include "OpenAIParser.h"

// Constructor
OpenAIParser::OpenAIParser(const FGPT3Settings& promptSettings)
	: settings(promptSettings)
{
}

//De-constructor
OpenAIParser::~OpenAIParser()
{
}

// parses a single Completion.
FCompletion OpenAIParser::ParseCompletion(const FJsonObject& json)
{
	FCompletion res = {};
	
	res.text = json.GetStringField(TEXT("text")) + settings.injectRestartText;
	res.index = json.GetIntegerField(TEXT("index"));
	res.finishReason = json.GetStringField(TEXT("finish_reason"));
	
	// parses the completion's log probs
	if (json.HasField("logprobs"))
	{
		FLogProbs Logprobs;

		auto LogProbsObject = json.GetObjectField(TEXT("logprobs"));
		auto TokensObject = LogProbsObject->GetArrayField(TEXT("tokens"));
		for (TSharedPtr<FJsonValue>& elem : TokensObject)
		{
			FString token = *elem->AsString();
			Logprobs.tokens.Add(token);
		}

		auto TokensLogprobsObject = LogProbsObject->GetArrayField(TEXT("token_logprobs"));
		for (TSharedPtr<FJsonValue>& elem : TokensLogprobsObject)
		{
			float Token_LogProb = elem->AsNumber();
			Logprobs.token_logprobs.Add(Token_LogProb);
		}

		auto TextOffsetObject = LogProbsObject->GetArrayField(TEXT("text_offset"));
		for (TSharedPtr<FJsonValue>& elem : TextOffsetObject)
		{
			int32 TextOffset = elem->AsNumber();
			Logprobs.text_offset.Add(TextOffset);
		}

		res.logProbs = Logprobs;
	}

	return res;
}

// parses the response info
FCompletionInfo OpenAIParser::ParseCompletionInfo(const FJsonObject& json)
{
	FCompletionInfo res = {};

	res.id = json.GetStringField("id");
	res.object = json.GetStringField("object");
	res.created = FDateTime::FromUnixTimestamp(json.GetNumberField("created"));
	res.model = json.GetStringField("model");

	return res;
}
// parses a single Generated Image.
FString OpenAIParser::ParseGeneratedImage(FJsonObject& json)
{
	FString res = "";
	res = json.GetStringField(TEXT("url"));

	return res;
}
