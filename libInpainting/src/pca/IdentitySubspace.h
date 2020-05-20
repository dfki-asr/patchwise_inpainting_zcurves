#pragma once

#include "math/Vec3.h"

namespace ettention
{
	class ByteVolume;

	namespace inpainting 
	{	
		template<class TDATA>
	    class IdentitySubspace
        {
        public:
			IdentitySubspace( ByteVolume* data, ByteVolume* dictionary, Vec3ui patchSize, std::vector<unsigned int>& permutation, std::vector<unsigned int>& patchIndices, unsigned int subspaceDimensions );
			~IdentitySubspace();

			std::vector<TDATA> getDataPointInPrincipalSpace( unsigned int );
			std::vector<TDATA> getDataPointInPrincipalSpace( Vec3ui patchCenter );
			std::vector<TDATA> getDictionaryPointInPrincipalSpace(unsigned int);

        protected:
			ByteVolume* data;
			BytePatchAccess8Bit dictionaryAccess;
			BytePatchAccess8Bit dataAccess;
			std::vector<unsigned int> permutation;
			std::vector<unsigned int>& patchIndices;
		};
	} // namespace inpainting
} // namespace ettention`