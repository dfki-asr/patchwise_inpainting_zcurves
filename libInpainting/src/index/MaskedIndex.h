#pragma once

#include "index/Index.h"
#include "index/PatchAccess8Bit.h"

class IDistance;

namespace libmmv
{
	class ByteVolume;
}
	
namespace inpainting 
{
	class MaskedIndex : public Index
	{
	public:
		MaskedIndex( libmmv::ByteVolume* mask );
		MaskedIndex( libmmv::ByteVolume* mask, libmmv::Vec3ui patchSize, DimensionSelection::PreferredDirection preferredDirection, int unprojectedSize);
		virtual ~MaskedIndex();

		void initMaskAccess();

		virtual int rateQualityOfIndexForQuery( libmmv::Vec3i targetPatchCenter ) override;
		virtual libmmv::Vec3i adjustTargetPatchPosition( libmmv::Vec3i targetPatchCenter ) override;

		virtual std::tuple<libmmv::Vec3i, float> query(libmmv::Vec3i targetPatchCenter ) override;
		virtual std::string to_string() override;
			
		// public for testing
		std::vector<unsigned int>& getPermutation();

	protected:
		bool patchEntirelyInVolume(libmmv::Vec3i targetPatchCenter );

	protected:
		BytePatchAccess8Bit* maskAccess;
		std::vector<unsigned int> permutation;
		DimensionSelection::PreferredDirection preferredDirection;
		libmmv::ByteVolume* mask;
		libmmv::Vec3ui patchSize;
	};
}
