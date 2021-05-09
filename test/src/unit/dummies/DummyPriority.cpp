#include "stdafx.h"

#include "DummyPriority.h"
#include "libmmv/model/volume/ByteVolume.h"

#include "computeorder/ComputeFront.h"

namespace inpainting
{
    DummyPriority::DummyPriority(libmmv::ByteVolume* mask )
        : ComputeOrder( mask )
    {
    }

    DummyPriority::~DummyPriority()
    {
    }

    libmmv::Vec3ui DummyPriority::selectCenterOfPatchToProcess(bool pop)
    {
        return libmmv::Vec3ui(0,0,0);
    }

    float DummyPriority::computePriorityForVoxel(libmmv::Vec3ui coordinate)
    {
        return 0.0f;
    }

	void DummyPriority::setIndex(Index* index)
	{

	}

} // namespace inpainting
