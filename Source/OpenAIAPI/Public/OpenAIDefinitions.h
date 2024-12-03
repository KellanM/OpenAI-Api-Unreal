// Copyright Kellan Mythen 2023. All rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Misc/DateTime.h"
#include "OpenAIDefinitions.generated.h"

UENUM(BlueprintType)
enum class EOAOpenAIVoices : uint8
{
    ALLOY = 0 UMETA(DisplayName = "Alloy"),
    ECHO = 1 UMETA(DisplayName = "Echo"),
    SHIMMER = 2 UMETA(DisplayName = "Shimmer")
};

UENUM(BlueprintType)
enum class EOACompletionsEngineType : uint8
{

	DAVINCI = 0 UMETA(ToolTip = "Davinci is the most capable test engine and can perform any task the other models can perform and often with less instruction."),
	CURIE = 1 UMETA(ToolTip = "Curie is extremely powerful, yet very fast. While Davinci is stronger when it comes to analyzing complicated text, Curie is quite capable for many nuanced tasks like sentiment classification and summarization. Curie is also quite good at answering questions and performing Q&A and as a general service chatbot."),
	BABBAGE = 2 UMETA(ToolTip = "Babbage can perform straightforward tasks like simple classification. It�s also quite capable when it comes to Semantic Search ranking how well documents match up with search queries."),
	ADA = 3 UMETA(ToolTip = "Ada is usually the fastest model and can perform tasks like parsing text, address correction and certain kinds of classification tasks that don�t require too much nuance. Ada�s performance can often be improved by providing more context."),
	TEXT_DAVINCI_002 = 4 UMETA(ToolTip = "Most capable model in the GPT-3 series. Can perform any task the other GPT-3 models can, often with less context."),
	TEXT_CURIE_001 = 5 UMETA(ToolTip = "Very capable, but faster and lower cost than text-davinci-001."),
	TEXT_BABBAGE_001 = 6 UMETA(ToolTip = "Capable of straightforward tasks, very fast, and lower cost."),
	TEXT_ADA_001 = 7 UMETA(ToolTip = "Capable of simple tasks, using the fastest model in the GPT-3 series, and lowest cost."),
	TEXT_DAVINCI_003 = 8 UMETA(ToolTip = "Most capable model in the GPT-3 series. Can perform any task the other GPT-3 models can, often with less context."),
};

UENUM(BlueprintType)
enum class EOAChatEngineType : uint8
{
	GPT_3_5_TURBO = 0 UMETA(ToolTip = "More capable than any GPT-3.5 model, able to do more complex tasks, and optimized for chat. Will be updated with our latest model iteration."),
	GPT_4 = 1 UMETA(ToolTip = "More capable than any GPT-3.5 model, able to do more complex tasks, and optimized for chat. Will be updated with our latest model iteration."),
	GPT_4_32k = 2 UMETA(ToolTip = "Same capabilities as the base gpt-4 model but with 4x the context length. Will be updated with our latest model iteration."),
	GPT_4_TURBO = 3 UMETA(ToolTip = "The latest GPT-4 model with improved instruction following, JSON mode, reproducible outputs, parallel function calling, and more. Returns a maximum of 4,096 output tokens. This preview model is not yet suited for production traffic. Learn more."),
  GPT_4o = 4 UMETA(ToolTip = "GPT-4o Model"),
	GPT_4o_mini = 5 UMETA(ToolTip = "GPT-4o Mini Model"),
};

UENUM(BlueprintType)
enum class EOAChatRole : uint8
{
	SYSTEM = 0 UMETA(ToolTip = "More capable than any GPT-3.5 model, able to do more complex tasks, and optimized for chat. Will be updated with our latest model iteration."),
	USER= 1 UMETA(ToolTip = "More capable than any GPT-3.5 model, able to do more complex tasks, and optimized for chat. Will be updated with our latest model iteration."),
	ASSISTANT = 2 UMETA(ToolTip = "Same capabilities as the base gpt-4 model but with 4x the context length. Will be updated with our latest model iteration."),
};

