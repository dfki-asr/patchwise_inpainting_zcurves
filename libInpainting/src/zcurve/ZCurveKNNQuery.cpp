#include "stdafx.h"

#include "ZCurveKNNQuery.h"
#include "patchselection/PatchSelectionStatistics.h"
#include "patchselection/Log.h"
#include "costfunction/L1Distance.h"
#include "costfunction/L2Distance.h"
#include "costfunction/L2DistanceWithWeight.h"

namespace ettention
{
	namespace inpainting 
	{  
		template<class NORM>
		ZCurveKNNQuery<NORM>::ZCurveKNNQuery( DataSet<ZCurveEntry>* dataset, int k, unsigned int minimumIntervalLengthToUseRecursion, Log* log)
            : dataset(dataset)
			, k(k)
            , minimumIntervalLengthToUseRecursion(minimumIntervalLengthToUseRecursion)
			, log(log)
        {
        }
		
		template<class NORM>
		ZCurveKNNQuery<NORM>::~ZCurveKNNQuery()
        {
        }

		template<class NORM>
		std::list<KNNCandidate>& ZCurveKNNQuery<NORM>::query( const ZCurveEntry& targetPoint )
        {
			nodesVisited = 0;
            IndexInterval fullRangeInterval( 0, dataset->sizeOfDataset()-1 );

			NDPoint first( targetPoint.value.size(), 0  );
			NDPoint last ( targetPoint.value.size(), 255);

			candidates.clear();

			HyperCube activeRange( first, last );
            recursiveKnnSearch(targetPoint, activeRange, fullRangeInterval );

			logQuery();

			return candidates;
        }

		template<class NORM>
		bool ZCurveKNNQuery<NORM>::recursiveKnnSearch( const ZCurveEntry& targetPoint, HyperCube& activeRange, IndexInterval datasetInterval )
        {
			if (datasetInterval.last >= (int) dataset->sizeOfDataset() || datasetInterval.last < datasetInterval.first)
				return false;

			bool didImproveDistance = false;

			if ( datasetInterval.length() <= minimumIntervalLengthToUseRecursion )
				return useBruteForceSearch( targetPoint, activeRange, datasetInterval );

			const unsigned int splitDimension = activeRange.findDimensionWhereHighestBitDiffersBetweenFirstAndLast( );
			const unsigned int splitPosition = activeRange.findPositionWhereHighestBitDiffersBetweenFirstAndLast();
			
			const float leftDistance = calculateDistanceToLeftSubrange( targetPoint, activeRange, splitPosition, splitDimension );
			const float rightDistance = calculateDistanceToRightSubrange( targetPoint, activeRange, splitPosition, splitDimension );

			IndexInterval lowerInterval = getLowerInterval(activeRange, datasetInterval, splitPosition, splitDimension);
			IndexInterval upperInterval = getUpperInterval(activeRange, datasetInterval, splitPosition, splitDimension);

			if (lowerInterval.last > upperInterval.first)
				throw std::runtime_error("illegal interval search result: overlap");

			if ( rightDistance >= leftDistance )
				didImproveDistance |= traverseLeftRight( targetPoint, activeRange, datasetInterval, lowerInterval, upperInterval, splitPosition, splitDimension, rightDistance );
			else
				didImproveDistance |= traverseRightLeft( targetPoint, activeRange, datasetInterval, lowerInterval, upperInterval, splitPosition, splitDimension, leftDistance );

			return didImproveDistance;
		}

		template<class NORM>
		float ZCurveKNNQuery<NORM>::kDistanceEntry()
		{
			if ( candidates.size() < k )
				return largeNumber;
			return candidates.back().distance;
		}

		template<class NORM>
		void ZCurveKNNQuery<NORM>::addCandidate(KNNCandidate candidate)
		{
			KNNCandidateNearer compare;
			auto position = std::lower_bound(candidates.begin(), candidates.end(), candidate, compare );
			candidates.insert( position, candidate );
			if (candidates.size() > k)
				candidates.pop_back();
		}

		template<class NORM>
		IndexInterval ZCurveKNNQuery<NORM>::getInterval(HyperCube& activeRange, IndexInterval datasetInterval)
		{
			return dataset->getDataPointsInInterval(activeRange, datasetInterval, DataSet<ZCurveEntry>::KNOW_NONE);
		}

		template<class NORM>
		bool ZCurveKNNQuery<NORM>::useBruteForceSearch(const ZCurveEntry& targetPoint, HyperCube& activeRange, IndexInterval datasetInterval)
		{
			nodesVisited += datasetInterval.length();
			bool didImproveDistance = false;

			for ( int i = datasetInterval.first; i <= datasetInterval.last; i++)
			{
				ZCurveEntry& dataPoint = dataset->getDataPointAtIntex(i);
				float distance = norm.distance(targetPoint.value, dataPoint.value, kDistanceEntry());

				if (distance < kDistanceEntry())
				{
					addCandidate(KNNCandidate(distance, dataPoint.key));
					didImproveDistance = true;
				}
			}

			if (didImproveDistance)
			{
				HyperCube newBoundingRange(targetPoint.value, targetPoint.value);
				newBoundingRange.extend((int)std::ceilf(kDistanceEntry()));
				activeRange.crop(newBoundingRange);
			}
			return didImproveDistance;
		}

