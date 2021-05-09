#pragma once

#include "index/PatchAccess8Bit.h"

namespace inpainting 
{

	template<class T>
	class PatchAccess8Bit;

    typedef std::vector<unsigned char> NDPoint;

    std::ostream& operator<<(std::ostream& ofs, const NDPoint& point);

    class NDPointPair
    {
    public:
		NDPointPair(int dimensions);
		NDPointPair( NDPoint first, NDPoint last );
		NDPointPair( const NDPointPair& other);

        int numberOfDimensions() const;
        int bitsPerDimension() const;

        NDPoint first;
        NDPoint last;
    };

    class HyperCube : public NDPointPair 
    {
    public:
		HyperCube( NDPoint first, NDPoint last );
		HyperCube(const BytePatchAccess8Bit& first, const BytePatchAccess8Bit& last);
		HyperCube( const HyperCube& other );
		HyperCube( int dimensions );

		bool contains(NDPoint& point);
		void extend(int radius);
		void crop(const HyperCube& other);
		bool hasZeroVolume();

		unsigned int findDimensionWhereHighestBitDiffersBetweenFirstAndLast( );
        unsigned int findPositionWhereHighestBitDiffersBetweenFirstAndLast();
        HyperCube generateLowerHalfspace(unsigned int splitDimension, unsigned int splitPosition);
        HyperCube generateUpperHalfspace(unsigned int splitDimension, unsigned int splitPosition);

		unsigned int numberOfPrefixBitsWhereFirstAndLastAreIdentical() const;
		unsigned int numberOfPostfixBitsWhereFirstIsZeroAndLastIsOne() const;
    };

    std::ostream& operator<<(std::ostream& ofs, const HyperCube& cube);

} // namespace inpainting