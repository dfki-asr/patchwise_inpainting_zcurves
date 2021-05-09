#pragma once

#include "libmmv/math/Vec3.h"
#include "index/NDPointPair.h"
#include "index/PatchAccess8Bit.h"

namespace libmmv
{
	class FloatVolume;
	class ByteVolume;
}

namespace inpainting 
{
	class SubspaceProjection;

    class NDSpaceMapping 
    {
    public:
		NDSpaceMapping(libmmv::ByteVolume* dataVolume, libmmv::ByteVolume* maskVolume, libmmv::Vec3i patchSize, std::vector<unsigned int> permutation);
        virtual ~NDSpaceMapping();

		HyperCube getZeroDistanceRangeForIncompletePatch(libmmv::Vec3i targetPatchCenter);
		HyperCube getProjectedZeroDistanceRangeForIncompletePatch(libmmv::Vec3i targetPatchCenter);
		NDPoint getCurvePointForIncompletePatch(libmmv::Vec3i targetPatchUpperLeft, bool lowerBoundary);
		NDPoint getInterpolatedCurvePointForIncompletePatch(libmmv::Vec3i targetPatchCenter);
		unsigned char generateOneValueForIncompletePatch( unsigned int index, bool lowerBoundary );
		bool isDataAvailableForVoxel(unsigned int index);
		bool isDataAvailableForVoxel(libmmv::Vec3i targetPatchCenter, unsigned int dimension );

	protected:
		std::vector<unsigned int> permutation;
		BytePatchAccess8Bit* data;
		BytePatchAccess8Bit* mask;
		libmmv::Vec3i patchSize;
		libmmv::Vec3i volumeResolution;
	};
}
