// Copyright (c) 2024 tobenot, See LICENSE in the project root for license information.


// UOpenAICallEmbedding.cpp

#include "OpenAICallEmbedding.h"
#include "OpenAIUtils.h"

UOpenAICallEmbedding::UOpenAICallEmbedding()
	: OpenAIEmbeddingInstance(nullptr)
{
}

UOpenAICallEmbedding::~UOpenAICallEmbedding()
{
	if (OpenAIEmbeddingInstance)
	{
		OpenAIEmbeddingInstance->ConditionalBeginDestroy();
	}
}

UOpenAICallEmbedding* UOpenAICallEmbedding::OpenAICallEmbedding(const FEmbeddingSettings& EmbeddingSettingsInput)
{
	UOpenAICallEmbedding* BPNode = NewObject<UOpenAICallEmbedding>();
	BPNode->EmbeddingSettings = EmbeddingSettingsInput;
	return BPNode;
}

void UOpenAICallEmbedding::Activate()
{
	if (!OpenAIEmbeddingInstance)
	{
		OpenAIEmbeddingInstance = UOpenAIEmbedding::CreateEmbeddingInstance();
	}

	OpenAIEmbeddingInstance->Init(EmbeddingSettings);

	OpenAIEmbeddingInstance->OnResponseReceived.BindDynamic(this, &UOpenAICallEmbedding::OnResponse);

	OpenAIEmbeddingInstance->StartEmbedding();
}

void UOpenAICallEmbedding::OnResponse(const FEmbeddingResult& Result, const FString& ErrorMessage, bool Success)
{
	OpenAIEmbeddingInstance->OnResponseReceived.Unbind();
	Finished.Broadcast(Result, ErrorMessage, Success);
	OpenAIEmbeddingInstance->ConditionalBeginDestroy();
	OpenAIEmbeddingInstance = nullptr;
}