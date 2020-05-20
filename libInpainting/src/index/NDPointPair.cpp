#include "stdafx.h"

#include "PatchAccess8Bit.h"
#include "NDPointPair.h"

namespace ettention
{
	namespace inpainting 
	{  

        NDPointPair::NDPointPair( int dimensions )
            : first(dimensions), last(dimensions)
        {

        }

        NDPointPair::NDPointPair( NDPoint first, NDPoint last )
            : first(first), last(last)
        {
            if (first.size() != last.size())
                throw std::runtime_error("dimension mismatch when creating N-D point pair");
        }

        NDPointPair::NDPointPair(const NDPointPair& other)
            : first( other.first ), last( other.last )
        {

        }

		int NDPointPair::numberOfDimensions() const
        {
            return (int) first.size();
        }

        int NDPointPair::bitsPerDimension() const
        {
            return CHAR_BIT * sizeof(first[0]);
        }

        HyperCube::HyperCube(NDPoint first, NDPoint last)
            : NDPointPair( first, last )
        {

        }

		HyperCube::HyperCube(const BytePatchAccess8Bit& first, const BytePatchAccess8Bit& last)
			: NDPointPair( (unsigned int) first.size() )
		{
			for (unsigned int i = 0; i < first.size(); i++)
			{
				this->first[i] = first[i];
				this->last[i] = last[i];
			}
		}

        HyperCube::HyperCube(const HyperCube& other)
            : NDPointPair( other )
        {

        }

		HyperCube::HyperCube(int dimensions)
			: NDPointPair( dimensions )
		{
			for (size_t i = 0; i < dimensions; i++)
			{
				first[i] = 0;
				last[i] = 255;
			}
		}

		bool HyperCube::contains(NDPoint& point)
        {
            for ( unsigned int dimension = 0; dimension < (unsigned int) first.size(); dimension++)
            {
                if (first[dimension] > point[dimension])
                    return false;
                if (last[dimension] < point[dimension])
                    return false;
            }
            return true;
        }

		void HyperCube::extend( int radius )
		{
			for (size_t i = 0; i < first.size(); i++)
				first[i] = (unsigned char) std::max(0, ((int)first[i]) - radius);
			for (size_t i = 0; i < last.size(); i++)
				last[i] = (unsigned char) std::min(255, ((int)last[i]) + radius);
		}

		void HyperCube::crop(const HyperCube& other)
		{
			for (size_t i = 0; i < first.size(); i++)
				first[i] = std::max(first[i], other.first[i]);
			for (size_t i = 0; i < last.size(); i++)
				last[i] = std::min(last[i], other.last[i]);
		}

		bool HyperCube::hasZeroVolume()
		{
			for (size_t i = 0; i < first.size(); i++)
				if (first[i] > last[i])
					return true;
			return false;
		}

		unsigned int HyperCube::findDimensionWhereHighestBitDiffersBetweenFirstAndLast( )
        {
            int nBits = bitsPerDimension();

            for (int bit = 0; bit < nBits; bit++)
            {
                unsigned int bitmask = 1 << (nBits - bit - 1);

                for (int dimension = 0; dimension < (int)first.size(); dimension++)
                {
                    unsigned int aBit = first[dimension] & bitmask;
                    unsigned int bBit = last[dimension] & bitmask;

                    if (aBit != bBit)
                        return dimension;
                }
            }
            throw std::runtime_error("Hypercube contains only a single point (first==last), so no split position can be computed");
        }

        unsigned int HyperCube::findPositionWhereHighestBitDiffersBetweenFirstAndLast()
        {
            int nBits = bitsPerDimension();

            for (int bitThatDiffers = 0; bitThatDiffers < nBits; bitThatDiffers++)
            {
                unsigned int bitmask = 1 << (nBits - bitThatDiffers - 1);

                for (int dimension = 0; dimension < (int) first.size(); dimension++)
                {
                    unsigned int aBit = first[dimension] & bitmask;
                    unsigned int bBit = last[dimension] & bitmask;

                    if (aBit != bBit)
                    {
                        unsigned int resultPosition = bitmask;
                        for (int resultBit = 0; resultBit <= bitThatDiffers-1; resultBit++)
                        {
                            bitmask = 1 << (nBits - resultBit - 1);
                            resultPosition += bitmask & first[dimension];
                        }
                        return resultPosition-1;
                    }
                }
            }
            throw std::runtime_error("Hypercube contains only a single point (first==last), so no split position can be computed");
        }

