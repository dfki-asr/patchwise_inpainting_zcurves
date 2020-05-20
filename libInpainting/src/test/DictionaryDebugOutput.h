#pragma once

#include <vector>

#include "math/Vec3.h"

namespace ettention
{
    class ByteVolume;

	namespace inpainting 
	{
		class DictionaryDebugOutput
		{
		public: 
            static void writeDebugVolume( ByteVolume* dictionaryVolume, Vec3ui patchSize, std::string filename, std::vector<unsigned int> positions );
            static ByteVolume* extractDictionaryToVolume(ByteVolume* dictionaryVolume, Vec3ui patchSize, std::vector<unsigned int> patches );
		};


	} // namespace inpainting
} // namespace ettention