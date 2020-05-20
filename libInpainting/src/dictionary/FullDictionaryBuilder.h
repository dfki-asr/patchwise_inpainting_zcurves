#pragma once

#include "math/Vec3.h"

namespace ettention 
{

    class ByteVolume;

    namespace inpainting 
    {

        class Problem;
        class Dictionary;

        class FullDictionaryBuilder 
        {
        public:
            FullDictionaryBuilder( Problem* problem );
            FullDictionaryBuilder( ByteVolume* dictionaryVolume, Vec3ui patchSize );
            ~FullDictionaryBuilder();

            virtual Dictionary* createDictionary( );
			virtual std::vector<Vec3ui> extractValidPatchPositions();
			virtual std::vector<unsigned int> extractValidPatchIndices();

        protected:
			ByteVolume* dictionaryVolume;
            Vec3ui patchSize;
        };
    }
}
