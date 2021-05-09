#pragma once

#include "ZCurveOperator.h"
#include "IndexInterval.h"
#include "ZCurveKNNQuery.h"
#include "parallel/WorkerThread.h"

namespace inpainting 
{

	template<class TDATA>
	class PresortedDataSet : public DataSet<TDATA>, public Serializable
    {
    public:
		PresortedDataSet(std::vector<ZCurveEntry>& pointData);
		PresortedDataSet( std::istream& is );
		virtual ~PresortedDataSet();

		IndexInterval searchCurve(const HyperCube& interval, IndexInterval datasetInterval, IntervalBorders searchParameters);

		int lower_bound(const NDPoint& patch, int& from, int& to);
		int lower_bound(const NDPoint& patch);
		int upper_bound(const NDPoint& patch, int& from, int& to);
		int upper_bound(const NDPoint& patch);

		virtual IndexInterval getDataPointsInInterval( const HyperCube& interval, IndexInterval datasetInterval, IntervalBorders searchParameters ) override;
		virtual TDATA& getDataPointAtIntex(unsigned int index) override;
		virtual unsigned int sizeOfDataset() override;

	public:
		virtual void writeToStream(std::ostream& os) override;
		virtual void loadFromStream(std::istream& is) override;

	protected:
		std::vector<ZCurveEntry> pointData;
		IndexEntryLessOnZCurve less;
	};

} // namespace inpainting
