#pragma once

#include "math/Vec3.h"
#include "DimensionSelection.h"
#include "index/PatchAccess8Bit.h"
#include "zcurve/ZCurveKNNQuery.h"

namespace ettention
{
	class ByteVolume;
	
	namespace inpainting 
	{
		class IndexCannotHandlePositionError : std::runtime_error
		{
		public:
			IndexCannotHandlePositionError( Vec3i targetPatchCenter, Vec3i minPosition, Vec3i maxPosition );
			Vec3i targetPatchCenter;
		};

		class Index
		{
		public:
			virtual ~Index();
			virtual std::tuple<Vec3i, float> query(Vec3i targetPatchCenter) = 0;
			virtual int rateQualityOfIndexForQuery(Vec3i targetPatchCenter) = 0;
			virtual Vec3i adjustTargetPatchPosition(Vec3i targetPatchCenter) = 0;
			virtual std::string to_string() = 0;

			virtual void writeToStream( std::ostream& os ) = 0;
			virtual void loadFromStream( std::istream& is ) = 0;
		};
    }
}