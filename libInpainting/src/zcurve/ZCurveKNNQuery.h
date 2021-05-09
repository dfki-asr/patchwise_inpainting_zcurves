#pragma once

#include "ZCurveOperator.h"
#include "index/NDPointPair.h"
#include "IndexInterval.h"
#include "costfunction/L2Distance.h"
#include "DataSet.h"
#include "KNNCandidate.h"

namespace inpainting 
{
	class Log;

	class KNNQuery
	{
	public:
		virtual std::list<KNNCandidate>& query(const ZCurveEntry& targetPoint) = 0;
	};

	template<class NORM>
    class ZCurveKNNQuery: public KNNQuery
    {
    public:
        ZCurveKNNQuery<NORM>( DataSet<ZCurveEntry>* dataset, int k, unsigned int minimumIntervalLengthToUseRecursion, Log* log );
		virtual ~ZCurveKNNQuery();

        virtual std::list<KNNCandidate>& query( const ZCurveEntry& targetPoint );

    public:
		virtual bool recursiveKnnSearch(const ZCurveEntry& targetPoint, HyperCube& activeRange, IndexInterval datasetInterval );			
		virtual float kDistanceEntry();
		virtual void addCandidate( KNNCandidate candidate );
		IndexInterval getInterval(HyperCube& activeRange, IndexInterval datasetInterval);

    protected:
		bool useBruteForceSearch(const ZCurveEntry& targetPoint, HyperCube& activeRange, IndexInterval datasetInterval );
		float calculateDistanceToLeftSubrange( const ZCurveEntry& targetPoint, HyperCube& activeRange, unsigned int splitPosition, unsigned int splitDimension );
		float calculateDistanceToRightSubrange( const ZCurveEntry& targetPoint, HyperCube& activeRange, unsigned int splitPosition, unsigned int splitDimension );
		bool traverseLeftRight(const ZCurveEntry& targetPoint, HyperCube& activeRange, IndexInterval datasetInterval, IndexInterval lowerInterval, IndexInterval upperInterval, unsigned int splitPosition, unsigned int splitDimension, float rightDistance );
		bool traverseRightLeft(const ZCurveEntry& targetPoint, HyperCube& activeRange, IndexInterval datasetInterval, IndexInterval lowerInterval, IndexInterval upperInterval, unsigned int splitPosition, unsigned int splitDimension, float leftDistance );

		IndexInterval getLowerInterval(HyperCube& activeRange, IndexInterval datasetInterval, unsigned int splitPosition, unsigned int splitDimension);
		IndexInterval getUpperInterval(HyperCube& activeRange, IndexInterval datasetInterval, unsigned int splitPosition, unsigned int splitDimension);

		virtual void logQuery();

	protected:
		int k;
		DataSet<ZCurveEntry>* dataset;
		NORM norm;

		std::list<KNNCandidate> candidates;
        unsigned int minimumIntervalLengthToUseRecursion;

		// do not use std::numeric_limits<float>::max() to prevent numeric overflow when computing LPDistance
		const float largeNumber = 255.0f * 255.0f; 
		int nodesVisited;
		Log* log;
    };

} // namespace inpainting