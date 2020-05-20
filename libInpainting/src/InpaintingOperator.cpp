#include "stdafx.h"

#include "InpaintingOperator.h"

#include "math/Vec3.h"
#include "index/Index.h"

#include "StatusFlags.h"
#include "computeorder/ComputeOrder.h"
#include "computeorder/ComputeFront.h"
#include "computeorder/CriminisiOrder.h"

#include "patchselection/PatchSelection.h"
#include "ProgressReporter.h"
#include "Problem.h"

#include "io/serializer/VolumeSerializer.h"
#include "setup/parameterset/OutputParameterSet.h"

#include "setup/InpaintingDebugParameters.h"
#include "setup/InpaintingOptimizationParameters.h"

namespace ettention
{
    namespace inpainting 
    {

        InpaintingOperator::InpaintingOperator( Problem* problem, ComputeOrder* priority, PatchSelection* patchSelection, ProgressReporter* progress, bool shouldUseBlendOperation, InpaintingDebugParameters* debugParameters, InpaintingOptimizationParameters* optimizationParameters )
            : problem(problem), priority(priority), patchSelection(patchSelection), progress(progress), shouldUseBlendOperation(shouldUseBlendOperation), debugParameters(debugParameters), optimizationParameters(optimizationParameters), error(nullptr)
        {			
            if ( debugParameters->shouldOutputErrorVolumes() )
            {
                error = new FloatVolume( problem->data->getProperties().getVolumeResolution(), 0.0f );
            }
        }

        InpaintingOperator::~InpaintingOperator()
        {
            if ( debugParameters->shouldOutputErrorVolumes() )
            {
                delete error;
            }
        }

        void InpaintingOperator::run()
        {
			bool shouldDoFurtherIteration = false;
            iterationNumber = 0;

			progress->reportTaskStart("inpainting", (float)priority->getSizeOfTargetArea());
			// do while loop for iterative patch match
			do 
			{
				while (!priority->isEmpty())
				{
					outputDebugVolumesIfRequired();
					runOneIteration();
				}
				shouldDoFurtherIteration = patchSelection->initializeNewIteration();
				if (shouldDoFurtherIteration)
				{
					BoundingBox3i imageBorder(Vec3i(0, 0, 0), problem->data->getProperties().getVolumeResolution() - Vec3i(1, 1, 1));
					priority->growFront(imageBorder);
				}
			} while (shouldDoFurtherIteration);
			progress->reportTaskEnd();
        }

        void InpaintingOperator::outputDebugVolumesIfRequired()
        {
            if ( iterationNumber % debugParameters->shouldOutputDebugVolumesEveryNIterations() != 0)
                return; 

            outputDebugVolumes( );
        }

        void InpaintingOperator::outputDebugVolumes()
        {
            OutputParameterSet parameter;
            VolumeSerializer serializer;

			if (debugParameters->shouldOutputData())
			{
				std::string fileNameData = debugParameters->getDebugInfoPath() + "debugOutput_data_iteration_" + std::to_string((int)iterationNumber) + ".mrc";
				serializer.write(problem->data, fileNameData, parameter.getVoxelType(), parameter.getOrientation());
			}

			if (debugParameters->shouldOutputMask())
			{
				std::string fileNameMask = debugParameters->getDebugInfoPath() + "debugOutput_mask_iteration_" + std::to_string((int)iterationNumber) + ".mrc";
				serializer.write(problem->mask, fileNameMask, parameter.getVoxelType(), parameter.getOrientation());
			}

			if (debugParameters->shouldOutputMask())
			{
				std::string fileNameMask = debugParameters->getDebugInfoPath() + "debugOutput_mask_iteration_" + std::to_string((int)iterationNumber) + ".mrc";
				serializer.write(problem->mask, fileNameMask, parameter.getVoxelType(), parameter.getOrientation());
			}

            priority->outputDebugVolumes( debugParameters->getDebugInfoPath(), iterationNumber, debugParameters );
        }

        FloatVolume* InpaintingOperator::getErrorVolume()
        {
            return error;
        }

        void InpaintingOperator::runOneIteration()
        {
			try {
				centerOfPatchToInpaint = priority->selectCenterOfPatchToProcess(true);
				selectedCoordinatesByCriminisi.push_back(centerOfPatchToInpaint);
				auto originalPosition = centerOfPatchToInpaint;
				centerOfPatchToInpaint = patchSelection->adjustTargetPatchPosition(centerOfPatchToInpaint);
				centerOfSourcePatch = patchSelection->selectCenterOfBestPatch(centerOfPatchToInpaint);
				patchSelection->iterationNumber = iterationNumber;
				inpaintPatch();
				iterationNumber++;
				priority->inpaintingIterationNumber = iterationNumber;
			}
			catch ( IndexCannotHandlePositionError cannotHandlePostion)
			{
				//outputDebugVolumes();
				priority->rememberPatchForLater( cannotHandlePostion.targetPatchCenter );
				if (priority->shouldUseBruteForce())
					patchSelection->activateBruteForceFallback();
			} 
			catch ( std::exception e )
			{
				outputDebugVolumes();
				throw e;
			}
        }

