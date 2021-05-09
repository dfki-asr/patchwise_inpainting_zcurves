#pragma once

#include "index/Index.h"
#include "index/PatchAccess8Bit.h"

class IDistance;

class ByteVolume;
	
namespace inpainting 
{

	class FilterAndRefine 
	{
	public:
		virtual std::tuple<libmmv::Vec3i, float> selectBestPatch(libmmv::Vec3i targetCoordinate, std::vector<int> knn) = 0;
	};

	template<class NORM>
	class FilterAndRefineImpl : public FilterAndRefine
	{
	public:
		FilterAndRefineImpl(libmmv::ByteVolume* data, libmmv::ByteVolume* mask, libmmv::ByteVolume* dictionary );
		FilterAndRefineImpl(libmmv::ByteVolume* data, libmmv::ByteVolume* mask, libmmv::ByteVolume* dictionary, libmmv::Vec3ui patchSize);
		virtual ~FilterAndRefineImpl();

		virtual std::tuple<libmmv::Vec3i, float> selectBestPatch(libmmv::Vec3i targetCoordinate, std::vector<int> knn) override;

	protected:
		libmmv::ByteVolume* data;
		libmmv::ByteVolume* mask;
		libmmv::ByteVolume* dictionary;
		libmmv::Vec3ui patchSize;
	};

}