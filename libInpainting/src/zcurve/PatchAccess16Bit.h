#pragma once

#include <vector>

#include "math/Vec3.h"

namespace ettention
{
    class FloatVolume;

	namespace inpainting 
	{

        class PatchAccess16Bit
        {
        public:
			typedef unsigned short value_type;

            PatchAccess16Bit(FloatVolume* volume, Vec3ui patchSize, unsigned int patchID, float minValue, float maxValue );
            unsigned short operator[](size_t byteIndex) const;
            size_t size() const;

        protected:
            FloatVolume* volume;
            Vec3ui patchSize;
            unsigned int patchID;
            float minValue;
            float maxValue;
        };

	} // namespace inpainting
} // namespace ettention