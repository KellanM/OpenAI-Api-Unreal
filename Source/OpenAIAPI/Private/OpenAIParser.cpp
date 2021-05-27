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
	/* still have a bit of work needed in order to get log_props working properly :P
	
	
	TArray<float> logProbsTemp= json.GetArrayField(TEXT("token_logprobs"));
	//res.logProbs = json.GetArrayField(TEXT("top_logprobs"));
	
	*/

	return res;
}

