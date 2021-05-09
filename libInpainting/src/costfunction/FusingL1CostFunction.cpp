#include "stdafx.h"

#include "FusingL1CostFunction.h"

#include "dictionary/Dictionary.h"
#include "StatusFlags.h"
#include "Problem.h"
#include "index/DimensionSelection.h"

namespace inpainting
{
	FusingL1CostFunction::FusingL1CostFunction(Problem* problem, Dictionary* dictionary, float denseWeight)
		: DictionaryBasedCostFunctionKernel( problem, dictionary )
		, denseDataAccess(problem->denseScan, problem->patchSize, DimensionSelection::standardPermutation(problem->patchSize))
		, denseWeight(denseWeight)
	{
	}

	FusingL1CostFunction::~FusingL1CostFunction()
	{
	}
		
	void FusingL1CostFunction::computeCostForInterval(IndexInterval interval)
	{
		for (int dictionaryIndex = interval.first; dictionaryIndex <= interval.last; dictionaryIndex++)
		{
			const int patchIndex = dictionary->getCompressedDictionary()[dictionaryIndex];
			const float cost = computeCostFunction(patchIndex);
			resultCost.push_back(cost);
		}
	}

	float FusingL1CostFunction::computeCostFunction(unsigned int indexOfSourcePatch)
	{
		dictionaryAccess.setPatchId(indexOfSourcePatch);
		dataAccess.setPatchId(indexOfTargetPatch);
		maskAccess.setPatchId(indexOfTargetPatch);
		denseDataAccess.setPatchId(indexOfTargetPatch);

		float distance = 0.0f;
		float weight = 0.0f;
		float weightOfDimension;
		for (unsigned int i = 0; i < dataAccess.size(); i++)
		{
			const unsigned char pixelStatus = maskAccess[i];
			unsigned char pB;
			if (pixelStatus == EMPTY_REGION || pixelStatus == TARGET_REGION)
			{
				pB = denseDataAccess[i];
				weightOfDimension = denseWeight;
			}
			else
			{
				pB = dataAccess[i];
				weightOfDimension = 1.0f;
			}
			unsigned char pA = dictionaryAccess[i];
			const float distanceInDimension = fabs( (float)(pB - pA) );
			distance += distanceInDimension * weightOfDimension;
			weight += weightOfDimension;
		}
		return distance / weight;
	}
}
