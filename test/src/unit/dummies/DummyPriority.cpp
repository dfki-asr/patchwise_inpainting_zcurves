#include "stdafx.h"

#include "DummyPriority.h"
#include "model/volume/ByteVolume.h"
#include "computeorder/ComputeFront.h"

namespace ettention
{
    namespace inpainting
    {
        DummyPriority::DummyPriority( ByteVolume* mask )
            : ComputeOrder( mask )
        {
        }

        DummyPriority::~DummyPriority()
        {
        }

        ettention::Vec3ui DummyPriority::selectCenterOfPatchToProcess(bool pop)
        {
            return Vec3ui(0,0,0);
        }

        float DummyPriority::computePriorityForVoxel(Vec3ui coordinate)
        {
            return 0.0f;
        }

		void DummyPriority::setIndex(Index* index)
		{

		}

	} // namespace inpainting
} // namespace ettention