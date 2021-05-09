#pragma once

#include "libmmv/math/Vec3.h"
#include "ComputeOrder.h"

namespace inpainting
{
    class Confidence;
    class Problem;

    class HighConfidenceOrder : public ComputeOrder
    {
    public:
        HighConfidenceOrder( Problem* problem, ComputeOrder* baseOrder, unsigned int radius );
        ~HighConfidenceOrder();

        virtual libmmv::Vec3ui selectCenterOfPatchToProcess(bool shouldPopPatch) override;
        virtual float computePriorityForVoxel(libmmv::Vec3ui coordinate) override;

        virtual void outputDebugVolumes(std::string pathToDebugFolder, unsigned int iterationNumber, InpaintingDebugParameters* parameters) override;

        virtual void progressFront(libmmv::BoundingBox3i region) override;
        virtual void updatePriorityInRegion(libmmv::Vec3ui from, libmmv::Vec3ui to) override;

        virtual size_t getSizeOfTargetArea() override;

    protected:
        libmmv::Vec3ui getBestCoordinateInProximity(libmmv::Vec3i center );

    protected:
        int radius;
        ComputeOrder* baseOrder;            
        Confidence* confidence;
    };

} // namespace inpainting
