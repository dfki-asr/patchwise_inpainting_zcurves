#pragma once

#include "libmmv/math/Vec3.h"
#include "index/PatchAccess8Bit.h"
#include "index/NDPointPair.h"

class FloatVolume;
class ByteVolume;
	
namespace inpainting 
{
	template<class TDATA>
	class LPDistance
	{
	public:
		LPDistance( float p = 2.0f );
		~LPDistance();
			
		float distance( const HyperCube& zeroDistanceRange, const TDATA& point, float maxDistance );
		float distance( const TDATA& pointA, const TDATA& pointB, const TDATA& status );
		float distance( const TDATA& pointA, const TDATA& pointB, float maxDistance );

	protected:
		float p;
	};

}