#pragma once

#include "math/Vec3.h"

#include <model/volume/FloatVolume.h>
#include <model/volume/ByteVolume.h>

namespace ettention
{
	namespace inpainting 
	{
		template<class TVOLUME>
        class PatchAccess8Bit
        {
        public:
			typedef unsigned char value_type;

			PatchAccess8Bit();
			PatchAccess8Bit(TVOLUME* volume, Vec3ui patchSize, std::vector<unsigned int> permutation);

			inline unsigned char operator[](unsigned int byteIndex) const
			{
				const size_t flatVoxelPosition = flatBaseAddress + offsetLookup[byteIndex];
				return (unsigned char) volume->nativeVoxelValue(flatVoxelPosition);
			}

            size_t size() const;
			void setPatchId(const unsigned int& patchID);
			void setPatchId(const Vec3ui patchCenter );
			Vec3ui getPatchCenter();
			void initOffsetLookup();

        protected:
			TVOLUME* volume;
            Vec3ui patchSize;
            unsigned int patchID;
			std::vector<unsigned int> offsetLookup;
			std::vector<unsigned int> permutation;
			unsigned int flatOffsetCorrespondingToPatchSizeHalf;
			unsigned int flatBaseAddress;
        };

		typedef PatchAccess8Bit<FloatVolume> FloatPatchAccess8Bit;
		typedef PatchAccess8Bit<ByteVolume> BytePatchAccess8Bit;

		std::ostream& operator<<(std::ostream& ofs, const FloatPatchAccess8Bit& access);
		std::ostream& operator<<(std::ostream& ofs, const BytePatchAccess8Bit& access);

	} // namespace inpainting
} // namespace ettention