        HyperCube HyperCube::generateLowerHalfspace(unsigned int splitDimension, unsigned int splitPosition)
        {
            HyperCube lowerHalfSpace( *this );
            lowerHalfSpace.last[splitDimension] = splitPosition;
            return lowerHalfSpace;
        }

        HyperCube HyperCube::generateUpperHalfspace(unsigned int splitDimension, unsigned int splitPosition)
        {
            HyperCube upperHalfSpace(*this);
            upperHalfSpace.first[splitDimension] = splitPosition+1;
            return upperHalfSpace;
        }

		/*
        ZCurveInterval::ZCurveInterval(NDPoint first, NDPoint last)
            : NDPointPair(first, last)
        {
        }

		std::string ZCurveInterval::firstToString()
        {
            return toString(first);
        }

        std::string ZCurveInterval::lastToString()
        {
            return toString(last);
        }

        std::string ZCurveInterval::toString(NDPoint point)
        {
            unsigned int nBits = CHAR_BIT * sizeof( point[0] );
            std::string result = "";

            for (int bit = nBits - 1; bit >= 0; bit--)
            {
                unsigned int bitmask = 1 << (nBits - bit - 1);

                for (unsigned int dimension = 0; dimension < (unsigned int) point.size(); dimension++)
                {
                    unsigned int bit = point[dimension] & bitmask;

                    if (bit == 0)
                        result = "0" + result;
                    else
                        result = "1" + result;
                }
            }
            return result;
        }
		*/

        unsigned int HyperCube::numberOfPrefixBitsWhereFirstAndLastAreIdentical() const
        {
            unsigned int nBits = bitsPerDimension();
            unsigned int prefixCount = 0;

            for (unsigned int bit = 0; bit < nBits; bit++)
            {
                unsigned int bitmask = 1 << (nBits - bit - 1);

                for (unsigned int dimension = 0; dimension < (unsigned int) first.size(); dimension++ )
                {
                    unsigned int aBit = first[dimension] & bitmask;
                    unsigned int bBit = last[dimension] & bitmask;
                    if ( aBit == bBit )
                        prefixCount++;
                    else
                        return prefixCount;
                }
            }
            return prefixCount;
        }


        unsigned int HyperCube::numberOfPostfixBitsWhereFirstIsZeroAndLastIsOne() const
        {
            unsigned int nBits = bitsPerDimension();
            unsigned int postfixCount = 0;

            for (int bit = nBits - 1; bit >= 0; bit--)
            {
                unsigned int bitmask = 1 << (nBits - bit - 1);

                for (int dimension = (unsigned int)first.size() - 1; dimension >= 0; dimension--)
                {
                    unsigned int aBit = first[dimension] & bitmask;
                    unsigned int bBit = last[dimension] & bitmask;
                    if ( aBit == 0 && bBit != 0 )
                        postfixCount++;
                    else
                        return postfixCount;
                }
            }
            return postfixCount;
        }

        std::ostream& operator<<(std::ostream& ofs, const NDPoint& point)
        {
			if (point.size() == 0)
			{
				return ofs;
			}
            for ( size_t i = 0; i < point.size() - 1; i++ )
                ofs << (unsigned int) point[i] << "/";
            ofs << (unsigned int) point[point.size() - 1];
            return ofs;
        }

        std::ostream& operator<<(std::ostream& ofs, const HyperCube& cube)
        {
            ofs << "from: " << cube.first << " to: " << cube.last;
            return ofs;
        }

    } // namespace inpainting
} // namespace ettention