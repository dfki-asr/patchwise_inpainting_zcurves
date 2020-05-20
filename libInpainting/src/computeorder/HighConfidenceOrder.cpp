#include "stdafx.h"

#include "HighConfidenceOrder.h"
#include "Problem.h"

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

        HighConfidenceOrder::HighConfidenceOrder( Problem* problem, ComputeOrder* baseOrder, unsigned int radius)
            : ComputeOrder( baseOrder->getMask() )
            , baseOrder( baseOrder )
            , radius( radius )
        {
            confidence = new Confidence( problem->mask, problem->patchSize, false );
            front = baseOrder->front;
        }

        HighConfidenceOrder::~HighConfidenceOrder()
        {
            delete baseOrder;
            delete confidence;
        }

        Vec3ui HighConfidenceOrder::selectCenterOfPatchToProcess( bool shouldPopPatch )
        {
            Vec3ui center = baseOrder->selectCenterOfPatchToProcess(shouldPopPatch);

            Vec3ui result = getBestCoordinateInProximity( center );

            return result;
        }

        float HighConfidenceOrder::computePriorityForVoxel(Vec3ui coordinate)
        {
            return baseOrder->computePriorityForVoxel(coordinate);
        }

		void HighConfidenceOrder::outputDebugVolumes(std::string pathToDebugFolder, unsigned int iterationNumber, InpaintingDebugParameters* parameters)
        {
            baseOrder->outputDebugVolumes( pathToDebugFolder, iterationNumber, parameters );
        }

        void HighConfidenceOrder::progressFront(BoundingBox3i region)
        {
            baseOrder->progressFront(region);
        }

        void HighConfidenceOrder::updatePriorityInRegion(Vec3ui from, Vec3ui to)
        {
            baseOrder->updatePriorityInRegion( from, to );
        }

        size_t HighConfidenceOrder::getSizeOfTargetArea()
        {
            return baseOrder->getSizeOfTargetArea();
        }

        ettention::Vec3ui HighConfidenceOrder::getBestCoordinateInProximity( Vec3i center )
        {
            Vec3ui resolution = mask->getProperties().getVolumeResolution();
            Vec3i position;
            Vec3i bestPositionSoFar = center;
            float bestConfidenceSoFar = confidence->computeConfidenceOfOneVoxel(center, 0);
            for (position.z = center.z - radius; position.z <= center.z + radius; position.z++)
                for (position.y = center.y - radius; position.y <= center.y + radius; position.y++)
                    for (position.x = center.x - radius; position.x <= center.x + radius; position.x++)
                    {
                        if ( position.x < 0 || position.y < 0 || position.z < 0 )
                            continue;
                        if ( position.x >= (int) resolution.x || position.y >= (int) resolution.y || position.z >= (int) resolution.z )
                            continue;
                        float confidenceAtPosition = confidence->computeConfidenceOfOneVoxel(position, 0);
                        if ( confidenceAtPosition > bestConfidenceSoFar )
                        {
                            // std::cout << "improved confidence from " << bestConfidenceSoFar;
                            bestConfidenceSoFar = confidenceAtPosition;
                            bestPositionSoFar = position;
                            // std::cout << " to " << bestConfidenceSoFar << std::endl;
                        }
                    }
            return bestPositionSoFar;
        }

    } // namespace inpainting
} // namespace ettention