#include "stdafx.h"

#include "NDSpaceMapping.h"

#include "algorithm/Coordinates.h"
#include "StatusFlags.h"

namespace ettention
{
    namespace inpainting
    {

		NDSpaceMapping::NDSpaceMapping(ByteVolume* dataVolume, ByteVolume* maskVolume, Vec3i patchSize, std::vector<unsigned int> permutation )
			: patchSize(patchSize), permutation(permutation)
		{
			volumeResolution = dataVolume->getProperties().getVolumeResolution();
			data = new BytePatchAccess8Bit(dataVolume, patchSize, permutation);
			mask = new BytePatchAccess8Bit(maskVolume, patchSize, permutation);
		}

		NDSpaceMapping::~NDSpaceMapping()
		{
			delete mask;
			delete data;
		}

		HyperCube NDSpaceMapping::getZeroDistanceRangeForIncompletePatch( Vec3i targetPatchCenter )
		{
			NDPoint first = getCurvePointForIncompletePatch(targetPatchCenter, true);
			NDPoint last = getCurvePointForIncompletePatch(targetPatchCenter, false);
			return HyperCube(first, last);
		}

		NDPoint NDSpaceMapping::getCurvePointForIncompletePatch(Vec3i targetPatchCenter, bool lowerBoundary)
		{
			NDPoint patch( permutation.size() );

			unsigned int index = Flatten3D(targetPatchCenter, volumeResolution);
			data->setPatchId(index);
			mask->setPatchId(index);

			for ( unsigned int index = 0; index < permutation.size(); index++)
			{
				patch[index] = generateOneValueForIncompletePatch( index, lowerBoundary );
			}
			return patch;
		}

		ettention::inpainting::NDPoint NDSpaceMapping::getInterpolatedCurvePointForIncompletePatch(Vec3i targetPatchCenter)
		{
			NDPoint point(permutation.size());

			unsigned int index = Flatten3D(targetPatchCenter, volumeResolution);
			data->setPatchId(index);
			mask->setPatchId(index);

			int average = 0;
			int count = 0;
			for (unsigned int index = 0; index < permutation.size(); index++)
			{
				if (isDataAvailableForVoxel(index))
				{
					average += data->operator[](index);
					count++;
				}
			}
			average /= count;

			for (unsigned int index = 0; index < permutation.size(); index++)
			{
				if (isDataAvailableForVoxel(index))
					point[index] = data->operator[](index);
				else
					point[index] = average;
			}

			return point;
		}

		unsigned char NDSpaceMapping::generateOneValueForIncompletePatch(unsigned int index, bool lowerBoundary)
		{
			if ( isDataAvailableForVoxel( index ) )
			{
				return data->operator[](index);
			}
			if (lowerBoundary)
				return 0;
			else
				return 255;
		}

		bool NDSpaceMapping::isDataAvailableForVoxel( unsigned int index )
		{
			const unsigned char statusFlag = mask->operator[](index);
			if (statusFlag == TARGET_REGION || statusFlag == EMPTY_REGION)
				return false;
			return true;
		}

		bool NDSpaceMapping::isDataAvailableForVoxel(Vec3i targetPatchCenter, unsigned int dimension)
		{
			unsigned int index = Flatten3D(targetPatchCenter, volumeResolution);
			mask->setPatchId(index);
			return isDataAvailableForVoxel( dimension );
		}

	} // namespace
} // namespace
