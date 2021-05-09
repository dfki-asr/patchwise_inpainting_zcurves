#include "stdafx.h"

#include "L2CostFunction.h"
#include "dictionary/Dictionary.h"
#include "StatusFlags.h"

namespace inpainting
{
	L2CostFunction::L2CostFunction(Problem* problem, Dictionary* dictionary)
		: DictionaryBasedCostFunctionKernel( problem, dictionary )
	{
	}

	L2CostFunction::~L2CostFunction()
	{
	}
		
	void L2CostFunction::computeCostForInterval(IndexInterval interval)
	{
		for (int dictionaryIndex = interval.first; dictionaryIndex <= interval.last; dictionaryIndex++)
		{
			const int patchIndex = dictionary->getCompressedDictionary()[ dictionaryIndex ];
			const float cost = computeCostFunction( patchIndex );
			resultCost.push_back(cost);
		}
	}

	float L2CostFunction::computeCostFunction( unsigned int indexOfSourcePatch )
	{
		dictionaryAccess.setPatchId(indexOfSourcePatch);
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
			distance += distanceInDimension * distanceInDimension;
		}
		return sqrtf(distance);
	}
}