#pragma once

#include "libmmv/math/Vec3.h"

#include "Gradient3D.h"
#include "DataTerm.h"

namespace libmmv
{
	class Volume;
}

namespace inpainting
{

	class CriminisiDataTerm : public DataTerm
	{
	public:
		// maskVolume must be a float volume that contains 0.0 in the target region and 1.0 in the source region
		CriminisiDataTerm(libmmv::Volume* dataVolume, libmmv::Volume* maskVolume, libmmv::Vec3ui patchSize);
		~CriminisiDataTerm();

		float computeDataTermForOneVoxel(libmmv::Vec3ui voxelCoord, float alpha ) override;
		libmmv::Vec3f computeImageGradient(libmmv::Vec3ui voxelCoord);
		libmmv::Vec3f computeMaskNormal(libmmv::Vec3ui voxelCoord);

		virtual void outputDebugVolumes(std::string pathToDebugFolder, unsigned int iterationNumber, InpaintingDebugParameters* parameters) override;

	protected:
		Gradient3D gradient;
		libmmv::Volume* dataVolume;
		libmmv::Volume* maskVolume;
		libmmv::Vec3ui volumeResolution;
		libmmv::Vec3i patchSize;
		libmmv::Vec3i patchRadius;
	};

} // namespace inpainting

