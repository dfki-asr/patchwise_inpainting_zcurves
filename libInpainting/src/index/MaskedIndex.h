#pragma once

#include "index/Index.h"
#include "index/PatchAccess8Bit.h"

class IDistance;

namespace ettention
{
	class ByteVolume;
	
	namespace inpainting 
	{
		class MaskedIndex : public Index
		{
		public:
			MaskedIndex( ByteVolume* mask );
			MaskedIndex( ByteVolume* mask, Vec3ui patchSize, DimensionSelection::PreferredDirection preferredDirection, int unprojectedSize);
			virtual ~MaskedIndex();

			void initMaskAccess();

			virtual int rateQualityOfIndexForQuery( Vec3i targetPatchCenter ) override;
			virtual Vec3i MaskedIndex::adjustTargetPatchPosition( Vec3i targetPatchCenter ) override;

			virtual std::tuple<Vec3i, float> query(Vec3i targetPatchCenter ) override;
			virtual std::string to_string() override;
			
			// public for testing
			std::vector<unsigned int>& getPermutation();

		protected:
			bool patchEntirelyInVolume( Vec3i targetPatchCenter );

		protected:
			BytePatchAccess8Bit* maskAccess;
			std::vector<unsigned int> permutation;
			DimensionSelection::PreferredDirection preferredDirection;
			ByteVolume* mask;
			Vec3ui patchSize;
		};
    }
}