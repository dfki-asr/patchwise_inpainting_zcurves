#include "stdafx.h"

#include "PresortedDataSet.h"

namespace inpainting 
{  

	template<class TDATA>
	PresortedDataSet<TDATA>::PresortedDataSet( std::vector<ZCurveEntry>& pointData )
		: pointData(pointData)
	{
	}

	template<class TDATA>
	PresortedDataSet<TDATA>::PresortedDataSet(std::istream& is)
	{
		loadFromStream( is );
	}

	template<class TDATA>
	PresortedDataSet<TDATA>::~PresortedDataSet()
	{
	}

	template<class TDATA>
	IndexInterval PresortedDataSet<TDATA>::searchCurve(const HyperCube& interval, IndexInterval datasetInterval, IntervalBorders searchParameters)
	{
		IndexInterval result = datasetInterval;
		result.containsExactResult = true;

		if (searchParameters != KNOW_LOWER)
		{
			int from = datasetInterval.first;
			int to = datasetInterval.last;
			result.first = upper_bound(interval.first, from, to);
		}

		if (searchParameters != KNOW_UPPER)
		{
			int from = datasetInterval.first;
			int to = datasetInterval.last;
			result.last = lower_bound(interval.last, from, to);
		}

		if (result.last > pointData.size() || result.first > result.last)
		{
			result.last = result.first - 1;
		}

		return result;
	}

	template<class TDATA>
	int PresortedDataSet<TDATA>::upper_bound(const NDPoint& patch, int& from, int& to)
	{
		while (from <= to)
		{
			const int middle = (from + to) / 2;
			if (less(patch, pointData[middle]))
			{
				to = middle - 1;
			}
			else
			{
				from = middle + 1;
			}
		}
		if (to >= 0)
			if (!less( pointData[to], patch))
				return to;
		return to + 1;
	}

	template<class TDATA>
	int PresortedDataSet<TDATA>::upper_bound(const NDPoint& patch)
	{
		int from = 0;
		int to = (int) pointData.size();
		return upper_bound(patch, from, to);
	}

	template<class TDATA>
	int PresortedDataSet<TDATA>::lower_bound(const NDPoint& patch, int& from, int& to)
	{
		while (from <= to)
		{
			const int middle = from + ((to - from) / 2);
			if (less(patch, pointData[middle]))
			{
				to = middle - 1;
			}
			else
			{
				from = middle + 1;
			}
		}
		if (from <= pointData.size() - 1)
			if (!less(patch, pointData[from]))
				return from;
		return from - 1;
	}

	template<class TDATA>
	int PresortedDataSet<TDATA>::lower_bound(const NDPoint& patch)
	{
		int from = 0;
		int to = (int)pointData.size();
		return lower_bound(patch, from, to);
	}

	template<class TDATA>
	unsigned int PresortedDataSet<TDATA>::sizeOfDataset()
	{
		return (unsigned int)pointData.size();
	}

	template<class TDATA>
	IndexInterval PresortedDataSet<TDATA>::getDataPointsInInterval( const HyperCube& interval, IndexInterval datasetInterval, IntervalBorders searchParameters )
	{
		return searchCurve(interval, datasetInterval, searchParameters);
	}

	template<class TDATA>
	TDATA& PresortedDataSet<TDATA>::getDataPointAtIntex(unsigned int index)
	{
		if (index > pointData.size())
			throw std::runtime_error( "index of of dataset bounds" );
		return pointData[index];
	}

	template<class TDATA>
	void PresortedDataSet<TDATA>::loadFromStream( std::istream& os )
	{
		ensure_binary_read( os, "Dataset" );

		unsigned int size;
		binary_read(os, size);

		unsigned int elementSize;
		binary_read(os, elementSize);

		ZCurveEntry exampleEntry;
		exampleEntry.value.resize(elementSize);
		pointData.resize( size, exampleEntry );

		/* for (size_t i = 0; i < pointData.size(); i++)
		{
			pointData[i].value.resize( elementSize );
		} */
		for (size_t i = 0; i < pointData.size(); i++)
		{
			pointData[i].performLoadFromStream(os);
		}
	}

	template<class TDATA>
	void PresortedDataSet<TDATA>::writeToStream(std::ostream& os)
	{
		std::string keyword( "Dataset" );
		binary_write(os, keyword);
		unsigned int size = (unsigned int) pointData.size();
		binary_write(os, size);
		unsigned int elementSize = (unsigned int) pointData[0].value.size();
		binary_write(os, elementSize);
		for (auto it : pointData)
			it.writeToStream( os );
	}

	template class PresortedDataSet<ZCurveEntry>;

} // namespace inpainting
