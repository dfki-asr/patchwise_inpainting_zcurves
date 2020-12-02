#pragma once

#include "math/Vec2.h"
#include "math/Vec3.h"

#include "Gradient2D.h"
#include "DataTerm.h"

namespace ettention
{
	class Volume;
	class ByteVolume;

	namespace inpainting
	{
		class MultiChannelDataTerm : public DataTerm
		{
		public:
			// maskVolume must be a float volume that contains 0.0 in the target region and 1.0 in the source region
			MultiChannelDataTerm(Volume* dataVolume, ByteVolume* maskVolume, Vec3ui patchSize);
			~MultiChannelDataTerm();

			float computeDataTermForOneVoxel(Vec3ui voxelCoord, float alpha) override;
			Vec2f computeImageGradient(Vec3ui voxelCoord);
			Vec2f computeMaskNormal(Vec3ui voxelCoord);

			virtual void outputDebugVolumes(std::string pathToDebugFolder, unsigned int iterationNumber, InpaintingDebugParameters* parameters) override;

		protected:
			Volume* plotGradientToVolume();
			Volume* plotMaskNormalToVolume();

		protected:
			Gradient2D gradient;
			Volume* dataVolume;
			ByteVolume* maskVolume;
			Vec3ui volumeResolution;
			Vec3i patchSize;
			Vec3i patchRadius;
		};

	} // namespace inpainting
} // namespace ettention
