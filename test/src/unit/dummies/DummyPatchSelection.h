#pragma once

#include "patchselection/PatchSelection.h"

namespace ettention
{
    class Framework;
    class Volume;
    class GPUMappedVolume;

    namespace inpainting
    {
        class ComputeFront;
        class Confidence;
        class DataTerm;

        class DummyPatchSelection : public PatchSelection
        {
        public:
            DummyPatchSelection();
            virtual Vec3i selectCenterOfBestPatch(Vec3i sourcePosition);
			virtual Vec3i adjustTargetPatchPosition(Vec3i targetPatchCenter) override;
        };

    } // namespace inpainting
} // namespace ettention
