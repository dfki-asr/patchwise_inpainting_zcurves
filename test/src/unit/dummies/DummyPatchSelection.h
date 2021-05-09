#pragma once

#include "patchselection/PatchSelection.h"

namespace libmmv 
{
    class Volume;
}

namespace inpainting
{
    class ComputeFront;
    class Confidence;
    class DataTerm;

    class DummyPatchSelection : public PatchSelection
    {
    public:
        DummyPatchSelection();
        virtual libmmv::Vec3i selectCenterOfBestPatch(libmmv::Vec3i sourcePosition);
		virtual libmmv::Vec3i adjustTargetPatchPosition(libmmv::Vec3i targetPatchCenter) override;
    };

} // namespace inpainting

