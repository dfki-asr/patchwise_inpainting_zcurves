#include "stdafx.h"

#include "L2CostFunctionWithWeight.h"
#include "dictionary/Dictionary.h"
#include "StatusFlags.h"
#include "../../libInpainting/src/Problem.h"

namespace ettention
{
	namespace inpainting
	{
		L2CostFunctionWithWeight::L2CostFunctionWithWeight(Problem* problem, Dictionary* dictionary)
			: DictionaryBasedCostFunctionKernel( problem, dictionary )
		{
			if (problem->patchSize.z != problem->costWeight.size()) {
				std::ostringstream stringStream;
				stringStream << "size of costWeight and patchSize.z don't match: ";
				stringStream << "size of costWeight is " << problem->costWeight.size() << " while patchSize.z is " << problem->patchSize.z << std::endl;
				std::string errorStr = stringStream.str();

				throw std::runtime_error(errorStr);
			}
			costWeight = problem->costWeight;
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
				const unsigned char pixelStatus = maskAccess[i];
				if (pixelStatus == EMPTY_REGION || pixelStatus == TARGET_REGION)
					continue;
			
				Vec3ui vIndex = dataAccess.getPositionInVolume(i);
				
				unsigned char pA = dictionaryAccess[i];
				unsigned char pB = dataAccess[i];
				const float distanceInDimension = (float)(pB - pA);
				distance += distanceInDimension * distanceInDimension * costWeight[vIndex.z];
			}

			return std::sqrtf(distance);
		}
	}
}