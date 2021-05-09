#pragma once

#include "computeorder/ComputeOrder.h"

namespace libmmv 
{
    class ByteVolume;
}

namespace inpainting
{
    class ComputeFront;
    class Confidence;
    class DataTerm;

    class DummyPriority : public inpainting::ComputeOrder
    {
    public:
        DummyPriority(libmmv::ByteVolume* mask );
        virtual ~DummyPriority();
        virtual libmmv::Vec3ui selectCenterOfPatchToProcess( bool pop );
        virtual float computePriorityForVoxel(libmmv::Vec3ui coordinate);
		virtual void setIndex(Index* index);
    };

} // namespace inpainting
