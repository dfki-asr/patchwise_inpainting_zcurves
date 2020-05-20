#pragma once

#include "math/Vec3.h"

namespace ettention
{
	class ByteVolume;

	namespace inpainting
	{

		class Confidence
		{
		public:
			// maskVolume must be a float volume that contains 0.0 in the target region and 1.0 in the source region
			Confidence(ByteVolume* maskVolume, Vec3ui patchSize, bool regularizeConfidence = false );
			~Confidence();

			float computeConfidenceOfOneVoxel( Vec3i patchCenterCoord, size_t inpaintingIterationNumber );
            bool regularizeConfidence;

		protected:
			ByteVolume* maskVolume;
			Vec3ui volumeResolution;
			Vec3i patchSize;
		};

	} // namespace inpainting
} // namespace ettention
