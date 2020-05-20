#pragma once

#include "math/Vec3.h"

namespace ettention 
{

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
}
