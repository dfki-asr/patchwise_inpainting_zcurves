#pragma once

#include "libmmv/math/Vec3.h"
#include "DimensionSelection.h"
#include "index/PatchAccess8Bit.h"
#include "zcurve/ZCurveKNNQuery.h"

	class ByteVolume;
	
namespace inpainting 
{
	class IndexCannotHandlePositionError : std::runtime_error
	{
	public:
		IndexCannotHandlePositionError(libmmv::Vec3i targetPatchCenter, libmmv::Vec3i minPosition, libmmv::Vec3i maxPosition );
		libmmv::Vec3i targetPatchCenter;
	};

	class Index
	{
	public:
		virtual ~Index();
		virtual std::tuple<libmmv::Vec3i, float> query(libmmv::Vec3i targetPatchCenter) = 0;
		virtual int rateQualityOfIndexForQuery(libmmv::Vec3i targetPatchCenter) = 0;
		virtual libmmv::Vec3i adjustTargetPatchPosition(libmmv::Vec3i targetPatchCenter) = 0;
		virtual std::string to_string() = 0;

		virtual void writeToStream( std::ostream& os ) = 0;
		virtual void loadFromStream( std::istream& is ) = 0;
	};
}