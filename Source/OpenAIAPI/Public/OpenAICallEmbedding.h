// Copyright (c) 2024 tobenot, See LICENSE in the project root for license information.

// UOpenAICallEmbedding.h

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "OpenAIDefinitions.h"
#include "OpenAIEmbedding.h"
#include "OpenAICallEmbedding.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnEmbeddingResponseReceived, const FEmbeddingResult&, Result, const FString&, ErrorMessage, bool, Success);

/**
 * 
 */
UCLASS()
class OPENAIAPI_API UOpenAICallEmbedding : public UBlueprintAsyncActionBase
{
    GENERATED_BODY()

public:
    UOpenAICallEmbedding();
    ~UOpenAICallEmbedding();

    FEmbeddingSettings EmbeddingSettings; // This should be of type appropriate for embedding settings

    UPROPERTY(BlueprintAssignable, Category = "OpenAI")
    FOnEmbeddingResponseReceived Finished;

private:
    UPROPERTY()
    UOpenAIEmbedding* OpenAIEmbeddingInstance;

    UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "OpenAI")
    static UOpenAICallEmbedding* OpenAICallEmbedding(const FEmbeddingSettings& EmbeddingSettings); // This should use the correct type for embedding settings

    virtual void Activate() override;

    UFUNCTION()
    void OnResponse(const FEmbeddingResult& Result, const FString& ErrorMessage, bool Success);
};