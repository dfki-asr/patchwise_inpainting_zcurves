#pragma once

#include "libmmv/math/Vec3.h"
#include "index/PatchAccess8Bit.h"
#include "libmmv/algorithm/Coordinates.h"

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
			bool operator()(libmmv::Vec3i a, libmmv::Vec3i  b);
			libmmv::Vec3i patchSize;
			libmmv::Vec3i preferredDirection;
		};

		struct CompareDimension3D
		{
			bool operator()(libmmv::Vec3i a, libmmv::Vec3i  b);
			libmmv::Vec3i patchSize;
			libmmv::Vec3i preferredDirection;
		};

		static std::vector<unsigned int> flattenSequence(libmmv::Vec3i patchSize, std::vector<libmmv::Vec3i> sequence);
		static std::vector<unsigned int> standardPermutation(libmmv::Vec3i patchSize);
		static std::vector<unsigned int> customPermutation(libmmv::Vec3i patchSize, PreferredDirection selection, int size );
		static std::vector<libmmv::Vec3i> standardSequence(libmmv::Vec3i patchSize);
		static std::vector<libmmv::Vec3i> customSequence(libmmv::Vec3i patchSize, PreferredDirection selection );
		static libmmv::Vec3i getPermutationPreferredPosition(libmmv::Vec3i patchSize, PreferredDirection selection );
		static bool is3DDirection( PreferredDirection direction );

		static std::string to_string( PreferredDirection direction );
		static std::vector<PreferredDirection> allIndexDirections();
	};

}