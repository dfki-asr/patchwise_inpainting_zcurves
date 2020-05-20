#pragma once

#include "IndexInterval.h"
#include "index/NDPointPair.h"

namespace ettention
{
	namespace inpainting 
	{

		template<class TDATA>
		class DataSet
		{
		public:
			virtual ~DataSet() {};
			enum IntervalBorders
			{
				KNOW_LOWER,
				KNOW_UPPER,
				KNOW_NONE
			};
			virtual IndexInterval getDataPointsInInterval(const HyperCube& interval, IndexInterval datasetInterval, IntervalBorders searchParameters ) = 0;
			virtual TDATA& getDataPointAtIntex(unsigned int index) = 0;
			virtual unsigned int sizeOfDataset() = 0;
		};

 
	} // namespace inpainting
} // namespace ettention