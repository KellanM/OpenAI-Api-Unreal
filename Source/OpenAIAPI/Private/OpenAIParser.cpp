// Copyright Kellan Mythen 2021. All rights Reserved.


#include "OpenAIParser.h"

//Constructor
OpenAIParser::OpenAIParser()
{
}
//De-constructor
OpenAIParser::~OpenAIParser()
{
}

// returns an array of Completions.
TArray<FCompletion> OpenAIParser::ParseCompletions(FJsonObject json)
{
	TArray<FCompletion> Completions;

	if (json.HasField(TEXT("Choices")))
	{
		auto CompletionsObject = json.GetArrayField(TEXT("Choices"));
		for(TSharedPtr<FJsonValue> &elem : CompletionsObject)
		{
			FCompletion Completion = ParseCompletion(*elem->AsObject());
			Completions.Add(Completion);
		}
	}
	return Completions;
}

// parses a single Completion.
FCompletion OpenAIParser::ParseCompletion(const FJsonObject& json)
{
	FCompletion res = {};
	
	res.text = json.GetStringField(TEXT("text"));
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