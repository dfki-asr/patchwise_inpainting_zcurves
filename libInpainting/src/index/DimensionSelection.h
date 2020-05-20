#pragma once

#include "math/Vec3.h"
#include "index/PatchAccess8Bit.h"
#include "algorithm/Coordinates.h"

namespace ettention
{

	class ByteVolume;
	
	namespace inpainting 
	{

		class DimensionSelection
		{
		public:
			enum PreferredDirection 
			{
				STANDARD,
				CENTER_2D, 
				TOP, 
				LEFT, 
				RIGHT, 
				BOTTOM, 
				TOPLEFT, 
				TOPRIGHT, 
				BOTTOMLEFT, 
				BOTTOMRIGHT,
				CENTER_3D,
				CORNER000,
				CORNER001,
				CORNER010,
				CORNER011,
				CORNER100,
				CORNER101,
				CORNER110,
				CORNER111,
				FACE00N,
				FACE00P,
				FACE0N0,
				FACE0P0,
				FACEN00,
				FACEP00,
				MIDDLE
			};

			struct CompareDimension2D
			{
				bool operator()(Vec3i a, Vec3i  b);
				Vec3i patchSize;
				Vec3i preferredDirection;
			};

			struct CompareDimension3D
			{
				bool operator()(Vec3i a, Vec3i  b);
				Vec3i patchSize;
				Vec3i preferredDirection;
			};

			static std::vector<unsigned int> flattenSequence(Vec3i patchSize, std::vector<Vec3i> sequence);
			static std::vector<unsigned int> standardPermutation(Vec3i patchSize);
			static std::vector<unsigned int> customPermutation(Vec3i patchSize, PreferredDirection selection, int size );
			static std::vector<Vec3i> standardSequence(Vec3i patchSize);
			static std::vector<Vec3i> customSequence( Vec3i patchSize, PreferredDirection selection );
			static Vec3i getPermutationPreferredPosition(Vec3i patchSize, PreferredDirection selection );
			static bool is3DDirection( PreferredDirection direction );

			static std::string to_string( PreferredDirection direction );
			static std::vector<PreferredDirection> allIndexDirections();
		};

    }
}