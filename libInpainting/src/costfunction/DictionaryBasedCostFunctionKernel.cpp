#include "stdafx.h"

#include "DictionaryBasedCostFunctionKernel.h"

#include "Problem.h"
#include "NDSpaceMapping.h"
#include "dictionary/Dictionary.h"
#include "index/DimensionSelection.h"

namespace ettention
{
	namespace inpainting
	{

		DictionaryBasedCostFunctionKernel::DictionaryBasedCostFunctionKernel( Problem* problem, Dictionary* dictionary )
			: problem(problem)
			, dictionary(dictionary)
			, dictionaryAccess(problem->dictionaryVolume, problem->patchSize, DimensionSelection::standardPermutation(problem->patchSize))
			, maskAccess(problem->mask, problem->patchSize, DimensionSelection::standardPermutation(problem->patchSize))
			, dataAccess(problem->data, problem->patchSize, DimensionSelection::standardPermutation(problem->patchSize))
		{
			if (dictionary->getCompressedDictionary().size() > std::numeric_limits<unsigned int>::max())
				throw std::runtime_error("maximum dictionary size exceeded");

			resultCost.reserve( dictionary->getCompressedDictionary().size() );
		}

		DictionaryBasedCostFunctionKernel::~DictionaryBasedCostFunctionKernel()
		{
		}
		

		void DictionaryBasedCostFunctionKernel::computeCostForAllIntervals( std::vector< IndexInterval > all_intervals )
		{
			resultCost.resize(0);
			for ( auto& interval : all_intervals )
				computeCostForInterval( interval );
		}

		/* 
		Derived classes of DictionaryBasedCostFunctionKernel need to implement computeCostForInterval following this example.
		computeCostFunction is not virtual itself for performance reasons, so implementing the function in 
		DictionaryBasedCostFunctionKernel is not possible

		void DictionaryBasedCostFunctionKernel::computeCostForInterval(IndexInterval interval)
		{
			for (int dictionaryIndex = interval.first; dictionaryIndex <= interval.last; dictionaryIndex++)
			{
				const float cost = computeCostFunction(dictionaryIndex);
				resultCost.push_back(cost);
			}
		}
		*/

		float DictionaryBasedCostFunctionKernel::computeCostFunction(unsigned int patchCoordinate)
		{
			return 0.0f;
		}

		std::vector<float>& DictionaryBasedCostFunctionKernel::getResult()
		{
			return resultCost;
		}

		void DictionaryBasedCostFunctionKernel::setCenterOfTargetPatch(Vec3i centerOfTargetPatch)
		{
			this->centerOfTargetPatch = centerOfTargetPatch;
			indexOfTargetPatch = Flatten3D(centerOfTargetPatch, problem->data->getProperties().getVolumeResolution() );
		}
	}
}