// Copyright 2021 Kellan Mythen. All Rights Reserved.

#include "OpenAIDefinitions.h"

OpenAIValueMapping::OpenAIValueMapping()
{

	engineTypes.Add(EOAEngineType::DAVINCI, TEXT("DAVINCI"));
	engineTypes.Add(EOAEngineType::CURIE, TEXT("CURIE"));
	engineTypes.Add(EOAEngineType::BABBAGE, TEXT("BABBAGE"));
	engineTypes.Add(EOAEngineType::ADA, TEXT("ADA"));
	engineTypes.Add(EOAEngineType::DAVINCI_INSTRUCT_BETA, TEXT("DAVINCI-INSTRUCT-BETA"));
	engineTypes.Add(EOAEngineType::CURIE_INSTRUCT_BETA, TEXT("CURIE-INSTRUCT-BETA"));
}	