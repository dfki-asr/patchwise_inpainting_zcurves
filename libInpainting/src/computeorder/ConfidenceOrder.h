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
		class CriminisiDataTerm;
		class ProgressReporter;

		class ConfidenceOrder : public ComputeOrder
		{
		public:
			ConfidenceOrder( Volume* dataVolume, ByteVolume* maskVolume, Vec3ui patchSize, ProgressReporter* progress);
			~ConfidenceOrder();

			virtual Vec3ui selectCenterOfPatchToProcess(bool shouldPopPatch);
			virtual float computePriorityForVoxel(Vec3ui coordinate);

            virtual void outputDebugVolumes(std::string pathToDebugFolder, unsigned int iterationNumber, InpaintingDebugParameters* parameters) override;

			// for testing
			Volume* plotConfidenceToVolume();
			Volume* plotDataTermToVolume();
			Volume* plotImageGradientToVolume();
			Volume* plotMaskNormalToVolume();
			Volume* plotComputeFrontToVolume();

		protected:
			ProgressReporter* progress;
			ByteVolume* maskVolume;
			Volume* dataVolume;
			Confidence* confidence;
			CriminisiDataTerm* dataTerm;
		};

	} // namespace inpainting
} // namespace ettention
