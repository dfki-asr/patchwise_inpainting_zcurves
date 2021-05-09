#pragma once

#include "libmmv/math/Vec3.h"

namespace libmmv
{
    class FloatVolume;
    class ByteVolume;
}

namespace inpainting 
{

    class Problem
    {
    public:
        Problem();
        virtual ~Problem();

        libmmv::ByteVolume* data;
        libmmv::ByteVolume* mask;
        libmmv::ByteVolume* denseScan;
        libmmv::ByteVolume* dictionaryVolume;
        libmmv::ByteVolume* dictionaryMask;
        libmmv::Vec3i patchSize;
        std::vector<float> costWeight;
    };

} // namespace inpainting
