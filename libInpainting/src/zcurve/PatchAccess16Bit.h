#pragma once

#include <vector>

#include "math/Vec3.h"

namespace libmmv
{
    class FloatVolume;
}

namespace inpainting 
{

    class PatchAccess16Bit
    {
    public:
		typedef unsigned short value_type;

        PatchAccess16Bit(libmmv::FloatVolume* volume, libmmv::Vec3ui patchSize, unsigned int patchID, float minValue, float maxValue );
        unsigned short operator[](size_t byteIndex) const;
        size_t size() const;

    protected:
        libmmv::FloatVolume* volume;
        libmmv::Vec3ui patchSize;
        unsigned int patchID;
        float minValue;
        float maxValue;
    };

} // namespace inpainting
