#pragma once

#include "NDPointPair.h"

namespace inpainting 
{

	class SubspaceProjection : public std::vector< std::vector< float > >
	{
	public:
		SubspaceProjection( int unprojectedSize , int projectedSize );

		NDPoint project( NDPoint unprojected );

		static SubspaceProjection empty();
		static SubspaceProjection identity( int size);
		static SubspaceProjection random( int unprojectedSize, int projectedSize );
	};

}
