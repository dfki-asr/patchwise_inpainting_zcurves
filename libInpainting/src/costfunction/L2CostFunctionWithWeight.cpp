#include "stdafx.h"

#include "L2CostFunctionWithWeight.h"
#include "dictionary/Dictionary.h"
#include "StatusFlags.h"

namespace ettention
{
	namespace inpainting
	{
		L2CostFunctionWithWeight::L2CostFunctionWithWeight(Problem* problem, Dictionary* dictionary)
			: DictionaryBasedCostFunctionKernel( problem, dictionary )
		{
		}

		L2CostFunctionWithWeight::~L2CostFunctionWithWeight()
		{
		}
		
		void L2CostFunctionWithWeight::computeCostForInterval(IndexInterval interval)
		{
			for (int dictionaryIndex = interval.first; dictionaryIndex <= interval.last; dictionaryIndex++)
			{
				const int patchIndex = dictionary->getCompressedDictionary()[ dictionaryIndex ];
				const float cost = computeCostFunction( patchIndex );
				resultCost.push_back(cost);
			}
		}

		float L2CostFunctionWithWeight::computeCostFunction( unsigned int indexOfSourcePatch )
		{
			dictionaryAccess.setPatchId(indexOfSourcePatch);
			dataAccess.setPatchId(indexOfTargetPatch);
			maskAccess.setPatchId(indexOfTargetPatch);

			float distance = 0.0f;
			for (unsigned int i = 0; i < dataAccess.size(); i++)
			{
				std::cout << "dataAccess.size(): " << dataAccess.size() << std::endl;

				/*
				dataAccess.size() == 363 == 3 * 11 * 11;
				3 + 3 + 3 + 3 + 3 ... ?
				or
				11*11 + 11*11 + 11*11 ?
				*/

				const unsigned char pixelStatus = maskAccess[i];
				if (pixelStatus == EMPTY_REGION || pixelStatus == TARGET_REGION)
					continue;
				unsigned char pA = dictionaryAccess[i];
				unsigned char pB = dataAccess[i];
				const float distanceInDimension = (float)(pB - pA);
				distance += distanceInDimension * distanceInDimension;
			}
			return std::sqrtf(distance);
		}
	}
}