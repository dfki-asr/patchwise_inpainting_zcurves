#include "stdafx.h"

#include "ZCurveOperator.h"
#include "index/PatchAccess8Bit.h"
#include "index/NDPointpair.h"

namespace ettention
{
	namespace inpainting 
	{  

		template<typename TDATA>
		inline bool lessRegardingMostSignificantBit(TDATA a, TDATA b)
		{
			return ( a < b ) && ( a < (a ^ b) );
		}

		template<class T1, class T2>
		CompareResult compareRelativeToZCurve( const T1& a, const T2& b)
        {
			typedef typename T1::value_type TDATA;

            if ( a.size() != b.size() )
                throw std::runtime_error("cannot compare points with unequal dimensions");

			unsigned int bitWiseDifferenceOfBestDimensionSoFar = 0;
			unsigned int bestDimensionSoFar = 0;
			unsigned int size = (unsigned int) a.size();
			for (unsigned int dimension = 0; dimension < size; dimension++)
			{
				auto bitWiseDifference = a[dimension] ^ b[dimension];
				if ( lessRegardingMostSignificantBit<TDATA> ( bitWiseDifferenceOfBestDimensionSoFar, bitWiseDifference) )
				{
					bitWiseDifferenceOfBestDimensionSoFar = bitWiseDifference;
					bestDimensionSoFar = dimension;
				}
			}
			if (a[bestDimensionSoFar] < b[bestDimensionSoFar])
				return LESS;
			if (a[bestDimensionSoFar] > b[bestDimensionSoFar])
				return GREATER;
            return EQUAL;
        }

        PatchLessOnZCurve::PatchLessOnZCurve(ByteVolume* volume, Vec3ui patchSize, std::vector<unsigned int> permutation )
			: volume(volume), patchSize(patchSize)
			, patchA(volume, patchSize, permutation)
			, patchB(volume, patchSize, permutation)
        {
        }

        bool PatchLessOnZCurve::operator()(unsigned int a, unsigned int b) 
        {
			patchA.setPatchId(a);
            patchB.setPatchId(b); 
            return compareRelativeToZCurve< BytePatchAccess8Bit, BytePatchAccess8Bit >(patchA, patchB) == LESS;
        }

        bool PatchLessOnZCurve::operator()(const std::vector<unsigned char>& a, unsigned int b) 
        {
			counter++;
			patchB.setPatchId(b);
            return compareRelativeToZCurve<std::vector<unsigned char>, BytePatchAccess8Bit>(a, patchB) == LESS;
        }

		bool PatchLessOnZCurve::operator()(unsigned int a, const std::vector<unsigned char>& b)
		{
			counter++;
			patchA.setPatchId(a);
			return compareRelativeToZCurve<BytePatchAccess8Bit, std::vector<unsigned char> >(patchA, b) == LESS;
		}

		bool PatchLessOnZCurve::operator()(const std::vector<unsigned char>& a, const std::vector<unsigned char>& b)
		{
			counter++;
			return compareRelativeToZCurve< std::vector<unsigned char>, std::vector<unsigned char> >(a, b) == LESS;
		}

        std::string PatchLessOnZCurve::toString() const
        {
            return "<";
        }

		IndexEntryLessOnZCurve::IndexEntryLessOnZCurve( )
		{
		}

		bool IndexEntryLessOnZCurve::operator()(const ZCurveEntry& a, const ZCurveEntry& b)
		{
			return compareRelativeToZCurve<NDPoint, NDPoint>(a.value, b.value) == LESS;
		}

		bool IndexEntryLessOnZCurve::operator()(const ZCurveEntry& a, const NDPoint& b)
		{
			return compareRelativeToZCurve<NDPoint, NDPoint>(a.value, b) == LESS;
		}

		bool IndexEntryLessOnZCurve::operator()(const NDPoint& a, const ZCurveEntry& b)
		{
			return compareRelativeToZCurve<NDPoint, NDPoint>(a, b.value) == LESS;
		}

		std::string IndexEntryLessOnZCurve::toString() const
		{
			return "<";
		}

		// explicit instantiation of non-standard template argument required for unit tests
		template CompareResult compareRelativeToZCurve<std::vector<unsigned short>, std::vector<unsigned short>>(const std::vector<unsigned short>&, const std::vector<unsigned short>&);
		template CompareResult compareRelativeToZCurve<NDPoint, NDPoint>(const NDPoint&, const NDPoint&);

	} // namespace inpainting
} // namespace ettention