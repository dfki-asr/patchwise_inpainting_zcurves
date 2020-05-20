#pragma once

#include "index/Index.h"
#include "index/PatchAccess8Bit.h"

class IDistance;

namespace ettention
{
	class ByteVolume;
	
	namespace inpainting 
	{

		class FilterAndRefine 
		{
		public:
			virtual std::tuple<ettention::Vec3i, float> selectBestPatch(Vec3i targetCoordinate, std::vector<int> knn) = 0;
		};

		template<class NORM>
		class FilterAndRefineImpl : public FilterAndRefine
		{
		public:
			FilterAndRefineImpl(ByteVolume* data, ByteVolume* mask, ByteVolume* dictionary );
			FilterAndRefineImpl(ByteVolume* data, ByteVolume* mask, ByteVolume* dictionary, Vec3ui patchSize);
			virtual ~FilterAndRefineImpl();

			virtual std::tuple<ettention::Vec3i, float> selectBestPatch( Vec3i targetCoordinate, std::vector<int> knn) override;

		protected:
			ByteVolume* data;
			ByteVolume* mask;
			ByteVolume* dictionary;
			Vec3ui patchSize;
		};

	}
}