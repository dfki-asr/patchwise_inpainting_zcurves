#pragma once

#include "libmmv/math/Vec3.h"

namespace libmmv 
{
    class ByteVolume;
}

namespace inpainting 
{

    class Problem;
    class Dictionary;

    class FullDictionaryBuilder 
    {
    public:
        FullDictionaryBuilder( Problem* problem );
        FullDictionaryBuilder(libmmv::ByteVolume* dictionaryVolume, libmmv::Vec3ui patchSize );
        ~FullDictionaryBuilder();

        virtual Dictionary* createDictionary( );
		virtual std::vector<libmmv::Vec3ui> extractValidPatchPositions();
		virtual std::vector<unsigned int> extractValidPatchIndices();

    protected:
        libmmv::ByteVolume* dictionaryVolume;
        libmmv::Vec3ui patchSize;
    };
}
