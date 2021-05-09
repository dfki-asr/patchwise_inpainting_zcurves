#include "stdafx.h"

#include "PatchAccess8Bit.h"
#include "NDPointPair.h"

#include "libmmv/algorithm/Coordinates.h"

namespace inpainting 
{ 
	template<class TVOLUME>
	PatchAccess8Bit<TVOLUME>::PatchAccess8Bit()
	{
	}

	template<class TVOLUME>
    PatchAccess8Bit<TVOLUME>::PatchAccess8Bit(TVOLUME* volume, libmmv::Vec3ui patchSize, std::vector<unsigned int> permutation)
        : volume(volume), patchSize(patchSize), patchID(patchID), permutation( permutation )
    {
		initOffsetLookup();

		const libmmv::Vec3ui volumeResolution = volume->getProperties().getVolumeResolution();
		flatOffsetCorrespondingToPatchSizeHalf = Flatten3D(patchSize / 2, volumeResolution);
    }

	template<class TVOLUME>
	size_t PatchAccess8Bit<TVOLUME>::size() const
    {
        return permutation.size();
    } 

	template<class TVOLUME>
	void PatchAccess8Bit<TVOLUME>::setPatchId(const unsigned int& patchID)
    {
        this->patchID = patchID;
		flatBaseAddress = patchID - flatOffsetCorrespondingToPatchSizeHalf;
    }

	template<class TVOLUME>
	void PatchAccess8Bit<TVOLUME>::setPatchId(const libmmv::Vec3ui patchCenter)
	{
		unsigned int index = Flatten3D(patchCenter, volume->getProperties().getVolumeResolution());
		setPatchId(index);
	}

	template<class TVOLUME>
	libmmv::Vec3ui PatchAccess8Bit<TVOLUME>::getPatchCenter()
	{
		return Unflatten3D(patchID, volume->getProperties().getVolumeResolution());
	}

	template<class TVOLUME>
	void PatchAccess8Bit<TVOLUME>::initOffsetLookup()
    {
        offsetLookup.resize( size() );
		libmmv::Vec3ui volumeResolution = volume->getProperties().getVolumeResolution();

        // init lookup table
        for (unsigned int dimensionIndex = 0; dimensionIndex < (unsigned int)size(); dimensionIndex++)
        {
			unsigned int permutedIndex = permutation[ dimensionIndex ];
			libmmv::Vec3ui offset = Unflatten3D(permutedIndex, patchSize);
            unsigned int flatVoxelOffset = Flatten3D(offset, volumeResolution);
            offsetLookup[dimensionIndex] = flatVoxelOffset;
        }
    }

	template<class TVOLUME>
	libmmv::Vec3ui PatchAccess8Bit<TVOLUME>::getPositionInVolume(unsigned int byteIndex)
	{
		const libmmv::Vec3ui volumeResolution = volume->getProperties().getVolumeResolution();
		const size_t flatVoxelPosition = flatBaseAddress + offsetLookup[byteIndex];
		return Unflatten3D(flatVoxelPosition, volumeResolution);
	}

	std::ostream& operator<<(std::ostream& ofs, const FloatPatchAccess8Bit& access)
	{
		for (unsigned int i = 0; i < (unsigned int)access.size(); i++)
		{
			ofs << (int)access[i];
			if (i < access.size() - 1)
				ofs << "/";
		}
		return ofs;
	}

	std::ostream& operator<<(std::ostream& ofs, const BytePatchAccess8Bit& access)
	{
		for (unsigned int i = 0; i < (unsigned int)access.size(); i++)
		{
			ofs << (int)access[i];
			if (i < access.size() - 1)
				ofs << "/";
		}
		return ofs;
	}

	template class PatchAccess8Bit<libmmv::ByteVolume>;
	template class PatchAccess8Bit<libmmv::FloatVolume>;

} // namespace inpainting
