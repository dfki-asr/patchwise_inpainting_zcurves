#pragma once

#include "libmmv/math/Vec3.h"

namespace libmmv
{
	class ByteVolume;
}

namespace inpainting
{

	class Confidence
	{
	public:
		// maskVolume must be a float volume that contains 0.0 in the target region and 1.0 in the source region
		Confidence(libmmv::ByteVolume* maskVolume, libmmv::Vec3ui patchSize, bool regularizeConfidence = false );
		~Confidence();

		float computeConfidenceOfOneVoxel(libmmv::Vec3i patchCenterCoord, size_t inpaintingIterationNumber );
        bool regularizeConfidence;

	protected:
		libmmv::ByteVolume* maskVolume;
		libmmv::Vec3ui volumeResolution;
		libmmv::Vec3i patchSize;
	};

} // namespace inpainting

