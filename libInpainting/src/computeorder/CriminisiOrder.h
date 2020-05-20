#pragma once

#include "math/Vec3.h"
#include "ComputeOrder.h"

namespace ettention
{
    class Volume;
    
    namespace inpainting
    {
        class ComputeFront;
        class Confidence;
		class DataTerm;
		class ProgressReporter;
        class Problem;

        class CriminisiOrder : public ComputeOrder
        {
        public:
            CriminisiOrder( Problem* problem, ProgressReporter* progress, bool regularizeConfidence = false);
            virtual ~CriminisiOrder();

            virtual Vec3ui selectCenterOfPatchToProcess(bool shouldPopPatch) override;
            virtual float computePriorityForVoxel(Vec3ui coordinate) override;

            virtual void outputDebugVolumes(std::string pathToDebugFolder, unsigned int iterationNumber, InpaintingDebugParameters* parameters) override;

			virtual void init();

            // for testing
            Volume* plotConfidenceToVolume();
            Volume* plotDataTermToVolume();
            Volume* plotComputeFrontToVolume();

        protected:
			Problem* problem;
            ProgressReporter* progress;
            Confidence* confidence;
			DataTerm* dataTerm;
        };

    } // namespace inpainting
} // namespace ettention
