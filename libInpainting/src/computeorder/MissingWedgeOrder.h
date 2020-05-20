#pragma once

#include "math/Vec3.h"
#include "ComputeOrder.h"

namespace ettention
{
    namespace inpainting
    {
        class Confidence;
        class Problem;

        class MissingWedgeOrder : public ComputeOrder
        {
        public:
            MissingWedgeOrder( Problem* problem, ComputeOrder* baseOrder );
            ~MissingWedgeOrder();

            virtual Vec3ui selectCenterOfPatchToProcess(bool shouldPopPatch) override;
            virtual float computePriorityForVoxel(Vec3ui coordinate) override;

            virtual void outputDebugVolumes(std::string pathToDebugFolder, unsigned int iterationNumber, InpaintingDebugParameters* parameters) override;

            virtual void progressFront(BoundingBox3i region) override;
            virtual void updatePriorityInRegion(Vec3ui from, Vec3ui to) override;

            virtual size_t getSizeOfTargetArea() override;

        protected:
            Vec3ui shiftCoordinateToPatchBorderCenter( Vec3ui center );

        protected:
            ComputeOrder* baseOrder;            
            Confidence* confidence;
            Vec3ui patchSize;
        };

    } // namespace inpainting
} // namespace ettention
