#pragma once

#include <vector>

#include "math/Vec3.h"
#include <algorithm/volumestatistics/VolumeStatistics.h>

#include "index/PatchAccess8Bit.h"
#include "index/NDPointPair.h"
#include "zcurve/ZCurveEntry.h"

namespace ettention
{
	namespace inpainting 
	{

        enum CompareResult { LESS = -1, EQUAL = 0, GREATER = 1 };

        template<class T1, class T2>
        CompareResult compareRelativeToZCurve( const T1& a, const T2& b);

		class PatchLessOnZCurve
		{
		public:
			PatchLessOnZCurve(ByteVolume* volume, Vec3ui patchSize, std::vector<unsigned int> permutation);

			bool operator()(unsigned int a, unsigned int b);
			bool operator()(const std::vector<unsigned char>& a, unsigned int b);
			bool operator()(unsigned int a, const std::vector<unsigned char>& b);
			bool operator()(const std::vector<unsigned char>& a, const std::vector<unsigned char>& b);
			std::string toString() const;

		protected:
			Vec3ui patchSize;
			ByteVolume* volume;
			BytePatchAccess8Bit patchA, patchB;
			size_t counter;
		};

		class IndexEntryLessOnZCurve
		{
		public:
			IndexEntryLessOnZCurve();

			bool operator()(const ZCurveEntry& a, const ZCurveEntry& b);
			bool operator()(const NDPoint& a, const ZCurveEntry& b);
			bool operator()(const ZCurveEntry& a, const NDPoint& b);
			std::string toString() const;
		};

	} // namespace inpainting
} // namespace ettention