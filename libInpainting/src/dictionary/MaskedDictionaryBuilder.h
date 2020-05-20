#pragma once

#include "FullDictionaryBuilder.h"
#include "index/PatchAccess8Bit.h"

namespace ettention 
{

    class ByteVolume;

    namespace inpainting 
    {

        class Problem;
        class Dictionary;

        class MaskedDictionaryBuilder : public FullDictionaryBuilder 
        {
        public:
			MaskedDictionaryBuilder( Problem* problem );
			MaskedDictionaryBuilder( ByteVolume* dictionaryVolume, Vec3ui patchSize );
            ~MaskedDictionaryBuilder();

            virtual Dictionary* createDictionary( );
			virtual std::vector<Vec3ui> extractValidPatchPositions();
			bool isPatchPositionValid( Vec3ui patchCenterCoord );

        protected:
            ByteVolume* dictionaryMask;
			BytePatchAccess8Bit* patchAccess;
        };
    }
}
