#include "stdafx.h"

#include "RGBCriminisiOrder.h"

#include "Problem.h"
#include "Confidence.h"
#include "RGBDataTerm.h"
#include "index/Index.h"
#include "StatusFlags.h"

namespace ettention
{
    namespace inpainting
    {

		RGBCriminisiOrder::RGBCriminisiOrder( Problem* problem, ProgressReporter* progress,bool regularizeConfidence)
            : CriminisiOrder( problem, progress, regularizeConfidence)
        {
			dataTerm = new RGBDataTerm(problem->data, problem->mask, problem->patchSize);
		}

		RGBCriminisiOrder::~RGBCriminisiOrder()
        {
        }

		int RGBCriminisiOrder::growFront(BoundingBox3i region)
		{
			Vec3i coord;
			int sizeOfTargetArea = 0;
			coord.z = 1;
			for (coord.y = region.getMin().y; coord.y <= region.getMax().y; coord.y++)
			{
				for (coord.x = region.getMin().x; coord.x <= region.getMax().x; coord.x++)
				{
					size_t index = mask->getVoxelIndex(coord);
					unsigned char status = mask->nativeVoxelValue(index);

					if (status == TARGET_REGION)
						sizeOfTargetArea += 3;

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

		void RGBCriminisiOrder::addCoordinateToFront(Vec3ui coordinate)
		{
			if (coordinate.z != 1)
				return;
			ComputeOrder::addCoordinateToFront(coordinate);
		}

	} // namespace inpainting
} // namespace ettention