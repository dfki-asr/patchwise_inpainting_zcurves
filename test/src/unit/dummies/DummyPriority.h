#pragma once

#include "computeorder/ComputeOrder.h"

namespace ettention
{
    class Framework;
    class ByteVolume;

    namespace inpainting
    {
        class ComputeFront;
        class Confidence;
        class DataTerm;

        class DummyPriority : public ettention::inpainting::ComputeOrder
        {
        public:
            DummyPriority( ByteVolume* mask );
            virtual ~DummyPriority();
            virtual Vec3ui selectCenterOfPatchToProcess( bool pop );
            virtual float computePriorityForVoxel(Vec3ui coordinate);
			virtual void setIndex(Index* index);
        };

    } // namespace inpainting
} // namespace ettention
