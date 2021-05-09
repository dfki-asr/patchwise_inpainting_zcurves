#pragma once

#ifdef _DEBUG
#undef new
#endif 

#include <daal.h>

#include "libmmv/math/Vec3.h"
#include "index/PatchAccess8Bit.h"
#include "io/Serializable.h"

namespace libmmv
{
	class ByteVolume;
}

namespace inpainting 
{	
	typedef daal::data_management::HomogenNumericTable<float> HNT;
	typedef daal::services::SharedPtr<HNT> SharedPtrHNT;

	template<class TDATA>
	class PCASubspace : public Serializable
    {
    public:
		PCASubspace( libmmv::ByteVolume* data, libmmv::ByteVolume* dictionary, libmmv::Vec3ui patchSize, std::vector<unsigned int>& permutation, std::vector<unsigned int>& patchKeys, unsigned int subspaceDimensions);
		PCASubspace( std::istream& is, libmmv::ByteVolume* data );
		~PCASubspace();

		std::vector<TDATA> getDataPointInPrincipalSpace( unsigned int );
		std::vector<TDATA> getDataPointInPrincipalSpace(libmmv::Vec3ui patchCenter );
		std::vector<TDATA> getDictionaryPointInPrincipalSpace( unsigned int patchKey );

	protected:
		SharedPtrHNT loadVariableBlock( const std::vector<unsigned int>& patchKeys, size_t i, size_t size );
		void transferDictionaryPointToUnscaledPrincipalSpace( unsigned int patchKey, std::vector<float>& buffer );
		void extractEigenvectors( daal::algorithms::pca::Online<float>& pca_algorithm );
		void determineDataRange( const std::vector<unsigned int>& patchKeys );
		void normalizeDataRange( std::vector<float>& data );

	public:
		virtual void writeToStream( std::ostream& os ) override;
		virtual void loadFromStream( std::istream& is ) override;

    protected:
		libmmv::ByteVolume* data;
		BytePatchAccess8Bit dictionaryAccess;
		BytePatchAccess8Bit dataAccess;
		std::vector<unsigned int> permutation;
		unsigned int subspaceDimensions;
		float minValue, maxValue; 
		std::vector<float> eigvecs;
		libmmv::Vec3ui patchSize;

		SharedPtrHNT tablePtr;
	};

} // namespace inpainting
