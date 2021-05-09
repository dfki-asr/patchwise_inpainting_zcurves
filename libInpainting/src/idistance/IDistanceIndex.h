#pragma once

#include "index/MaskedIndex.h"
#include "index/FilterAndRefine.h"
#include "index/PatchAccess8Bit.h"
#include "costfunction/L2Distance.h"
#include "pca/PCASubspace.h"

class IDistance;

namespace libmmv
{
	class ByteVolume;
}
	
namespace inpainting 
{
	class IDistanceIndex : public MaskedIndex
	{
	public:
		IDistanceIndex(libmmv::ByteVolume* data, libmmv::ByteVolume* mask, libmmv::ByteVolume* dictionary, std::vector<unsigned int>& patchIds, libmmv::Vec3ui patchSize, DimensionSelection::PreferredDirection preferredDirection, int unprojectedSize, int subspaceDimensions, int filterSize );
		virtual  ~IDistanceIndex();

		virtual std::tuple<libmmv::Vec3i, float> query(libmmv::Vec3i targetPatchCenter ) override;
		virtual std::string to_string() override;

	protected:
		std::vector<float> getDictionaryPointSetInPrincipalSpace( PCASubspace<float>* subspace, std::vector<unsigned int>& patchIds );

	public:
		virtual void writeToStream(std::ostream& os) override;
		virtual void loadFromStream(std::istream& is) override;

	protected:
		DimensionSelection::PreferredDirection preferredDirection;
		libmmv::ByteVolume* data;
		libmmv::ByteVolume* dictionary;
		PCASubspace<float> *subspace;
		IDistance *iDistance;
		FilterAndRefine* filterAndRefine;
		int subspaceDimensions;
		int filterSize;
	};
}