        void InpaintingOperator::inpaintPatch()
        {
            updateDataAndMask( centerOfPatchToInpaint );
            updatePriority( centerOfPatchToInpaint );
        }

        void InpaintingOperator::updateDataAndMask(Vec3ui centerOfPatchToInpaint)
        {
            const Vec3ui patchSize = problem->patchSize;
            this->centerOfPatchToInpaint = centerOfPatchToInpaint;
    
            targetPatchUpperLeft = centerOfPatchToInpaint - (patchSize / 2);

            Vec3i coord;
            voxelsUpdated = 0;
            auto boxToInpaint = BoundingBox3i(Vec3i(targetPatchUpperLeft), Vec3i(targetPatchUpperLeft) + patchSize - Vec3i(1, 1, 1));

            for (coord.x = boxToInpaint.getMin().x; coord.x <= boxToInpaint.getMax().x; coord.x++)
            {
                for (coord.y = boxToInpaint.getMin().y; coord.y <= boxToInpaint.getMax().y; coord.y++)
                {
                    for (coord.z = boxToInpaint.getMin().z; coord.z <= boxToInpaint.getMax().z; coord.z++)
                    {
                        updateDataAndMaskOfOneVoxel(coord);
                    }
                }
            }
            
            progress->reportIncrementalProgress( (float) voxelsUpdated );
        }

        void InpaintingOperator::updateDataAndMaskOfOneVoxel(Vec3ui coord)
        {
            size_t targetIndex = problem->mask->calculateVoxelIndex(coord);
            byte_t status = problem->mask->nativeVoxelValue(targetIndex);
           
            Vec3i sourcePatchUpperLeft = centerOfSourcePatch - (problem->patchSize / 2);

            if (status == TARGET_REGION)
            {
                problem->mask->setVoxelToByteValue(targetIndex, INPAINTED_REGION);
                const float inpaintValue = calculateTargetValue(coord);
                problem->data->setVoxelToValue(targetIndex, inpaintValue);
                voxelsUpdated++;

                if ( debugParameters->shouldOutputErrorVolumes() )
                {
                    const float errorValue = patchSelection->lastCostFunctionValue;
                    error->setVoxelToValue(targetIndex, errorValue);
                }

            }
            else if (shouldUseBlendOperation && status != SOURCE_REGION && status != EMPTY_REGION)
            {
                unsigned char numberOfTimesTheVoxelWasWritten = status + 1;
                problem->mask->setVoxelToByteValue(targetIndex, numberOfTimesTheVoxelWasWritten);

                const float alpha = 1.0f / (float)numberOfTimesTheVoxelWasWritten;
                const float oldValue = problem->data->nativeVoxelValue(targetIndex);
                const float inpaintValue = calculateTargetValue(coord);
                const float finalValue = alpha * inpaintValue + (1.0f - alpha) * oldValue;

                problem->data->setVoxelToValue(targetIndex, finalValue);
            }
        }

        void InpaintingOperator::updatePriority(Vec3ui centerOfPatchToInpaint)
        {			
            const Vec3ui patchSize = problem->patchSize;
            Vec3ui upperLeftCorner = centerOfPatchToInpaint - patchSize / 2;
            Vec3ui lowerRightCorner = centerOfPatchToInpaint + patchSize / 2;
            priority->progressFront( BoundingBox3i(upperLeftCorner, lowerRightCorner) );

            Vec3ui upperLeftOfImpactWindow = centerOfPatchToInpaint - patchSize;
            Vec3ui lowerRightOfImpactWindow = centerOfPatchToInpaint + patchSize;
            priority->updatePriorityInRegion(upperLeftCorner, lowerRightCorner);
        }

        float InpaintingOperator::calculateTargetValue(Vec3ui coord)
        {
            Vec3i sourcePatchUpperLeft = centerOfSourcePatch - (problem->patchSize / 2);
            const Vec3i sourceCoord = coord - targetPatchUpperLeft + sourcePatchUpperLeft;
            size_t sourceIndex = problem->dictionaryVolume->calculateVoxelIndex(sourceCoord);
            return problem->dictionaryVolume->nativeVoxelValue(sourceIndex);
        }

        ComputeOrder* InpaintingOperator::getPriority()
        {
            return priority;
        }

        PatchSelection* InpaintingOperator::getPatchSelection()
        {
            return patchSelection;
        }

        ettention::Vec3ui InpaintingOperator::getCenterOfLastInpaintedPatch()
        {
            return centerOfPatchToInpaint;
        }

        ettention::Vec3ui InpaintingOperator::getCenterOfLastSourcePatch()
        {
            return centerOfSourcePatch;
        }

        void InpaintingOperator::setCenterOfSourcePatch(Vec3i value)
        {
            centerOfSourcePatch = value;
        }

    }

}