UENUM(BlueprintType)
enum class EOAImageSize : uint8
{
	SMALL = 0 UMETA(ToolTip = "Generates 256x256 images. This settings takes the least amount of time to generate images."),
	MEDIUM = 1 UMETA(ToolTip = "Generates 512x512 images. This setting takes a moderate amount of time to generate images."),
	LARGE = 2 UMETA(ToolTip = "Generates 1024x1024 images. This setting takes the longest amount of time to generate images.")
};

/* make one for the speech model
* model
string
Required
One of the available TTS models: tts-1 or tts-1-hd

*/
UENUM(BlueprintType)
enum class EOASpeechEngineType : uint8
{
	TTS_1 = 0 UMETA(ToolTip = "The latest text to speech model, optimized for speed."),
	TTS_1_HD = 1 UMETA(ToolTip = "The latest text to speech model, optimized for quality."),
};


/*
 *make one for the voices
*voice
string
Required
The voice to use when generating the audio. Supported voices are alloy, echo, fable, onyx, nova, and shimmer.
 */
UENUM(BlueprintType)
enum class EOASpeechVoice : uint8
{
	ALLOY = 0 UMETA(ToolTip = "Generates 256x256 images. This settings takes the least amount of time to generate images."),
	ECHO = 1 UMETA(ToolTip = "Generates 512x512 images. This setting takes a moderate amount of time to generate images."),
	SHIMMER = 2 UMETA(ToolTip = "Generates 1024x1024 images. This setting takes the longest amount of time to generate images.")
};

// Structs for GPT

USTRUCT(BlueprintType)
struct FChatLog
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OpenAI")
	EOAChatRole role = EOAChatRole::SYSTEM;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OpenAI")
	FString content = "";
};

USTRUCT(BlueprintType)
struct FCompletionInfo
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OpenAI")
			FString id = "Null";

		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OpenAI")
			FString object = "Null";

		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OpenAI")
			FDateTime created;

		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OpenAI")
			FString model = "Null";

};

USTRUCT(BlueprintType)
struct FCompletion
{
	GENERATED_USTRUCT_BODY()

	// OpenAI's response.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OpenAI")
		FString text = "";

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OpenAI")
		int32 index = 0;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OpenAI")
		FString finishReason = "";
};

USTRUCT(BlueprintType)
struct FChatCompletion
{
	GENERATED_USTRUCT_BODY()

	// OpenAI's response.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OpenAI")
	FChatLog message;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OpenAI")
	FString finishReason = "";
};

USTRUCT(BlueprintType)
struct FSpeechCompletion
{
	GENERATED_USTRUCT_BODY()

	// OpenAI's response.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OpenAI")
	FString audioFilePath = "";

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OpenAI")
	FString finishReason = "";
};

class OpenAIValueMapping
{
public:
	OpenAIValueMapping();

	TMap<EOACompletionsEngineType, FString> engineTypes;
	TMap<EOAImageSize, FString> imageSizes;
};

USTRUCT(BlueprintType)
struct FCompletionSettings
{
	GENERATED_BODY()

	/** A sequence that is appended to the start of your prompt */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OpenAI")
		FString startSequence = "";

	/** A sequence that is appended at the beginning of your prompt */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OpenAI")
		FString injectStartText = "";

	/** A sequence that is appended to the start of your prompt */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OpenAI")
		FString injectRestartText = "";

	/** Up to four sequences where the API will stop generating further tokens. The returned text will not contain the stop sequence. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OpenAI")
		TArray<FString> stopSequences;

	/** The maximum number of tokens to generate. Requests can use up to 2048 tokens shared between prompt and completion. (One token is roughly 4 characters for normal English text) */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OpenAI")
		int32 maxTokens = 250;

