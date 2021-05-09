#pragma once

#include "libmmv/math/Vec2.h"
#include "libmmv/math/Vec3.h"

#include "Gradient2D.h"
#include "DataTerm.h"

namespace libmmv
{
	class Volume;
	class ByteVolume;
}

namespace inpainting
{
	class MultiChannelDataTerm : public DataTerm
	{
	public:
		// maskVolume must be a float volume that contains 0.0 in the target region and 1.0 in the source region
		MultiChannelDataTerm(libmmv::Volume* dataVolume, libmmv::ByteVolume* maskVolume, libmmv::Vec3ui patchSize);
		~MultiChannelDataTerm();

		float computeDataTermForOneVoxel(libmmv::Vec3ui voxelCoord, float alpha) override;
		libmmv::Vec2f computeImageGradient(libmmv::Vec3ui voxelCoord);
		libmmv::Vec2f computeMaskNormal(libmmv::Vec3ui voxelCoord);

		virtual void outputDebugVolumes(std::string pathToDebugFolder, unsigned int iterationNumber, InpaintingDebugParameters* parameters) override;

	protected:
		libmmv::Volume* plotGradientToVolume();
		libmmv::Volume* plotMaskNormalToVolume();

	protected:
		Gradient2D gradient;
		libmmv::Volume* dataVolume;
		libmmv::ByteVolume* maskVolume;
		libmmv::Vec3ui volumeResolution;
		libmmv::Vec3i patchSize;
		libmmv::Vec3i patchRadius;
	};

	} // namespace inpainting
