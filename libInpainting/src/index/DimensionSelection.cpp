#include "stdafx.h"

#include "DimensionSelection.h"

namespace inpainting
{

	bool DimensionSelection::CompareDimension2D::operator()(libmmv::Vec3i a, libmmv::Vec3i b)
	{
		libmmv::Vec3i center = patchSize / 2;
		libmmv::Vec2i differenceA = componentWiseAbs(a - preferredDirection).xy();
		libmmv::Vec2i differenceB = componentWiseAbs(b - preferredDirection).xy();
		int distanceA = differenceA.x * differenceA.x + differenceA.y * differenceA.y;
		int distanceB = differenceB.x * differenceB.x + differenceB.y * differenceB.y;

		if ( distanceA < distanceB )
			return true;
		if ( distanceB < distanceA )
			return false;

		return ( a.xy() < b.xy() );
	}

	bool DimensionSelection::CompareDimension3D::operator()(libmmv::Vec3i a, libmmv::Vec3i b)
	{
		libmmv::Vec3i center = patchSize / 2;
		libmmv::Vec3i differenceA = componentWiseAbs(a - preferredDirection);
		libmmv::Vec3i differenceB = componentWiseAbs(b - preferredDirection);
		int distanceA = differenceA.x * differenceA.x + differenceA.y * differenceA.y + differenceA.z * differenceA.z;
		int distanceB = differenceB.x * differenceB.x + differenceB.y * differenceB.y + differenceB.z * differenceB.z;

		if (distanceA < distanceB)
			return true;
		if (distanceB < distanceA)
			return false;

		return (a < b);
	}


	std::vector<unsigned int> DimensionSelection::flattenSequence(libmmv::Vec3i patchSize, std::vector<libmmv::Vec3i> sequence)
	{
		std::vector<unsigned int> result(sequence.size());
		for (unsigned int i = 0; i < sequence.size(); i++)
		{
			result[i] = Flatten3D(sequence[i], patchSize);
		}
		return result;
	}

	std::vector<unsigned int> DimensionSelection::standardPermutation(libmmv::Vec3i patchSize)
	{
		return flattenSequence(patchSize, standardSequence(patchSize));
	}

	std::vector<unsigned int> DimensionSelection::customPermutation(libmmv::Vec3i patchSize, PreferredDirection selection, int size )
	{
		auto sequence = flattenSequence(patchSize, customSequence(patchSize, selection));
		sequence.resize( size );
		return sequence;
	}

	std::vector<libmmv::Vec3i> DimensionSelection::standardSequence(libmmv::Vec3i patchSize)
	{
		std::vector<libmmv::Vec3i> result(patchSize.x * patchSize.y * patchSize.z);
		libmmv::Vec3i position;

		unsigned int i = 0;
		for (position.z = 0; position.z < patchSize.z; position.z++)
			for (position.y = 0; position.y  < patchSize.y; position.y++)
				for (position.x = 0; position.x < patchSize.x; position.x++)
				{
					result[i] = position;
					i++;
				}

		return result;
	}

	std::vector<libmmv::Vec3i> DimensionSelection::customSequence(libmmv::Vec3i patchSize, PreferredDirection selection )
	{
		std::vector<libmmv::Vec3i> permutation = standardSequence( patchSize );
		if (selection == STANDARD)
			return permutation;

		if ( is3DDirection(selection) )
		{
			CompareDimension3D compare;
			compare.patchSize = patchSize;
			compare.preferredDirection = getPermutationPreferredPosition(patchSize, selection);
			std::sort(permutation.begin(), permutation.end(), compare);
		}
		else
		{
			CompareDimension2D compare;
			compare.patchSize = patchSize;
			compare.preferredDirection = getPermutationPreferredPosition(patchSize, selection);
			std::sort(permutation.begin(), permutation.end(), compare);
		}
		return permutation;
	}

