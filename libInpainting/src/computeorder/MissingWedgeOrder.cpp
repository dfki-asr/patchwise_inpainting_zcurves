#include "stdafx.h"

#include "MissingWedgeOrder.h"
#include "Problem.h"

#include "Confidence.h"
#include "CriminisiDataTerm.h"
#include "ComputeFront.h"
#include "Gradient3D.h"
#include "ComputeFrontInitializer.h"
#include "StatusFlags.h"

#include "libmmv/model/volume/ByteVolume.h"

    namespace inpainting
    {

        MissingWedgeOrder::MissingWedgeOrder( Problem* problem, ComputeOrder* baseOrder)
            : ComputeOrder( baseOrder->getMask() )
            , baseOrder( baseOrder )
            , patchSize(problem->patchSize)
        {
            confidence = new Confidence( problem->mask, problem->patchSize, false );
            front = baseOrder->front;
        }

        MissingWedgeOrder::~MissingWedgeOrder()
        {
            delete baseOrder;
            delete confidence;
        }

        libmmv::Vec3ui MissingWedgeOrder::selectCenterOfPatchToProcess( bool shouldPopPatch )
        {
            libmmv::Vec3ui center = baseOrder->selectCenterOfPatchToProcess(shouldPopPatch);

            libmmv::Vec3ui result = shiftCoordinateToPatchBorderCenter( center );

            return result;
        }

        float MissingWedgeOrder::computePriorityForVoxel(libmmv::Vec3ui coordinate)
        {
            return baseOrder->computePriorityForVoxel(coordinate);
        }

		void MissingWedgeOrder::outputDebugVolumes(std::string pathToDebugFolder, unsigned int iterationNumber, InpaintingDebugParameters* parameters)
        {
            baseOrder->outputDebugVolumes( pathToDebugFolder, iterationNumber, parameters);
        }

        void MissingWedgeOrder::progressFront(libmmv::BoundingBox3i region)
        {
            baseOrder->progressFront(region);
        }

        void MissingWedgeOrder::updatePriorityInRegion(libmmv::Vec3ui from, libmmv::Vec3ui to)
        {
            baseOrder->updatePriorityInRegion( from, to );
        }

        size_t MissingWedgeOrder::getSizeOfTargetArea()
        {
            return baseOrder->getSizeOfTargetArea();
        }

        libmmv::Vec3ui MissingWedgeOrder::shiftCoordinateToPatchBorderCenter(libmmv::Vec3ui center )
        {
            libmmv::Vec3ui resolution = mask->getProperties().getVolumeResolution();
            libmmv::Vec3ui shiftedCoordinate;

            if( center.z > (resolution.z / 2) )
                shiftedCoordinate = libmmv::Vec3ui(center.x, center.y, center.z - (patchSize.z / 2) + 1);
            else
                shiftedCoordinate = libmmv::Vec3ui(center.x, center.y, center.z + (patchSize.z / 2) - 1);

            return shiftedCoordinate;
        }

    } // namespace inpainting
