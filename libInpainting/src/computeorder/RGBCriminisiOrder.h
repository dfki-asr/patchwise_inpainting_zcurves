#pragma once

#include "CriminisiOrder.h"

namespace ettention
{
    namespace inpainting
    {
		class Index;

        class RGBCriminisiOrder : public CriminisiOrder
        {
        public:
			RGBCriminisiOrder( Problem* problem, ProgressReporter* progress, bool regularizeConfidence = false);
            ~RGBCriminisiOrder();

			virtual void addCoordinateToFront(Vec3ui coordinate) override;
			virtual int growFront(BoundingBox3i region) override;
		protected:
			Index* index;
        };

    } // namespace inpainting
} // namespace ettention
