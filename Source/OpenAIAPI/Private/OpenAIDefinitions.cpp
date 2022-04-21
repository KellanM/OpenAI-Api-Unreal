// Copyright 2021 Kellan Mythen. All Rights Reserved.

#include "OpenAIDefinitions.h"

OpenAIValueMapping::OpenAIValueMapping()
{

	engineTypes.Add(EOAEngineType::DAVINCI, TEXT("DAVINCI"));
	engineTypes.Add(EOAEngineType::CURIE, TEXT("CURIE"));
	engineTypes.Add(EOAEngineType::BABBAGE, TEXT("BABBAGE"));
	engineTypes.Add(EOAEngineType::ADA, TEXT("ADA"));
	engineTypes.Add(EOAEngineType::TEXT_DAVINCI_002, TEXT("TEXT-DAVINCI-002"));
	engineTypes.Add(EOAEngineType::TEXT_CURIE_001, TEXT("TEXT-CURIE-001"));
	engineTypes.Add(EOAEngineType::TEXT_BABBAGE_001, TEXT("TEXT-BABBAGE-001"));
	engineTypes.Add(EOAEngineType::TEXT_ADA_001, TEXT("TEXT-ADA-001"));
}	