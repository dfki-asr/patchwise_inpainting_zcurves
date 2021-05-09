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
	class CriminisiDataTerm;
	class ProgressReporter;

	class ConfidenceOrder : public ComputeOrder
	{
	public:
		ConfidenceOrder( libmmv::Volume* dataVolume, libmmv::ByteVolume* maskVolume, libmmv::Vec3ui patchSize, ProgressReporter* progress);
		~ConfidenceOrder();

		virtual libmmv::Vec3ui selectCenterOfPatchToProcess(bool shouldPopPatch);
		virtual float computePriorityForVoxel(libmmv::Vec3ui coordinate);

        virtual void outputDebugVolumes(std::string pathToDebugFolder, unsigned int iterationNumber, InpaintingDebugParameters* parameters) override;

		// for testing
		libmmv::Volume* plotConfidenceToVolume();
		libmmv::Volume* plotDataTermToVolume();
		libmmv::Volume* plotImageGradientToVolume();
		libmmv::Volume* plotMaskNormalToVolume();
		libmmv::Volume* plotComputeFrontToVolume();

	protected:
		ProgressReporter* progress;
		libmmv::ByteVolume* maskVolume;
		libmmv::Volume* dataVolume;
		Confidence* confidence;
		CriminisiDataTerm* dataTerm;
	};

} // namespace inpainting

