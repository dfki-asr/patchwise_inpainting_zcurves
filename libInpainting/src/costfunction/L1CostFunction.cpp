#include "stdafx.h"

#include "L1CostFunction.h"
#include "dictionary/Dictionary.h"
#include "StatusFlags.h"

namespace inpainting
{
	L1CostFunction::L1CostFunction(Problem* problem, Dictionary* dictionary)
		: DictionaryBasedCostFunctionKernel( problem, dictionary )
	{
		distance = new L1Distance< BytePatchAccess8Bit>();
	}

	L1CostFunction::~L1CostFunction()
	{
		delete distance;
	}
		
	void L1CostFunction::computeCostForInterval(IndexInterval interval)
	{
		for (int dictionaryIndex = interval.first; dictionaryIndex <= interval.last; dictionaryIndex++)
		{
			const int patchIndex = dictionary->getCompressedDictionary()[dictionaryIndex];
			const float cost = computeCostFunction(patchIndex);
			resultCost.push_back(cost);
		}
	}

	float L1CostFunction::computeCostFunction(unsigned int indexOfSourcePatch)
	{
		// const unsigned int dictionaryEntry = dictionary->getCompressedDictionary()[];
		dictionaryAccess.setPatchId(indexOfSourcePatch);

		// const unsigned int targetEntry = dictionary->getCompressedDictionary()[];
		dataAccess.setPatchId(indexOfTargetPatch);
		maskAccess.setPatchId(indexOfTargetPatch);

		float distance = 0.0f;
		for (unsigned int i = 0; i < dataAccess.size(); i++)
		{
			const unsigned char pixelStatus = maskAccess[i];
			if (pixelStatus == EMPTY_REGION || pixelStatus == TARGET_REGION)
				continue;
			unsigned char pA = dictionaryAccess[i];
			unsigned char pB = dataAccess[i];
			const float distanceInDimension = (float)(pB - pA);
			distance += fabs(distanceInDimension);
		}
		return distance;
	}
}
