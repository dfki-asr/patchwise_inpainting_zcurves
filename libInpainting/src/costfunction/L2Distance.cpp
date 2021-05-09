#include "stdafx.h"

#include "L2Distance.h"
#include "StatusFlags.h"
#include "zcurve/ZCurveEntry.h"

namespace inpainting
{
	template<class TDATA>
	L2Distance<TDATA>::L2Distance()
	{

	}

	template<class TDATA>
	L2Distance<TDATA>::~L2Distance()
	{

	}

	template<class TDATA>
	float L2Distance<TDATA>::distance( const HyperCube& zeroDistanceRange, const TDATA& point, float maxDistance)
	{
		float distance = 0.0f;
		float maxDistancePower = std::numeric_limits<float>::max();

		if (maxDistance < std::numeric_limits<float>::max())
			maxDistancePower = maxDistance * maxDistance;

		const unsigned int size = (unsigned int) point.size();
		for (unsigned int i = 0; i < size; i++)
		{
			float distanceInDimension = 0.0f;

			if (point[i] < zeroDistanceRange.first[i])
				distanceInDimension = (float) ( zeroDistanceRange.first[i] - point[i] );
			if (point[i] > zeroDistanceRange.last[i])
				distanceInDimension = (float) ( point[i] - zeroDistanceRange.last[i] );

			distance += distanceInDimension * distanceInDimension;
			if ( distance > maxDistancePower )
				break;
		}
		return sqrtf(distance);
	}

	template<class TDATA>
	float L2Distance<TDATA>::distance(const TDATA& pointA, const TDATA& pointB, const TDATA& status)
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
			const float distanceInDimension = (float) (pB - pA);
			distance += distanceInDimension*distanceInDimension;
		}
		return sqrtf(distance);
	}

	template<class TDATA>
	float L2Distance<TDATA>::distance(const TDATA& pointA, const TDATA& pointB, float maxDistance)
	{
		float distance = 0.0f;
		const unsigned int size = (unsigned int)pointA.size();
		for (unsigned int i = 0; i < size; i++)
		{
			unsigned char pB = pointB[i];
			unsigned char pA = pointA[i];
			const float distanceInDimension = (float) (pB - pA);
			distance += distanceInDimension*distanceInDimension;
		}
		return sqrtf(distance);
	}

	template class L2Distance< BytePatchAccess8Bit>;
	template class L2Distance< NDPoint>;

} // namespace