		template<class NORM>
		float ZCurveKNNQuery<NORM>::calculateDistanceToLeftSubrange( const ZCurveEntry& targetPoint, HyperCube& activeRange, unsigned int splitPosition, unsigned int splitDimension )
		{
			const unsigned char maxInDimension = activeRange.last[splitDimension];
			activeRange.last[splitDimension] = splitPosition;
			const float leftDistance = norm.distance(activeRange, targetPoint.value, kDistanceEntry());
			activeRange.last[splitDimension] = maxInDimension;
			return leftDistance;
		}

		template<class NORM>
		float ZCurveKNNQuery<NORM>::calculateDistanceToRightSubrange( const ZCurveEntry& targetPoint, HyperCube& activeRange, unsigned int splitPosition, unsigned int splitDimension )
		{
			const unsigned char minInDimension = activeRange.first[splitDimension];
			activeRange.first[splitDimension] = splitPosition + 1;
			const float rightDistance = norm.distance(activeRange, targetPoint.value, kDistanceEntry());
			activeRange.first[splitDimension] = minInDimension;
			return rightDistance;
		}

		template<class NORM>
		bool ZCurveKNNQuery<NORM>::traverseLeftRight( const ZCurveEntry& targetPoint, HyperCube& activeRange, IndexInterval datasetInterval, IndexInterval lowerInterval, IndexInterval upperInterval, unsigned int splitPosition, unsigned int splitDimension, float rightDistance )
		{
			bool didImproveDistance = false;

			const unsigned char maxInDimension = activeRange.last[splitDimension];
			activeRange.last[splitDimension] = splitPosition;
			didImproveDistance = recursiveKnnSearch(targetPoint, activeRange, lowerInterval );
			activeRange.last[splitDimension] = maxInDimension;

			if (rightDistance >= kDistanceEntry())
				return didImproveDistance;

			const unsigned char minInDimension = activeRange.first[splitDimension];
			activeRange.first[splitDimension] = splitPosition + 1;
			didImproveDistance |= recursiveKnnSearch(targetPoint, activeRange, upperInterval );
			activeRange.first[splitDimension] = minInDimension;

			return didImproveDistance;
		}

		template<class NORM>
		bool ZCurveKNNQuery<NORM>::traverseRightLeft(const ZCurveEntry& targetPoint, HyperCube& activeRange, IndexInterval datasetInterval, IndexInterval lowerInterval, IndexInterval upperInterval, unsigned int splitPosition, unsigned int splitDimension, float leftDistance )
		{
			bool didImproveDistance = false;

			const unsigned char minInDimension = activeRange.first[splitDimension];
			activeRange.first[splitDimension] = splitPosition + 1;
			didImproveDistance = recursiveKnnSearch(targetPoint, activeRange, upperInterval);
			activeRange.first[splitDimension] = minInDimension;

			if (leftDistance >= kDistanceEntry())
				return didImproveDistance;

			const unsigned char maxInDimension = activeRange.last[splitDimension];
			activeRange.last[splitDimension] = splitPosition;
			didImproveDistance |= recursiveKnnSearch(targetPoint, activeRange, lowerInterval );
			activeRange.last[splitDimension] = maxInDimension;

			return didImproveDistance;
		}

		template<class NORM>
		IndexInterval ZCurveKNNQuery<NORM>::getLowerInterval( HyperCube& activeRange, IndexInterval datasetInterval, unsigned int splitPosition, unsigned int splitDimension )
		{
			const unsigned char maxInDimension = activeRange.last[splitDimension];
			activeRange.last[splitDimension] = splitPosition;
			// const IndexInterval lowerInterval = dataset->getDataPointsInInterval(activeRange, datasetInterval, DataSet<ZCurveEntry>::KNOW_LOWER);
			const IndexInterval lowerInterval = dataset->getDataPointsInInterval(activeRange, datasetInterval, DataSet<ZCurveEntry>::KNOW_NONE);
			activeRange.last[splitDimension] = maxInDimension;
			return lowerInterval;
		}

		template<class NORM>
		IndexInterval ZCurveKNNQuery<NORM>::getUpperInterval(HyperCube& activeRange, IndexInterval datasetInterval, unsigned int splitPosition, unsigned int splitDimension)
		{
			const unsigned char minInDimension = activeRange.first[splitDimension];
			activeRange.first[splitDimension] = splitPosition + 1;
			IndexInterval upperInterval = dataset->getDataPointsInInterval(activeRange, datasetInterval, DataSet<ZCurveEntry>::KNOW_NONE );
			activeRange.first[splitDimension] = minInDimension;
			return upperInterval;
		}

		template<class NORM>
		void ZCurveKNNQuery<NORM>::logQuery()
		{
			log->setKnnCandidates( candidates );

			float visitedNodes = (float) nodesVisited;
			float totalNodes = (float) dataset->sizeOfDataset();
			float selectivity = visitedNodes / totalNodes;

			log->setSelectivity(selectivity);
		}

		template class ZCurveKNNQuery<L1Distance<NDPoint> >;
		template class ZCurveKNNQuery<L2Distance<NDPoint> >;
		template class ZCurveKNNQuery<L2DistanceWithWeight<NDPoint> >;

	} // namespace inpainting
} // namespace ettention