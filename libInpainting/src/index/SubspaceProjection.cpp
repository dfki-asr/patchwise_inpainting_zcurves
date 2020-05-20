#include "stdafx.h"

#include "SubspaceProjection.h"

namespace ettention
{
    namespace inpainting
    {

		SubspaceProjection::SubspaceProjection( int unprojectedSize, int projectedSize )
		{
			resize(projectedSize);
			for (size_t i = 0; i < projectedSize; i++)
				this->operator[](i).resize(unprojectedSize);
		}

		NDPoint SubspaceProjection::project(NDPoint unprojected)
		{
			NDPoint projectedData( size() );
			for (size_t row = 0; row < size(); row++)
			{
				float accumulator = 0;
				for (size_t column = 0; column < unprojected.size(); column++)
				{
					accumulator += (float) unprojected[column] * (*this)[row][column];
				}
				projectedData[row] = (int) accumulator;
			}
			return projectedData;
		}

		ettention::inpainting::SubspaceProjection SubspaceProjection::empty()
		{
			return SubspaceProjection( 0, 0 );
		}

		ettention::inpainting::SubspaceProjection SubspaceProjection::identity( int size )
		{
			SubspaceProjection projection( size, size );
			for (size_t j = 0; j < size; j++)
				for (size_t i = 0; i < size; i++)
					if (i == j)
						projection[i][j] = 1.0f;
					else
						projection[i][j] = 0.0f;
			return projection;
		}

		ettention::inpainting::SubspaceProjection SubspaceProjection::random(int unprojectedSize, int projectedSize)
		{
			SubspaceProjection randomProjection( unprojectedSize, projectedSize );
			std::random_device rd;
			std::mt19937 rng(rd());
			std::normal_distribution<> gaussian(0, 1);

			for (size_t i = 0; i < projectedSize; i++)
				for (size_t j = 0; j < unprojectedSize; j++)
				{
					randomProjection[i][j] = (float) gaussian(rng);
				}
			return randomProjection;
		}

	} // namespace
} // namespace
