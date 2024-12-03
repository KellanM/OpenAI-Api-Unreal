// Copyright Kellan Mythen 2023. All rights Reserved.


#include "OpenAIUtils.h"
#include "OpenAIDefinitions.h"
#include "OpenAIAPI.h"
#include "Modules/ModuleManager.h"
#include "OpenAICallRealtime.h"

UOpenAICallRealtime* UOpenAIUtils::OpenAICallRealtime(FString Instructions, FString CreateResponseMessage, EOAOpenAIVoices Voice)
{
    return UOpenAICallRealtime::OpenAICallRealtime(Instructions, CreateResponseMessage, Voice);
}

FString UOpenAIUtils::GetVoiceString(EOAOpenAIVoices Voice)
{
    switch (Voice)
    {
    case EOAOpenAIVoices::ALLOY:
        return TEXT("alloy");
    case EOAOpenAIVoices::ECHO:
        return TEXT("echo");
    case EOAOpenAIVoices::SHIMMER:
        return TEXT("shimmer");
    default:
        return TEXT("alloy"); // Default voice
    }
}

void UOpenAIUtils::setOpenAIApiKey(FString apiKey)
{
	FOpenAIAPIModule& mod = FModuleManager::Get().LoadModuleChecked<FOpenAIAPIModule>("OpenAIAPI");
	mod._apiKey = apiKey;
}

FString UOpenAIUtils::getApiKey()
{
	FOpenAIAPIModule& mod = FModuleManager::Get().LoadModuleChecked<FOpenAIAPIModule>("OpenAIAPI");
	return mod._apiKey;
}

void UOpenAIUtils::	setUseOpenAIApiKeyFromEnvironmentVars(bool bUseEnvVariable)
{
	FOpenAIAPIModule& mod = FModuleManager::Get().LoadModuleChecked<FOpenAIAPIModule>("OpenAIAPI");
	mod._useApiKeyFromEnvVariable = bUseEnvVariable;
}

bool UOpenAIUtils::getUseApiKeyFromEnvironmentVars()
{

	FOpenAIAPIModule& mod = FModuleManager::Get().LoadModuleChecked<FOpenAIAPIModule>("OpenAIAPI");
	return mod._useApiKeyFromEnvVariable;
}

FString UOpenAIUtils::GetEnvironmentVariable(FString key)
{
	FString result;
#if PLATFORM_WINDOWS
	result = FWindowsPlatformMisc::GetEnvironmentVariable(*key);
#endif
#if PLATFORM_MAC
	result = FApplePlatformMisc::GetEnvironmentVariable(*key);
#endif

#if PLATFORM_LINUX
	result = FLinuxPlatformMisc::GetEnvironmentVariable(*key);
#endif
	return result;
}

float UOpenAIUtils::HDVectorDotProductSIMD(const FHighDimensionalVector& A, const FHighDimensionalVector& B)
{
	check(A.Components.Num() == B.Components.Num());
	// To utilize SIMD acceleration, the dimensionality of high-dimensional vectors needs to be a multiple of 4.
	check(A.Components.Num() % 4 == 0);

	__m128 Sum = _mm_setzero_ps();
	for (int32 i = 0; i < A.Components.Num(); i += 4)
	{
		__m128 AVec = _mm_loadu_ps(&A.Components[i]);
		__m128 BVec = _mm_loadu_ps(&B.Components[i]);
		__m128 Mul = _mm_mul_ps(AVec, BVec);
		Sum = _mm_add_ps(Sum, Mul);
	}

	float Result[4];
	_mm_storeu_ps(Result, Sum);
	return Result[0] + Result[1] + Result[2] + Result[3];
}

float UOpenAIUtils::HDVectorLengthSIMD(const FHighDimensionalVector& Vector)
{
	return FMath::Sqrt(HDVectorDotProductSIMD(Vector, Vector));
}

float UOpenAIUtils::HDVectorCosineSimilaritySIMD(const FHighDimensionalVector& A, const FHighDimensionalVector& B)
{
	float DotProductValue = HDVectorDotProductSIMD(A, B);
	float LengthProduct = HDVectorLengthSIMD(A) * HDVectorLengthSIMD(B);
	return DotProductValue / LengthProduct;
}

float UOpenAIUtils::HDVectorDotProduct(const FHighDimensionalVector& A, const FHighDimensionalVector& B)
{
	check(A.Components.Num() == B.Components.Num());

	float Sum = 0.0f;
	for (int32 i = 0; i < A.Components.Num(); i++)
	{
		Sum += A.Components[i] * B.Components[i];
	}
	return Sum;
}

float UOpenAIUtils::HDVectorLength(const FHighDimensionalVector& Vector)
{
	return FMath::Sqrt(HDVectorDotProduct(Vector, Vector));
}

float UOpenAIUtils::HDVectorCosineSimilarity(const FHighDimensionalVector& A, const FHighDimensionalVector& B)
{
	float DotProductValue = HDVectorDotProduct(A, B);
	float LengthProduct = HDVectorLength(A) * HDVectorLength(B);
	return DotProductValue / LengthProduct;
}
