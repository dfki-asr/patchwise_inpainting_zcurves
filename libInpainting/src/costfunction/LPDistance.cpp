#include "stdafx.h"

#include "LPDistance.h"
#include "StatusFlags.h"
#include "math/Approximate.h"
#include "zcurve/ZCurveEntry.h"

namespace ettention
{
    namespace inpainting
    {
		template<class TDATA>
		LPDistance<TDATA>::LPDistance(float p)
			: p(p)
		{

		}

		template<class TDATA>
		LPDistance<TDATA>::~LPDistance()
		{

		}

		template<class TDATA>
		float LPDistance<TDATA>::distance( const HyperCube& zeroDistanceRange, const TDATA& point, float maxDistance)
		{
			float distance = 0.0f;
			float maxDistancePower = std::numeric_limits<float>::max();

			if (maxDistance < std::numeric_limits<float>::max())
				maxDistancePower = Approximate::fastpow(maxDistance, p);

			const unsigned int size = (unsigned int) point.size();
			for (unsigned int i = 0; i < size; i++)
			{
				float distanceInDimension = 0.0f;

				if (point[i] < zeroDistanceRange.first[i])
					distanceInDimension = (float) ( zeroDistanceRange.first[i] - point[i] );
				if (point[i] > zeroDistanceRange.last[i])
					distanceInDimension = (float) ( point[i] - zeroDistanceRange.last[i] );

				distance += Approximate::fastpow( distanceInDimension, p );
				if ( distance > maxDistancePower )
					break;
			}
			return Approximate::fastpow(distance, 1.0f / p);
		}

		template<class TDATA>
		float ettention::inpainting::LPDistance<TDATA>::distance(const TDATA& pointA, const TDATA& pointB, const TDATA& status)
		{
			float distance = 0.0f;
			const unsigned int size = (unsigned int) pointA.size();
			for (unsigned int i = 0; i < size; i++)
			{
				const unsigned char pixelStatus = status[i];
				if (pixelStatus == EMPTY_REGION || pixelStatus == TARGET_REGION)
					continue;
				unsigned char pB = pointB[i];
				unsigned char pA = pointA[i];
				float distanceInDimension = (float) std::abs( pB - pA );
				distance += Approximate::fastpow(distanceInDimension, p);
			}
			return Approximate::fastpow(distance, 1.0f / p);
		}

		template<class TDATA>
		float ettention::inpainting::LPDistance<TDATA>::distance(const TDATA& pointA, const TDATA& pointB, float maxDistance)
		{
			float distance = 0.0f;
			float maxDistancePower = std::numeric_limits<float>::max();
			const unsigned int size = (unsigned int)pointA.size();
			for (unsigned int i = 0; i < size; i++)
			{
				unsigned char pB = pointB[i];
				unsigned char pA = pointA[i];
				float distanceInDimension = (float)std::abs(pB - pA);
				distance += Approximate::fastpow(distanceInDimension, p);
				if (distance > maxDistancePower)
					break;
			}
			return Approximate::fastpow(distance, 1.0f / p);
		}

		template class LPDistance< BytePatchAccess8Bit>;
		template class LPDistance< NDPoint>;

	} // namespace
} // namespace
