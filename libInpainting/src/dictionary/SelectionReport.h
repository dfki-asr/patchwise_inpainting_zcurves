#pragma once

#include "libmmv/math/Vec3.h"

namespace inpainting 
{

	class SelectionReport
	{
	public:
		float finalCostFunction;
		float minimalSelectivity;
		float minimalConfidence;
		unsigned int numberOfNodesReturnedFromTraversal;
		unsigned int traversingBothSidesBecauseOfInsufficientDiscriminatingSpread;
		unsigned int traversingBothSidesBecauseOfInsufficientConfidence;
	};

}

