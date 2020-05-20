#include "stdafx.h"

#include "CurveBasedPatchSelection.h"

#include "StatusFlags.h"
#include "dictionary/Dictionary.h"

#include "setup/IndexOptions.h"

#include "PatchSelectionStatistics.h"
#include "costfunction/DictionaryBasedCostFunctionKernel.h"
#include "index/DimensionSelection.h"
#include "../zcurve/ZCurveKNNQuery.h"
#include "../zcurve/ZCurveIndex.h"
#include "index/MultiIndex.h"
#include "NDSpaceMapping.h"

namespace ettention
{
    namespace inpainting
    {

        CurveBasedPatchSelection::CurveBasedPatchSelection(DictionaryBasedCostFunctionKernel* costFunction, ByteVolume* dataVolume, ByteVolume* maskVolume, ByteVolume* dictionaryVolume, std::vector<unsigned int>& dictionaryPatches, Vec3ui patchSize, InpaintingDebugParameters* debugParameters, InpaintingOptimizationParameters* optimizationParameters, IndexOptions* indexOptions, CostFunctionOptions* costFunctionOptions, bool shouldUseBruteForce, ProgressReporter* progress )
            : PatchSelection(dataVolume, maskVolume, dictionaryVolume)
            , costFunction(costFunction)
            , dictionaryPatches(dictionaryPatches)
            , patchSize(patchSize)
            , debugParameters(debugParameters)
            , optimizationParameters(optimizationParameters)
			, index( nullptr )
			, zeroDistanceRange(0)
        {
			log = new Log( debugParameters );
            volumeStatistics = VolumeStatistics::compute(dictionaryVolume);
	
			ndSpaceMapping = new NDSpaceMapping( dataVolume, maskVolume, patchSize, DimensionSelection::standardPermutation(patchSize) );

			if ( indexOptions->getIndexType() != IndexOptions::IndexType::NONE)
			{
				std::filesystem::path pathToFile( indexOptions->getCacheFileName() );
				if (std::filesystem::exists(pathToFile) && std::filesystem::is_regular_file(pathToFile))
				{
					std::ifstream ifs( pathToFile.string(), std::ios_base::binary );
					index = new MultiIndex( ifs, dataVolume, maskVolume, dictionaryVolume, indexOptions, progress, log, costFunctionOptions );
					ifs.close();
				}
				else 
				{
					index = new MultiIndex(dataVolume, maskVolume, dictionaryVolume, dictionaryPatches, patchSize, indexOptions, progress, log, costFunctionOptions );
					if (indexOptions->getCacheFileName() != "")
					{
						std::ofstream ofs(pathToFile.string(), std::ios_base::binary);
						index->writeToStream(ofs);
						ofs.close();
					}
				}
			}

            totalPatchesSelected = 0;
            totalPatchesBruteForce = 0;
            offset = 0.0f;
			relaxedOffset = 0.0f;
        }

        CurveBasedPatchSelection::~CurveBasedPatchSelection()
        {
			delete ndSpaceMapping;
			if ( index != nullptr )
				delete index;
			delete log;
        }

		Vec3i CurveBasedPatchSelection::adjustTargetPatchPosition(Vec3i targetPatchCenter)
		{
			if ( index == nullptr )
				return ensureTargetPatchPositionInsideSafeRegion( targetPatchCenter );

			return index->adjustTargetPatchPosition( targetPatchCenter );
		}

        Vec3i CurveBasedPatchSelection::selectCenterOfBestPatch( Vec3i centerOfTargetPatch )
        {
			log->setTargetPosition(centerOfTargetPatch);
			setCenterOfTargetPatch( centerOfTargetPatch );

			dictionaryIntervals.clear();

			if (index == nullptr)
				performBruteForceSearch( );
            else
				performNearestNeighborSearch( );
  
			log->setPatchNorm(lastCostFunctionValue);

			if (debugParameters->shouldDoubleCheckUsingBruteForce())
				doubleCheckUsingBruteForce();

			log->setSourcePosition( bestCoordinate );
			log->reportPatchSelection();

            return bestCoordinate;
        }

		void CurveBasedPatchSelection::activateBruteForceFallback()
		{
			index = nullptr;
		}

		void CurveBasedPatchSelection::performNearestNeighborSearch()
		{
			auto result = index->query(centerOfTargetPatch);
			bestCoordinate = std::get<0>(result);
			lastCostFunctionValue = std::get<1>(result);
		}

		void CurveBasedPatchSelection::performBruteForceSearch()
		{
			dictionaryIntervals.push_back(getSearchIntervalForBruteForceSearch());
			bestCoordinate = findMinimumInResult();
		}

		ettention::Vec3i CurveBasedPatchSelection::ensureTargetPatchPositionInsideSafeRegion(Vec3i targetPatchCenter)
		{
			auto boundingBox = dataVolume->getProperties().getMemoryBoundingBox();
			boundingBox.setMin(boundingBox.getMin() + patchSize / 2);
			boundingBox.setMax(boundingBox.getMax() - patchSize / 2);
			return boundingBox.getNextPointInside( targetPatchCenter );
		}

		IndexInterval CurveBasedPatchSelection::getSearchIntervalForBruteForceSearch()
        {
            return IndexInterval( 0, (unsigned int) dictionaryPatches.size()-1 );
        }

        Vec3ui CurveBasedPatchSelection::findMinimumInResult()
        {
            auto& costVector = costFunction->getResult();

            float lowestCost = std::numeric_limits<float>::max();
            bestIndexInResultVector = -1;

            for (size_t i = 0; i < costVector.size(); i++)
            {
				const float cost = costVector[i];
                if (cost >= lowestCost)
                    continue;

                if ( std::isnan(cost) )
                    continue;

                lowestCost = cost;
                bestIndexInResultVector = (unsigned int) i;
            }

            if (bestIndexInResultVector == -1)
            {
                throw std::runtime_error("illegal source patch");
            }
			
            const Vec3ui bestCoordinate = Unflatten3D( dictionaryPatches[bestIndexInResultVector], dictionaryVolume->getProperties().getVolumeResolution() );
            lastCostFunctionValue = lowestCost;
            return bestCoordinate;
        }

		void CurveBasedPatchSelection::doubleCheckUsingBruteForce()
		{
			Vec3i zcruveBestCoordinate = bestCoordinate;
			float zcurveCost = lastCostFunctionValue;
			costFunction->computeCostForInterval( getSearchIntervalForBruteForceSearch() );
			Vec3i bruteForceBestCoordinate = findMinimumInResult();
			log->setPatchNormBruteForce(lastCostFunctionValue);
		}

		void CurveBasedPatchSelection::outputZeroCostPosition()
        {
            std::vector<IndexInterval> dictionaryIntervals;
            dictionaryIntervals.push_back(IndexInterval(0, (unsigned int)dictionaryPatches.size()-1));
            costFunction->computeCostForAllIntervals(dictionaryIntervals);
            bestCoordinate = findMinimumInResult();
            std::cout << "correct position " << bestIndexInDictionary << " (" << dictionaryPatches[bestIndexInDictionary] <<  " / " << bestCoordinate << ")  at cost " << lastCostFunctionValue << std::endl << std::endl;
        }

		void CurveBasedPatchSelection::setCenterOfTargetPatch( Vec3i centerOfTargetPatch )
		{
			this->centerOfTargetPatch = centerOfTargetPatch;
			zeroDistanceRange = ndSpaceMapping->getZeroDistanceRangeForIncompletePatch(centerOfTargetPatch);
		}
    } // namespace
} // namespace
