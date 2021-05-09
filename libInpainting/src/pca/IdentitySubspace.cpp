#include "stdafx.h"

#include "index/PatchAccess8Bit.h"
#include "libmmv/algorithm/Coordinates.h"

#include "IdentitySubspace.h"

namespace inpainting 
{ 

	template<class TDATA>
	IdentitySubspace<TDATA>::IdentitySubspace(libmmv::ByteVolume* data, libmmv::ByteVolume* dictionary, libmmv::Vec3ui patchSize, std::vector<unsigned int>& permutation, std::vector<unsigned int>& patchIndices, unsigned int subspaceDimensions )
		: dictionaryAccess(dictionary, patchSize, permutation ), data(data), dataAccess(data, patchSize, permutation), permutation( permutation ), patchIndices( patchIndices )
	{
	}

	template<class TDATA>
	IdentitySubspace<TDATA>::~IdentitySubspace()
	{
	}

	template<class TDATA>
	std::vector<TDATA> IdentitySubspace<TDATA>::getDataPointInPrincipalSpace(unsigned int patchID)
	{
		dataAccess.setPatchId(patchID);
		std::vector<float> pointInVectorSpace(permutation.size());
		for ( unsigned int dimension = 0; dimension < (unsigned int) permutation.size(); dimension++ )
		{
			pointInVectorSpace[dimension] = (float)dataAccess[dimension];
		}
		std::vector<TDATA> castResult(pointInVectorSpace.size() );
		for (size_t i = 0; i < pointInVectorSpace.size(); i++)
			castResult[i] = (TDATA) pointInVectorSpace[i];
		return castResult;
	}

	template<class TDATA>
	std::vector<TDATA> IdentitySubspace<TDATA>::getDataPointInPrincipalSpace(libmmv::Vec3ui patchCenter)
	{
		unsigned int patchId = Flatten3D( patchCenter, data->getProperties().getVolumeResolution() );
		return getDataPointInPrincipalSpace(patchId);
	}

	template<class TDATA>
	std::vector<TDATA> IdentitySubspace<TDATA>::getDictionaryPointInPrincipalSpace(unsigned int patchID)
	{
		dictionaryAccess.setPatchId(patchID);
		std::vector<float> pointInVectorSpace(permutation.size());
		for (unsigned int dimension = 0; dimension < (unsigned int)permutation.size(); dimension++)
		{
			pointInVectorSpace[dimension] = (float) dictionaryAccess[dimension];
		}
		std::vector<TDATA> castResult(pointInVectorSpace.size());
		for (size_t i = 0; i < pointInVectorSpace.size(); i++)
			castResult[i] = (TDATA)pointInVectorSpace[i];
		return castResult;
	}

	template class IdentitySubspace<float>;
	template class IdentitySubspace<unsigned char>;

} // namespace inpainting