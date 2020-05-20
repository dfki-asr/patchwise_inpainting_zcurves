#include "stdafx.h"

#include "ConfidenceOrder.h"

#include "model/volume/FloatVolume.h"

#include "Confidence.h"
#include "CriminisiDataTerm.h"
#include "ComputeFront.h"
#include "Gradient3D.h"
#include "ComputeFrontInitializer.h"
#include "StatusFlags.h"

#include "model/volume/ByteVolume.h"

namespace ettention
{
    namespace inpainting
    {

        ConfidenceOrder::ConfidenceOrder( Volume* dataVolume, ByteVolume* maskVolume, Vec3ui patchSize, ProgressReporter* progress)
            : ComputeOrder( maskVolume )
            , progress(progress)
            , dataVolume(dataVolume)
            , maskVolume(maskVolume)
        {
            volumeResolution = maskVolume->getProperties().getVolumeResolution();

            auto mask = dynamic_cast<ByteVolume*>(maskVolume);
            confidence = new Confidence(mask, patchSize);
            dataTerm = new CriminisiDataTerm( this->dataVolume, this->maskVolume, patchSize );

            ComputeFrontInitializer initializer( this, patchSize, progress);
            front = initializer.generateComputeFront();
            sizeOfTargetArea = initializer.getSizeOfTargetArea();
        }

        ConfidenceOrder::~ConfidenceOrder()
        {
            delete confidence;
            delete dataTerm;
            delete front;
        }

        ettention::Vec3ui ConfidenceOrder::selectCenterOfPatchToProcess( bool shouldPopPatch )
        {
            if (shouldPopPatch)
                return front->popEntryWithHighestPriority().coordinate;
            return front->peekEntryWithHighestPriority().coordinate;
        }

        float ConfidenceOrder::computePriorityForVoxel(Vec3ui coordinate)
        {
            return confidence->computeConfidenceOfOneVoxel(coordinate, inpaintingIterationNumber);
        }

        void ConfidenceOrder::outputDebugVolumes(std::string pathToDebugFolder, unsigned int iterationNumber, InpaintingDebugParameters* parameters )
        {

        }

		Volume* ConfidenceOrder::plotConfidenceToVolume()
        {
            Volume* volume = new FloatVolume(volumeResolution, 0.0f);

            Vec3ui coord(0,0,0);
            for (coord.z = 0; coord.z < volumeResolution.z; coord.z++)
            {
                for (coord.y = 0; coord.y < volumeResolution.y; coord.y++)
                {
                    for (coord.x = 0; coord.x < volumeResolution.x; coord.x++)
                    {
                        const size_t index = maskVolume->getVoxelIndex(coord);
                        if (maskVolume->nativeVoxelValue(index) == TARGET_REGION)
                        {
                            float value = confidence->computeConfidenceOfOneVoxel(coord, inpaintingIterationNumber);
                            volume->setVoxelToValue(coord, value);
                        }
                    }
                }
            }
            return volume;
        }

        Volume* ConfidenceOrder::plotDataTermToVolume()
        {
            Volume* volume = new FloatVolume(volumeResolution, 0.0f);

            Vec3ui coord(0, 0, 0);
            for (coord.z = 0; coord.z < volumeResolution.z; coord.z++)
            {
                for (coord.y = 0; coord.y < volumeResolution.y; coord.y++)
                {
                    for (coord.x = 0; coord.x < volumeResolution.x; coord.x++)
                    {
                        float value = dataTerm->computeDataTermForOneVoxel(coord, 255.0f);
                        volume->setVoxelToValue(coord, value);
                    }
                }
            }
            return volume;
        }

        Volume* ConfidenceOrder::plotImageGradientToVolume()
        {
            Volume* volume = new FloatVolume(volumeResolution, 0.0f);

            Vec3ui coord(0, 0, 0);
            for (coord.z = 1; coord.z < volumeResolution.z-1; coord.z++)
            {
                for (coord.y = 1; coord.y < volumeResolution.y-1; coord.y++)
                {
                    for (coord.x = 1; coord.x < volumeResolution.x-1; coord.x++)
                    {
                        float value = dataTerm->computeImageGradient(coord).getLengthF();
                        volume->setVoxelToValue(coord, value);
                    }
                }
            }
            return volume;
        }

        Volume* ConfidenceOrder::plotMaskNormalToVolume()
        {
            Volume* volume = new FloatVolume(volumeResolution, 0.0f);

            Vec3ui coord(0, 0, 0);
            for (coord.z = 1; coord.z < volumeResolution.z-1; coord.z++)
            {
                for (coord.y = 1; coord.y < volumeResolution.y-1; coord.y++)
                {
                    for (coord.x = 1; coord.x < volumeResolution.x-1; coord.x++)
                    {
                        float value = dataTerm->computeMaskNormal(coord).getLengthF();
                        volume->setVoxelToValue(coord, value);
                    }
                }
            }
            return volume;
        }

        Volume* ConfidenceOrder::plotComputeFrontToVolume()
        {
            return front->plotToVolume( volumeResolution );
        }

    } // namespace inpainting
} // namespace ettention