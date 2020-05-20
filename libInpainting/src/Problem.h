#pragma once

#include "math/Vec3.h"

namespace ettention
{
    class FloatVolume;
    class ByteVolume;

    namespace inpainting 
    {

        class Problem
        {
        public:
            Problem();
            virtual ~Problem();

			ByteVolume* data;
            ByteVolume* mask;
			ByteVolume* denseScan;
			ByteVolume* dictionaryVolume;
			ByteVolume* dictionaryMask;
            Vec3i patchSize;
        };

    } // namespace inpainting
} // namespace ettention