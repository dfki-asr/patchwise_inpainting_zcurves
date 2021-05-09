#pragma once

#include "libmmv/math/Vec3.h"

namespace inpainting
{
    /* The purpose of the dictionary is to store the valid patch positions of a volume. Hereby, the
        dictionary can map from a a 3D (x/y/z) address (called full dictionary) in the volume to a flat 
        1D address (called compressed dictionary). The purpose of this compresion/uncompression 
        mechanism is to save memory. */
    class Dictionary
    {
    public:
        Dictionary( libmmv::Vec3ui volumeResolution, libmmv::Vec3ui patchSize);
        Dictionary(libmmv::Vec3ui volumeResolution, libmmv::Vec3ui patchSize, std::vector<unsigned int> compressedDictionary );
        ~Dictionary();

        std::vector<unsigned int>& getCompressedDictionary();
        std::vector<libmmv::Vec3ui>& getFullDictionary();

        static std::vector<unsigned int> flatten( const std::vector<libmmv::Vec3ui>& uncompressed, libmmv::Vec3ui resolution );

        libmmv::Vec3ui getCoordinateOfEntry(size_t index);
        void compressDictionary();
        void decompressDictionary();

    protected:
        libmmv::Vec3ui patchSize;
        std::vector<unsigned int> compressedDictionary;
        std::vector<libmmv::Vec3ui> fullDictionary;
        libmmv::Vec3ui volumeResolution;
    };
}