	libmmv::Vec3i DimensionSelection::getPermutationPreferredPosition(libmmv::Vec3i patchSize, PreferredDirection selection)
	{
		switch ( selection )
		{
			case CENTER_2D:
				return libmmv::Vec3i(patchSize.x / 2, patchSize.y / 2, patchSize.z / 2);
			case CENTER_3D:
				return libmmv::Vec3i(patchSize.x / 2, patchSize.y / 2, patchSize.z / 2);

			case TOP:
				return libmmv::Vec3i( patchSize.x / 2, 0, patchSize.z / 2);
			case BOTTOM:
				return libmmv::Vec3i(patchSize.x / 2, patchSize.y-1, patchSize.z / 2);
			case LEFT:
				return libmmv::Vec3i(0, patchSize.y / 2, patchSize.z / 2);
			case RIGHT:
				return libmmv::Vec3i(patchSize.x - 1, patchSize.y / 2, patchSize.z / 2);

			case TOPLEFT:
				return libmmv::Vec3i(0, 0, patchSize.z / 2);
			case BOTTOMLEFT:
				return libmmv::Vec3i(0, patchSize.y - 1, patchSize.z / 2);
			case TOPRIGHT:
				return libmmv::Vec3i(patchSize.x - 1, 0, patchSize.z / 2);
			case BOTTOMRIGHT:
				return libmmv::Vec3i(patchSize.x - 1, patchSize.y - 1, patchSize.z / 2);
			case CORNER000:
				return libmmv::Vec3i( 0,              0,               0 );
			case CORNER001:
				return libmmv::Vec3i( 0,              0,               patchSize.z - 1 );
			case CORNER010:
				return libmmv::Vec3i( 0,              patchSize.y - 1, 0 );
			case CORNER011:
				return libmmv::Vec3i(0,               patchSize.y - 1, patchSize.z - 1);
			case CORNER100:
				return libmmv::Vec3i(patchSize.x - 1, 0,               0 );
			case CORNER101:
				return libmmv::Vec3i(patchSize.x - 1, 0,               patchSize.z - 1);
			case CORNER110:
				return libmmv::Vec3i(patchSize.x - 1, patchSize.y - 1, 0);
			case CORNER111:
				return libmmv::Vec3i(patchSize.x - 1, patchSize.y - 1, patchSize.z - 1);

			case FACE00N:
				return libmmv::Vec3i(patchSize.x / 2, patchSize.y / 2, 0);
			case FACE00P:
				return libmmv::Vec3i(patchSize.x / 2, patchSize.y / 2, patchSize.z - 1);
			case FACE0N0:
				return libmmv::Vec3i(patchSize.x / 2, 0,               patchSize.z / 2);
			case FACE0P0:
				return libmmv::Vec3i(patchSize.x / 2, patchSize.y - 1, patchSize.z / 2);
			case FACEN00:
				return libmmv::Vec3i(0,               patchSize.y / 2, patchSize.z / 2);
			case FACEP00:
				return libmmv::Vec3i(patchSize.x - 1, patchSize.y / 2, patchSize.z / 2);
			case MIDDLE:
				return libmmv::Vec3i(patchSize.x / 2, patchSize.y / 2, patchSize.z / 2);
		};
		throw std::runtime_error("illegal index direction");
	}

	bool DimensionSelection::is3DDirection(PreferredDirection direction)
	{
		if (direction >= CENTER_3D )
			return true;
		return false;
	}

	std::string DimensionSelection::to_string(PreferredDirection direction)
	{
		switch (direction)
		{
		case STANDARD:
			return "standard";
		case CENTER_2D:
			return "center(2D)";
		case TOP:
			return "top";
		case LEFT:
			return "left";
		case RIGHT:
			return "right";
		case BOTTOM:
			return "bottom";
		case TOPLEFT:
			return "top_left";
		case TOPRIGHT:
			return "top_right";
		case BOTTOMLEFT:
			return "bottom_left";
		case BOTTOMRIGHT:
			return "bottom_right";

		case CENTER_3D:
			return "center(3D)";
		case CORNER000:
			return "corner_0_0_0";
		case CORNER001:
			return "corner_0_0_1";
		case CORNER010:
			return "corner_0_1_0";
		case CORNER011:
			return "corner_0_1_1";
		case CORNER100:
			return "corner_1_0_0";
		case CORNER101:
			return "corner_1_0_1";
		case CORNER110:
			return "corner_1_1_0";
		case CORNER111:
			return "corner_1_1_1";

		case FACE00N:
			return "face_0_0_N";
		case FACE00P:
			return "face_0_0_P";
		case FACE0N0:
			return "face_0_N_0";
		case FACE0P0:
			return "face_0_P_0";
		case FACEN00:
			return "face_N_0_0";
		case FACEP00:
			return "face_P_0_0";
		case MIDDLE:
			return "middle";
		}
		return "illegal";
	}

	std::vector<DimensionSelection::PreferredDirection> DimensionSelection::allIndexDirections()
	{
		return {
			STANDARD, 
			CENTER_2D, 
			TOP, LEFT, RIGHT,BOTTOM,
			TOPLEFT, TOPRIGHT, BOTTOMLEFT, BOTTOMRIGHT,
			CENTER_3D,
			CORNER000,CORNER001, CORNER010, CORNER011, CORNER100, CORNER101, CORNER110, CORNER111,
			FACE00N, FACE00P, FACE0N0, FACE0P0, FACEN00, FACEP00, MIDDLE 
		};
	}

	} // namespace
