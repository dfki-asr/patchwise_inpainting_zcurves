#pragma once

#include "FullDictionaryBuilder.h"
#include "index/PatchAccess8Bit.h"

class ByteVolume;

namespace inpainting 
{

    class Problem;
    class Dictionary;

    class MaskedDictionaryBuilder : public FullDictionaryBuilder 
    {
    public:
		MaskedDictionaryBuilder( Problem* problem );
		MaskedDictionaryBuilder( libmmv::ByteVolume* dictionaryVolume, libmmv::Vec3ui patchSize );
        ~MaskedDictionaryBuilder();

        virtual Dictionary* createDictionary( );
		virtual std::vector<libmmv::Vec3ui> extractValidPatchPositions();
		bool isPatchPositionValid(libmmv::Vec3ui patchCenterCoord );

    protected:
        libmmv::ByteVolume* dictionaryMask;
		BytePatchAccess8Bit* patchAccess;
    };
}

