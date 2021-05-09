#pragma once

#include "libmmv/math/Vec3.h"

namespace libmmv 
{
	class ByteVolume;
}

namespace inpainting 
{	
	template<class TDATA>
	class IdentitySubspace
    {
    public:
		IdentitySubspace(libmmv::ByteVolume* data, libmmv::ByteVolume* dictionary, libmmv::Vec3ui patchSize, std::vector<unsigned int>& permutation, std::vector<unsigned int>& patchIndices, unsigned int subspaceDimensions );
		~IdentitySubspace();

		std::vector<TDATA> getDataPointInPrincipalSpace( unsigned int );
		std::vector<TDATA> getDataPointInPrincipalSpace(libmmv::Vec3ui patchCenter );
		std::vector<TDATA> getDictionaryPointInPrincipalSpace(unsigned int);

    protected:
		libmmv::ByteVolume* data;
		BytePatchAccess8Bit dictionaryAccess;
		BytePatchAccess8Bit dataAccess;
		std::vector<unsigned int> permutation;
		std::vector<unsigned int>& patchIndices;
	};
} // namespace inpainting