	/** What sampling temperature to use. Higher values means the model will take more risks. Try 0.9 for more creative applications, and 0 (argmax sampling) for ones with a well-defined answer. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OpenAI")
		float temperature = 0.7f;

	/** An alternative to sampling with temperature, called nucleus sampling, where the model considers the results of the tokens with top_p probability mass. So 0.1 means only the tokens comprising the top 10% probability mass are considered.  */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OpenAI")
		float topP = 1.0f;

	/** If logProbs is greater than 0, the model returns tokens, token_logprobs, and the text_offsets for a given completion. (support for top_logprobs will be added soon)*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OpenAI")
		int32 logprobs = 0;

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

USTRUCT(BlueprintType)
struct FChatSettings
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OpenAI")
	EOAChatEngineType model = EOAChatEngineType::GPT_4;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OpenAI")
	TArray<FChatLog> messages;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OpenAI")
	float temperature = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OpenAI")
	int32 maxTokens = 250;
};
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
 */


USTRUCT(BlueprintType)
struct FSpeechSettings
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OpenAI")
	EOASpeechEngineType model = EOASpeechEngineType::TTS_1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OpenAI")
	FString input = "";

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OpenAI")
	EOASpeechVoice voice = EOASpeechVoice::ALLOY;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OpenAI")
	float speed = 1.0f;
};

/*
* Create translation
POST

https://api.openai.com/v1/audio/translations

Translates audio into English.

Request body
file
file
Required
The audio file object (not file name) translate, in one of these formats: flac, mp3, mp4, mpeg, mpga, m4a, ogg, wav, or webm.

model
string
Required
ID of the model to use. Only whisper-1 is currently available.

prompt
string
Optional
An optional text to guide the model's style or continue a previous audio segment. The prompt should be in English.

response_format
string
Optional
Defaults to json
The format of the transcript output, in one of these options: json, text, srt, verbose_json, or vtt.

temperature
number
Optional
Defaults to 0
The sampling temperature, between 0 and 1. Higher values like 0.8 will make the output more random, while lower values like 0.2 will make it more focused and deterministic. If set to 0, the model will use log probability to automatically increase the temperature until certain thresholds are hit.

Returns
The translated text.
 */

USTRUCT(BlueprintType)
struct FTranslationSettings
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OpenAI")
	FString model = "whisper-1";

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OpenAI")
	FString prompt = "";

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OpenAI")
	FString response_format = "json";

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OpenAI")
	float temperature = 0.0f;
};

UENUM(BlueprintType)
enum class EEmbeddingEngineType : uint8
{
	TEXT_EMBEDDING_3_SMALL = 0 UMETA(ToolTip = "Our newest and most performant embedding model, optimized for lower costs and higher multilingual performance"),
	TEXT_EMBEDDING_3_LARGE = 1 UMETA(ToolTip = "Our newest and most performant embedding model, optimized for lower costs and higher multilingual performance"),
	TEXT_EMBEDDING_ADA_002 = 2 UMETA(ToolTip = "Previous generation model"),
};

USTRUCT(BlueprintType)
struct FEmbeddingSettings
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OpenAI")
	EEmbeddingEngineType model = EEmbeddingEngineType::TEXT_EMBEDDING_3_SMALL;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OpenAI")
	FString input = "";
};

USTRUCT(BlueprintType)
struct FHighDimensionalVector
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OpenAI")
	TArray<float> Components;

	FHighDimensionalVector()
	{
		Components = TArray<float>();
	};
	FHighDimensionalVector(int32 Dimension)
	{
		Components.SetNumZeroed(Dimension);
	}
	FHighDimensionalVector(const TArray<float>& ComponentsArray)
	{
		Components = ComponentsArray;
	}
};

USTRUCT(BlueprintType)
struct FEmbeddingResult
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OpenAI")
	FHighDimensionalVector embeddingVector;

	FEmbeddingResult(const TArray<float>& VectorArray)
	{
		embeddingVector = FHighDimensionalVector(VectorArray);
	}
	FEmbeddingResult()
	{
		embeddingVector = FHighDimensionalVector();
	}
};