#pragma once

#include "libmmv/math/Vec3.h"
#include "ComputeOrder.h"

namespace inpainting
{
    class Confidence;
    class Problem;

    class MissingWedgeOrder : public ComputeOrder
    {
    public:
        MissingWedgeOrder( Problem* problem, ComputeOrder* baseOrder );
        ~MissingWedgeOrder();

        virtual libmmv::Vec3ui selectCenterOfPatchToProcess(bool shouldPopPatch) override;
        virtual float computePriorityForVoxel(libmmv::Vec3ui coordinate) override;

        virtual void outputDebugVolumes(std::string pathToDebugFolder, unsigned int iterationNumber, InpaintingDebugParameters* parameters) override;

        virtual void progressFront(libmmv::BoundingBox3i region) override;
        virtual void updatePriorityInRegion(libmmv::Vec3ui from, libmmv::Vec3ui to) override;

        virtual size_t getSizeOfTargetArea() override;

    protected:
        libmmv::Vec3ui shiftCoordinateToPatchBorderCenter(libmmv::Vec3ui center );

    protected:
        ComputeOrder* baseOrder;            
        Confidence* confidence;
        libmmv::Vec3ui patchSize;
    };

} // namespace inpainting
