#pragma once

#include "libmmv/math/Vec3.h"
#include "ComputeOrder.h"

namespace libmmv
{
    class Volume;
}
    
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

        virtual libmmv::Vec3ui selectCenterOfPatchToProcess(bool shouldPopPatch) override;
        virtual float computePriorityForVoxel(libmmv::Vec3ui coordinate) override;

        virtual void outputDebugVolumes(std::string pathToDebugFolder, unsigned int iterationNumber, InpaintingDebugParameters* parameters) override;

		virtual void init();

        // for testing
        libmmv::Volume* plotConfidenceToVolume();
        libmmv::Volume* plotDataTermToVolume();
        libmmv::Volume* plotComputeFrontToVolume();

    protected:
		Problem* problem;
        ProgressReporter* progress;
        Confidence* confidence;
		DataTerm* dataTerm;
    };

} // namespace inpainting
