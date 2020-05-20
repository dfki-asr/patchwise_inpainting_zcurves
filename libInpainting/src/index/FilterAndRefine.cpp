#include "stdafx.h"

#include "FilterAndRefine.h"
#include "costfunction/L1Distance.h"
#include "costfunction/L2Distance.h"

namespace ettention
{
    namespace inpainting
    {
		template<class NORM>
		FilterAndRefineImpl<NORM>::FilterAndRefineImpl(ByteVolume* data, ByteVolume* mask, ByteVolume* dictionary )
			: data(data), mask(mask), dictionary(dictionary)
		{
		}

		template<class NORM>
		FilterAndRefineImpl<NORM>::FilterAndRefineImpl(ByteVolume* data, ByteVolume* mask, ByteVolume* dictionary, Vec3ui patchSize)
			: data(data), mask(mask), dictionary(dictionary), patchSize(patchSize)
		{

		}

		template<class NORM>
		ettention::inpainting::FilterAndRefineImpl<NORM>::~FilterAndRefineImpl()
		{

		}

		template<class NORM>
		std::tuple<ettention::Vec3i, float> FilterAndRefineImpl<NORM>::selectBestPatch(Vec3i targetCoordinate, std::vector<int> knn)
		{
			const Vec3ui dictionaryResolution = dictionary->getProperties().getVolumeResolution();
			const Vec3ui dataResolution = dictionary->getProperties().getVolumeResolution();

			NORM distance;

			BytePatchAccess8Bit targetPatch(data, patchSize, DimensionSelection::standardPermutation(patchSize));
			BytePatchAccess8Bit neightborPatch(dictionary, patchSize, DimensionSelection::standardPermutation(patchSize));
			BytePatchAccess8Bit statusPatch(mask, patchSize, DimensionSelection::standardPermutation(patchSize));

			float bestVectorSpaceDistance = std::numeric_limits<float>::max();
			Vec3i bestPatch;

			targetPatch.setPatchId(targetCoordinate);
			statusPatch.setPatchId(targetCoordinate);
			for (size_t i = 0; i < knn.size(); i++)
			{
				Vec3i neighbourCoordinate = Unflatten3D((unsigned int)knn[i], dictionaryResolution);
				neightborPatch.setPatchId((unsigned int)knn[i]);
				float vectorSpaceDistance = distance.distance(targetPatch, neightborPatch, statusPatch);
				if (vectorSpaceDistance < bestVectorSpaceDistance)
				{
					bestVectorSpaceDistance = vectorSpaceDistance;
					bestPatch = neighbourCoordinate;
				}
			}
			return std::tuple<ettention::Vec3i, float>(bestPatch, bestVectorSpaceDistance);
		}

		template class FilterAndRefineImpl< L1Distance<BytePatchAccess8Bit> >;
		template class FilterAndRefineImpl< L2Distance<BytePatchAccess8Bit> >;

	} // namespace
} // namespace
