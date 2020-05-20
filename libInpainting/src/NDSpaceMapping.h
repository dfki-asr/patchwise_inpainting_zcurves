#pragma once

#include "math/Vec3.h"
#include "index/NDPointPair.h"
#include "index/PatchAccess8Bit.h"

namespace ettention
{
    class FloatVolume;
    class ByteVolume; 

    namespace inpainting 
	{
		class SubspaceProjection;

        class NDSpaceMapping 
        {
        public:
			NDSpaceMapping( ByteVolume* dataVolume, ByteVolume* maskVolume, Vec3i patchSize, std::vector<unsigned int> permutation);
            virtual ~NDSpaceMapping();

			HyperCube getZeroDistanceRangeForIncompletePatch(Vec3i targetPatchCenter);
			HyperCube getProjectedZeroDistanceRangeForIncompletePatch(Vec3i targetPatchCenter);
			NDPoint getCurvePointForIncompletePatch(Vec3i targetPatchUpperLeft, bool lowerBoundary);
			NDPoint getInterpolatedCurvePointForIncompletePatch(Vec3i targetPatchCenter);
			unsigned char generateOneValueForIncompletePatch( unsigned int index, bool lowerBoundary );
			bool isDataAvailableForVoxel(unsigned int index);
			bool isDataAvailableForVoxel(Vec3i targetPatchCenter, unsigned int dimension );

		protected:
			std::vector<unsigned int> permutation;
			BytePatchAccess8Bit* data;
			BytePatchAccess8Bit* mask;
			Vec3i patchSize;
			Vec3i volumeResolution;
		};
    }
}