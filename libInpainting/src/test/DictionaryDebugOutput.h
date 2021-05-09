#pragma once

#include <vector>

#include "libmmv/math/Vec3.h"

namespace libmmv {
	class ByteVolume;
}

namespace inpainting 
{
	class DictionaryDebugOutput
	{
	public: 
        static void writeDebugVolume(libmmv::ByteVolume* dictionaryVolume, libmmv::Vec3ui patchSize, std::string filename, std::vector<unsigned int> positions );
        static libmmv::ByteVolume* extractDictionaryToVolume(libmmv::ByteVolume* dictionaryVolume, libmmv::Vec3ui patchSize, std::vector<unsigned int> patches );
	};

} // namespace inpainting
