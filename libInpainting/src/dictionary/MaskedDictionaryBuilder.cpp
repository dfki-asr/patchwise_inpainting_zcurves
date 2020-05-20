#include "stdafx.h"

#include "MaskedDictionaryBuilder.h"
#include "Dictionary.h"

#include "StatusFlags.h"
#include "Problem.h"

#include "index/PatchAccess8Bit.h"
#include "index/DimensionSelection.h"

namespace ettention
{
    namespace inpainting {

		MaskedDictionaryBuilder::MaskedDictionaryBuilder(Problem* problem)
			: FullDictionaryBuilder(problem), dictionaryMask(problem->dictionaryMask)
        {
			patchAccess = new BytePatchAccess8Bit( problem->dictionaryMask, problem->patchSize, DimensionSelection::standardPermutation( problem->patchSize ) );
        }

		MaskedDictionaryBuilder::MaskedDictionaryBuilder( ByteVolume* dictionaryVolume, Vec3ui patchSize)
            : FullDictionaryBuilder( dictionaryVolume, patchSize )
        {
        }

		MaskedDictionaryBuilder::~MaskedDictionaryBuilder()
        {
			delete patchAccess;
        }

        Dictionary* MaskedDictionaryBuilder::createDictionary( )
        {
            Dictionary* dictionary = new Dictionary( dictionaryVolume->getProperties().getVolumeResolution(), patchSize);
            dictionary->getFullDictionary() = extractValidPatchPositions();
            return dictionary;
        }

        std::vector<ettention::Vec3ui> MaskedDictionaryBuilder::extractValidPatchPositions()
        {
            Vec3ui patchCenterCoord;
            Vec3ui volumeRes = Vec3ui(0, 0, 0);
            size_t voxelCount = 0;

            volumeRes = dictionaryVolume->getProperties().getVolumeResolution();
            voxelCount = dictionaryVolume->getProperties().getVolumeVoxelCount();

            std::vector<ettention::Vec3ui> result;
            result.reserve(voxelCount);

            bool lastPatchWasValid = false;
            for (patchCenterCoord.z = patchSize.z / 2; patchCenterCoord.z < volumeRes.z - patchSize.z / 2; patchCenterCoord.z++)
            {
                for (patchCenterCoord.y = patchSize.y / 2; patchCenterCoord.y < volumeRes.y - patchSize.y / 2; patchCenterCoord.y++)
                {
                    for (patchCenterCoord.x = patchSize.x / 2; patchCenterCoord.x < volumeRes.x - patchSize.x / 2; patchCenterCoord.x++)
                    {
						if ( isPatchPositionValid(patchCenterCoord) )
							result.push_back(patchCenterCoord);
                    }
                }
            }
            result.shrink_to_fit();
            if (result.size() == 0)
                throw std::runtime_error("building full dictionary failed, check input");
            return result;
        }

		bool MaskedDictionaryBuilder::isPatchPositionValid(Vec3ui patchCenterCoord)
		{
			unsigned int patchID = Flatten3D( patchCenterCoord, dictionaryMask->getProperties().getVolumeResolution() );
			patchAccess->setPatchId( patchID );
			for (unsigned int i = 0; i < (unsigned int) patchAccess->size(); i++)
				if ( patchAccess->operator[](i) != SOURCE_REGION )
					return false;
			return true;
		}

	}
}