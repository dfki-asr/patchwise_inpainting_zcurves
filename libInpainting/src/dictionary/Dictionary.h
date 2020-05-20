#pragma once

#include "math/Vec3.h"

namespace ettention
{
    namespace inpainting
    {
        /* The purpose of the dictionary is to store the valid patch positions of a volume. Hereby, the
           dictionary can map from a a 3D (x/y/z) address (called full dictionary) in the volume to a flat 
           1D address (called compressed dictionary). The purpose of this compresion/uncompression 
           mechanism is to save memory. */
        class Dictionary
        {
        public:
            Dictionary( Vec3ui volumeResolution, Vec3ui patchSize);
            Dictionary( Vec3ui volumeResolution, Vec3ui patchSize, std::vector<unsigned int> compressedDictionary );
            ~Dictionary();

            std::vector<unsigned int>& getCompressedDictionary();
            std::vector<Vec3ui>& getFullDictionary();

            static std::vector<unsigned int> flatten( const std::vector<Vec3ui>& uncompressed, Vec3ui resolution );

            Vec3ui getCoordinateOfEntry(size_t index);
            void compressDictionary();
            void decompressDictionary();

        protected:
            Vec3ui patchSize;
            std::vector<unsigned int> compressedDictionary;
            std::vector<Vec3ui> fullDictionary;
            Vec3ui volumeResolution;
        };
    }
}
