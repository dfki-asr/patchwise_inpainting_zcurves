#include "stdafx.h"

#include "FullDictionaryBuilder.h"
#include "Dictionary.h"

#include "libmmv/model/volume/ByteVolume.h"

#include "StatusFlags.h"
#include "Problem.h"
#include "libmmv/algorithm/Coordinates.h"

namespace inpainting {

    FullDictionaryBuilder::FullDictionaryBuilder( Problem* problem)
        : dictionaryVolume(problem->dictionaryVolume), patchSize( problem->patchSize )
    {
    }

    FullDictionaryBuilder::FullDictionaryBuilder( libmmv::ByteVolume* dictionaryVolume, libmmv::Vec3ui patchSize)
        : dictionaryVolume(dictionaryVolume), patchSize(patchSize)
    {
    }

    FullDictionaryBuilder::~FullDictionaryBuilder()
    {
    }

    Dictionary* FullDictionaryBuilder::createDictionary( )
    {
        Dictionary* dictionary = new Dictionary( dictionaryVolume->getProperties().getVolumeResolution(), patchSize);
        dictionary->getFullDictionary() = extractValidPatchPositions();
        return dictionary;
    }

	std::vector<libmmv::Vec3ui> FullDictionaryBuilder::extractValidPatchPositions()
    {
        libmmv::Vec3ui patchCenterCoord;

        libmmv::Vec3ui volumeRes = dictionaryVolume->getProperties().getVolumeResolution();
		size_t voxelCount = dictionaryVolume->getProperties().getVolumeVoxelCount();

        std::vector<libmmv::Vec3ui> result;
        result.reserve(voxelCount);

        for (patchCenterCoord.z = patchSize.z / 2; patchCenterCoord.z < volumeRes.z - patchSize.z / 2; patchCenterCoord.z++)
        {
            for (patchCenterCoord.y = patchSize.y / 2; patchCenterCoord.y < volumeRes.y - patchSize.y / 2; patchCenterCoord.y++)
            {
                for (patchCenterCoord.x = patchSize.x / 2; patchCenterCoord.x < volumeRes.x - patchSize.x / 2; patchCenterCoord.x++)
                {
                    result.push_back(patchCenterCoord);
                }
            }
        }
        result.shrink_to_fit();
        if (result.size() == 0)
            throw std::runtime_error("building full dictionary failed, check input");
        return result;
    }

	std::vector<unsigned int> FullDictionaryBuilder::extractValidPatchIndices()
	{
        libmmv::Vec3ui volumeRes = dictionaryVolume->getProperties().getVolumeResolution();
		size_t voxelCount = dictionaryVolume->getProperties().getVolumeVoxelCount();

		std::vector<unsigned int> result;
		result.reserve(voxelCount);

        libmmv::Vec3ui patchCenterCoord;
		for (patchCenterCoord.z = patchSize.z / 2; patchCenterCoord.z < volumeRes.z - patchSize.z / 2; patchCenterCoord.z++)
		{
			for (patchCenterCoord.y = patchSize.y / 2; patchCenterCoord.y < volumeRes.y - patchSize.y / 2; patchCenterCoord.y++)
			{
				for (patchCenterCoord.x = patchSize.x / 2; patchCenterCoord.x < volumeRes.x - patchSize.x / 2; patchCenterCoord.x++)
				{
					result.push_back( Flatten3D( patchCenterCoord, volumeRes ) );
				}
			}
		}
		result.shrink_to_fit();
		if (result.size() == 0)
			throw std::runtime_error("building full dictionary failed, check input");
		return result;
	}

}