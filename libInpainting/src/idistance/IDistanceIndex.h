#pragma once

#include "index/MaskedIndex.h"
#include "index/FilterAndRefine.h"
#include "index/PatchAccess8Bit.h"
#include "costfunction/L2Distance.h"
#include "pca/PCASubspace.h"

class IDistance;

namespace ettention
{
	class ByteVolume;
	
	namespace inpainting 
	{
		class IDistanceIndex : public MaskedIndex
		{
		public:
			IDistanceIndex( ByteVolume* data, ByteVolume* mask, ByteVolume* dictionary, std::vector<unsigned int>& patchIds, Vec3ui patchSize, DimensionSelection::PreferredDirection preferredDirection, int unprojectedSize, int subspaceDimensions, int filterSize );
			virtual  ~IDistanceIndex();

			virtual std::tuple<Vec3i, float> query( Vec3i targetPatchCenter ) override;
			virtual std::string to_string() override;

		protected:
			std::vector<float> getDictionaryPointSetInPrincipalSpace( PCASubspace<float>* subspace, std::vector<unsigned int>& patchIds );

		public:
			virtual void writeToStream(std::ostream& os) override;
			virtual void loadFromStream(std::istream& is) override;

		protected:
			DimensionSelection::PreferredDirection preferredDirection;
			ByteVolume* data;
			ByteVolume* dictionary;
			PCASubspace<float> *subspace;
			IDistance *iDistance;
			FilterAndRefine* filterAndRefine;
			int subspaceDimensions;
			int filterSize;
		};
    }
}