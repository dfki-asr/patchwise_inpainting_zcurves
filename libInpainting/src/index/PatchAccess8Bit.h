#pragma once

#include "libmmv/math/Vec3.h"

#include <libmmv/model/volume/FloatVolume.h>
#include <libmmv/model/volume/ByteVolume.h>

namespace inpainting 
{
	template<class TVOLUME>
    class PatchAccess8Bit
    {
    public:
		typedef unsigned char value_type;

		PatchAccess8Bit();
		PatchAccess8Bit(TVOLUME* volume, libmmv::Vec3ui patchSize, std::vector<unsigned int> permutation);

		inline unsigned char operator[](unsigned int byteIndex) const
		{
			const size_t flatVoxelPosition = flatBaseAddress + offsetLookup[byteIndex];
			return (unsigned char) volume->nativeVoxelValue(flatVoxelPosition);
		}

        size_t size() const;
		void setPatchId(const unsigned int& patchID);
		void setPatchId(const libmmv::Vec3ui patchCenter );
		libmmv::Vec3ui getPatchCenter();
		void initOffsetLookup();
		libmmv::Vec3ui getPositionInVolume(unsigned int byteIndex);

    protected:
		TVOLUME* volume;
		libmmv::Vec3ui patchSize;
        unsigned int patchID;
		std::vector<unsigned int> offsetLookup;
		std::vector<unsigned int> permutation;
		unsigned int flatOffsetCorrespondingToPatchSizeHalf;
		unsigned int flatBaseAddress;
    };

	typedef PatchAccess8Bit<libmmv::FloatVolume> FloatPatchAccess8Bit;
	typedef PatchAccess8Bit<libmmv::ByteVolume> BytePatchAccess8Bit;

	std::ostream& operator<<(std::ostream& ofs, const FloatPatchAccess8Bit& access);
	std::ostream& operator<<(std::ostream& ofs, const BytePatchAccess8Bit& access);

} // namespace inpainting
