#include "stdafx.h"

#include "Dictionary.h"

#include "algorithm/Coordinates.h"

namespace ettention
{
    namespace inpainting 
    {

        Dictionary::Dictionary( Vec3ui volumeResolution, Vec3ui patchSize)
            : volumeResolution(volumeResolution), patchSize(patchSize)
        {
        }

        Dictionary::Dictionary( Vec3ui volumeResolution, Vec3ui patchSize, std::vector<unsigned int> dictionaryPatches )
            : volumeResolution(volumeResolution)
            , patchSize(patchSize)
            , compressedDictionary( dictionaryPatches.size() )
        {
            compressedDictionary = dictionaryPatches;
        }

        Dictionary::~Dictionary()
        {
        }

        std::vector<unsigned int>& Dictionary::getCompressedDictionary()
        {
            return compressedDictionary;
        }

        std::vector<Vec3ui>& Dictionary::getFullDictionary()
        {
            return fullDictionary;
        }

        std::vector<unsigned int> Dictionary::flatten(const std::vector<Vec3ui>& uncompressed, Vec3ui resolution )
        {
            std::vector<unsigned int> compressed( uncompressed.size() );
            size_t i = 0;
            for (i = 0; i < compressed.size(); ++i)
            {
                compressed[i] = Flatten3D( uncompressed[i], resolution);
            }
            return compressed;
        }

        ettention::Vec3ui Dictionary::getCoordinateOfEntry(size_t index)
        {
            return fullDictionary[index];
        }

        void Dictionary::compressDictionary()
        {
            if( !fullDictionary.size() )
                return;

            compressedDictionary = flatten( fullDictionary, volumeResolution );
        }

        void Dictionary::decompressDictionary()
        {
            if( compressedDictionary.size() == 0 )
                return;

            fullDictionary.resize( compressedDictionary.size() );
            for( size_t i = 0; i < fullDictionary.size(); ++i )
            {
                fullDictionary[i] = Unflatten3D( compressedDictionary[i], volumeResolution );
            }
        }        
    }
}