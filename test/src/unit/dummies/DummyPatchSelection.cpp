#include "stdafx.h"

#include "DummyPatchSelection.h"

namespace ettention
{
    namespace inpainting
    {

        DummyPatchSelection::DummyPatchSelection()
            : PatchSelection( nullptr, nullptr, nullptr)
        {

        }

        ettention::Vec3i DummyPatchSelection::selectCenterOfBestPatch(Vec3i sourcePosition)
        {
            return Vec3i(0, 0, 0);
        }

		ettention::Vec3i DummyPatchSelection::adjustTargetPatchPosition(Vec3i targetPatchCenter)
		{
			return targetPatchCenter;
		}

	} // namespace inpainting
} // namespace ettention