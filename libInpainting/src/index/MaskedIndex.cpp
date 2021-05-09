#include "stdafx.h"

#include "MaskedIndex.h"
#include "StatusFlags.h"

namespace inpainting
{

	MaskedIndex::MaskedIndex(libmmv::ByteVolume* mask )
		: mask(mask)
	{
	}

	MaskedIndex::MaskedIndex(libmmv::ByteVolume* mask, libmmv::Vec3ui patchSize, DimensionSelection::PreferredDirection preferredDirection, int unprojectedSize )
		: mask(mask), patchSize(patchSize), preferredDirection(preferredDirection)
	{
		permutation = DimensionSelection::customPermutation(patchSize, preferredDirection, unprojectedSize);
		initMaskAccess();
	}

	MaskedIndex::~MaskedIndex()
	{
		delete maskAccess;
	}

	void MaskedIndex::initMaskAccess()
	{
		maskAccess = new BytePatchAccess8Bit(mask, patchSize, permutation);
	}

	int MaskedIndex::rateQualityOfIndexForQuery(libmmv::Vec3i targetPatchCenter)
	{
		if ( !patchEntirelyInVolume(targetPatchCenter) )
			return -1;

		maskAccess->setPatchId( targetPatchCenter);
		for (unsigned int i = 0; i < maskAccess->size(); i++)
		{
			const unsigned char status = maskAccess->operator[]( i );
			if (status == EMPTY_REGION || status == TARGET_REGION)
				return -1;
		}
		return (int) maskAccess->size();
	}

	libmmv::Vec3i MaskedIndex::adjustTargetPatchPosition(libmmv::Vec3i targetPatchCenter )
	{
		return targetPatchCenter;
	}

	std::tuple<libmmv::Vec3i, float> MaskedIndex::query(libmmv::Vec3i targetPatchCenter)
	{
		throw std::runtime_error("implemented for testing only, use a subclass for actual performing a query");
	}

	std::string MaskedIndex::to_string()
	{
		return "masked index";
	}

	std::vector<unsigned int>& MaskedIndex::getPermutation()
	{
		return permutation;
	}

	bool MaskedIndex::patchEntirelyInVolume(libmmv::Vec3i targetPatchCenter)
	{
		libmmv::Vec3i lowerBound = patchSize / 2;
		libmmv::Vec3i upperBound = mask->getProperties().getVolumeResolution() - (patchSize / 2) - libmmv::Vec3i(1, 1, 1);
		libmmv::Vec3i correctedTargetPatchCenter = clamp( targetPatchCenter, lowerBound, upperBound );
		return ( correctedTargetPatchCenter == targetPatchCenter );
	}

} // namespace

