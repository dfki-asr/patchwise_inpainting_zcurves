#include "stdafx.h"

#include "L1Distance.h"
#include "StatusFlags.h"
#include "zcurve/ZCurveEntry.h"

namespace ettention
{
    namespace inpainting
    {
		template<class TDATA>
		L1Distance<TDATA>::L1Distance()
		{

		}

		template<class TDATA>
		L1Distance<TDATA>::~L1Distance()
		{

		}

		template<class TDATA>
		float L1Distance<TDATA>::distance( const HyperCube& zeroDistanceRange, const TDATA& point, float maxDistance)
		{
			float distance = 0.0f;

			const unsigned int size = (unsigned int) point.size();
			for (unsigned int i = 0; i < size; i++)
			{
				float distanceInDimension = 0.0f;

				if (point[i] < zeroDistanceRange.first[i])
					distanceInDimension = fabsf( (float) ( zeroDistanceRange.first[i] - point[i] ) );
				if (point[i] > zeroDistanceRange.last[i])
					distanceInDimension = fabsf( (float) ( point[i] - zeroDistanceRange.last[i] ) );

				distance += distanceInDimension;
				if ( distance > maxDistance)
					break;
			}
			return distance;
		}

		template<class TDATA>
		float ettention::inpainting::L1Distance<TDATA>::distance(const TDATA& pointA, const TDATA& pointB, const TDATA& status)
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
				const float distanceInDimension = fabsf( (float) (pB - pA) );
				distance += distanceInDimension;
			}
			return distance;
		}

		template<class TDATA>
		float ettention::inpainting::L1Distance<TDATA>::distance(const TDATA& pointA, const TDATA& pointB, float maxDistance) 
		{
			float distance = 0.0f;
			const unsigned int size = (unsigned int)pointA.size();
			for (unsigned int i = 0; i < size; i++)
			{
				unsigned char pB = pointB[i];
				unsigned char pA = pointA[i];
				const float distanceInDimension = fabsf( (float) (pB - pA) );
				distance += distanceInDimension;
			}
			return distance;
		}

		template class L1Distance< BytePatchAccess8Bit>;
		template class L1Distance< NDPoint>;

	} // namespace
} // namespace
