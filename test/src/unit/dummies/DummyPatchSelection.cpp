#include "stdafx.h"

#include "DummyPatchSelection.h"

namespace inpainting
{

    DummyPatchSelection::DummyPatchSelection()
        : PatchSelection( nullptr, nullptr, nullptr)
    {

    }

    libmmv::Vec3i DummyPatchSelection::selectCenterOfBestPatch(libmmv::Vec3i sourcePosition)
    {
        return libmmv::Vec3i(0, 0, 0);
    }

    libmmv::Vec3i DummyPatchSelection::adjustTargetPatchPosition(libmmv::Vec3i targetPatchCenter)
	{
		return targetPatchCenter;
	}

} // namespace inpainting
