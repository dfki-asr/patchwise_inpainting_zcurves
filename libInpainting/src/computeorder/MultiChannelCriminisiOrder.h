#pragma once

#include "CriminisiOrder.h"

namespace inpainting
{
	class Index;

    class MultiChannelCriminisiOrder : public CriminisiOrder
    {
    public:
		MultiChannelCriminisiOrder( Problem* problem, ProgressReporter* progress, bool regularizeConfidence = false);
        ~MultiChannelCriminisiOrder();

		virtual void addCoordinateToFront(libmmv::Vec3ui coordinate) override;
		virtual int growFront(libmmv::BoundingBox3i region) override;
	protected:
		Index* index;
        int z_resolution;
        int z_center;
    };

} // namespace inpainting
