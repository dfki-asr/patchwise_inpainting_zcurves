#include "stdafx.h"

#include "Confidence.h"
#include "StatusFlags.h"

#include "model/volume/ByteVolume.h"

namespace ettention
{
    namespace inpainting
    {

        Confidence::Confidence(ByteVolume* maskVolume, Vec3ui patchSize, bool regularizeConfidence)
            : patchSize(patchSize)
            , maskVolume(maskVolume)
            , regularizeConfidence(regularizeConfidence)
        {
            volumeResolution = maskVolume->getProperties().getVolumeResolution();
        }

        Confidence::~Confidence()
        {

        }

        float Confidence::computeConfidenceOfOneVoxel(Vec3i patchCenterCoord, size_t inpaintingIterationNumber)
        {
            Vec3i upperLeftCornerCoordinate = patchCenterCoord - patchSize / 2;

            BoundingBox3i aabb = maskVolume->getProperties().getMemoryBoundingBox();

            float confidenceSum = 0.0f;
            Vec3i coord;
            for (coord.x = 0; coord.x < patchSize.x; coord.x++)
            {
                for (coord.y = 0; coord.y < patchSize.y; coord.y++)
                {
                    for (coord.z = 0; coord.z < patchSize.z; coord.z++)
                    {
                        const Vec3i coordInTargetVolume = coord + upperLeftCornerCoordinate;
                        if (! aabb.isInside( coordInTargetVolume ) )
                            continue;
                        auto index = maskVolume->calculateVoxelIndex( coordInTargetVolume );
                        unsigned char status = maskVolume->nativeVoxelValue(index);
                        if (status != TARGET_REGION)
                        {
                            if (status == SOURCE_REGION)
                            {
                                confidenceSum += 1.0f;
                            } else {
                                if( regularizeConfidence )
                                    confidenceSum += 1.0f / (float)((inpaintingIterationNumber + 1));
                                else
                                    confidenceSum += 1.0f;
                            }
                        }
                    }
                }
            }

            return confidenceSum / (patchSize.x * patchSize.y * patchSize.z);
        }

    } // namespace inpainting
} // namespace ettention