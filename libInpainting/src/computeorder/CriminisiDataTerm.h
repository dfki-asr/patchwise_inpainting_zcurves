#pragma once

#include "math/Vec3.h"

#include "Gradient3D.h"
#include "DataTerm.h"

namespace ettention
{
	class Volume;

	namespace inpainting
	{

		class CriminisiDataTerm : public DataTerm
		{
		public:
			// maskVolume must be a float volume that contains 0.0 in the target region and 1.0 in the source region
			CriminisiDataTerm(Volume* dataVolume, Volume* maskVolume, Vec3ui patchSize);
			~CriminisiDataTerm();

			float computeDataTermForOneVoxel( Vec3ui voxelCoord, float alpha ) override;
			Vec3f computeImageGradient(Vec3ui voxelCoord);
			Vec3f computeMaskNormal(Vec3ui voxelCoord);

			virtual void outputDebugVolumes(std::string pathToDebugFolder, unsigned int iterationNumber, InpaintingDebugParameters* parameters) override;

		protected:
			Gradient3D gradient;
			Volume* dataVolume;
			Volume* maskVolume;
			Vec3ui volumeResolution;
			Vec3i patchSize;
			Vec3i patchRadius;
		};

	} // namespace inpainting
} // namespace ettention
