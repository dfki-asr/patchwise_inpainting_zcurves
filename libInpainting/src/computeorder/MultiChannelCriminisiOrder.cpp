#include "stdafx.h"

#include "MultiChannelCriminisiOrder.h"

#include "Problem.h"
#include "Confidence.h"
#include "MultiChannelDataTerm.h"
#include "index/Index.h"
#include "StatusFlags.h"

namespace inpainting
{

	MultiChannelCriminisiOrder::MultiChannelCriminisiOrder( Problem* problem, ProgressReporter* progress,bool regularizeConfidence)
        : CriminisiOrder( problem, progress, regularizeConfidence)
    {
		z_resolution = problem->data->getProperties().getVolumeResolution().z;
		z_center = z_resolution / 2;
		dataTerm = new MultiChannelDataTerm(problem->data, problem->mask, problem->patchSize);
	}

	MultiChannelCriminisiOrder::~MultiChannelCriminisiOrder()
    {
    }

	int MultiChannelCriminisiOrder::growFront( libmmv::BoundingBox3i region)
	{
		libmmv::Vec3i coord;
		int sizeOfTargetArea = 0;
		coord.z = z_center;
		for (coord.y = region.getMin().y; coord.y <= region.getMax().y; coord.y++)
		{
			for (coord.x = region.getMin().x; coord.x <= region.getMax().x; coord.x++)
			{
				size_t index = mask->getVoxelIndex(coord);
				unsigned char status = mask->nativeVoxelValue(index);

				if (status == TARGET_REGION)
					sizeOfTargetArea += z_resolution;

				if (status == TARGET_REGION || status == EMPTY_REGION)
					continue;

				if (isAnyNeighborInTargetRegion(coord))
				{
					addCoordinateToFront(coord);
				}
			}
		}
		return sizeOfTargetArea;
	}

	void MultiChannelCriminisiOrder::addCoordinateToFront(libmmv::Vec3ui coordinate)
	{
		if (coordinate.z != z_center)
			return;
		ComputeOrder::addCoordinateToFront(coordinate);
	}

} // namespace inpainting
