// Copyright Kellan Mythen 2021. All rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OpenAIDefinitions.generated.h"


UENUM(BlueprintType)
enum class EOAEngineType : uint8
{
	DAVINCI = 0 UMETA(ToolTip = "Davinci is the most capable engine and can perform any task the other models can perform and often with less instruction."),
	CURIE = 1 UMETA(ToolTip = "Curie is extremely powerful, yet very fast. While Davinci is stronger when it comes to analyzing complicated text, Curie is quite capable for many nuanced tasks like sentiment classification and summarization. Curie is also quite good at answering questions and performing Q&A and as a general service chatbot."),
	BABBAGE = 2 UMETA(ToolTip = "Babbage can perform straightforward tasks like simple classification. It’s also quite capable when it comes to Semantic Search ranking how well documents match up with search queries."),
	ADA = 3 UMETA(ToolTip = "Ada is usually the fastest model and can perform tasks like parsing text, address correction and certain kinds of classification tasks that don’t require too much nuance. Ada’s performance can often be improved by providing more context.")
};

USTRUCT(BlueprintType)
struct FCompletion
{
	GENERATED_BODY()


	// OpenAI's response.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OpenAI")
		FString text = "";

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OpenAI")
		int32 index = 0;

	/** Currently Unsupported */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OpenAI")
		int32 logProbs = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OpenAI")
		FString finishReason = "";

};

class OpenAIValueMapping
{
public:
	OpenAIValueMapping();

	TMap<EOAEngineType, FString> engineTypes;
};

USTRUCT(BlueprintType)
struct FGPT3Settings
{
	GENERATED_BODY()

	/** A sequence that is used at the start of the prompt */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OpenAI")
		FString startSequence = "";

	/** A sequence where the API will stop generating further tokens. The returned text will not contain the stop sequence. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OpenAI")
		FString stopSequence = "";

	/** The maximum number of tokens to generate. Requests can use up to 2048 tokens shared between prompt and completion. (One token is roughly 4 characters for normal English text) */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OpenAI")
		int32 maxTokens = 16;

	/** What sampling temperature to use. Higher values means the model will take more risks. Try 0.9 for more creative applications, and 0 (argmax sampling) for ones with a well-defined answer. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OpenAI")
		float temperature = 1.0f;

	/** An alternative to sampling with temperature, called nucleus sampling, where the model considers the results of the tokens with top_p probability mass. So 0.1 means only the tokens comprising the top 10% probability mass are considered.  */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OpenAI")
		float topP = 1.0f;

	/** How many completions to generate for each prompt. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OpenAI")
		int32 numCompletions = 1;

	/** Number between 0 and 1 that penalizes new tokens based on their existing frequency in the text so far. Decreases the model's likelihood to repeat the same line verbatim. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OpenAI")
		float frequencyPenalty = 0.0f;

	/** Number between 0 and 1 that penalizes new tokens based on whether they appear in the text so far. Increases the model's likelihood to talk about new topics. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OpenAI")
		float presencePenalty = 0.0f;

	/** Generates best_of completions server-side and returns the "best" (the one with the lowest log probability per token). */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OpenAI")
		int32 bestOf = 1;

};