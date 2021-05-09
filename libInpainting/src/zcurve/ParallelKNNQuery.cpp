#include "stdafx.h"

#include "ParallelKNNQuery.h"
#include "parallel/ThreadPool.h"
#include "patchselection/Log.h"
#include "costfunction/L1Distance.h"
#include "costfunction/L2Distance.h"
#include "costfunction/L2DistanceWithWeight.h"

namespace inpainting 
{  

	template<class NORM>
	KNNQueryJob<NORM>::KNNQueryJob( ParallelKNNQuery<NORM>* parentQuery, const ZCurveEntry& targetPoint, HyperCube& activeRange, IndexInterval datasetInterval, float priority )
		: Job(priority)
		, parentQuery(parentQuery), targetPoint(targetPoint), activeRange(activeRange), datasetInterval(datasetInterval)
	{
	}

	template<class NORM>
	void KNNQueryJob<NORM>::performWork()
	{
		HyperCube newBoundingRange(targetPoint.value, targetPoint.value);
		newBoundingRange.extend((int) ceilf( parentQuery->kDistanceEntry() ) );
		activeRange.crop(newBoundingRange);
		datasetInterval = parentQuery->getInterval( activeRange, datasetInterval );
		parentQuery->recursiveKnnSearch( targetPoint, activeRange, datasetInterval );
	}

	template<class NORM>
	ParallelKNNQuery<NORM>::ParallelKNNQuery( ettention::ThreadPool* threadPool, DataSet<ZCurveEntry>* dataset, int k, unsigned int minimumIntervalLengthToUseRecursion, unsigned int minimumIntervalLengthToUseParallelism, Log* log )
		: ZCurveKNNQuery<NORM>(dataset, k, minimumIntervalLengthToUseRecursion, log )
		, minimumIntervalLengthToUseParallelism(minimumIntervalLengthToUseParallelism)
		, threadPool(threadPool)
	{
	}

	template<class NORM>
	ParallelKNNQuery<NORM>::~ParallelKNNQuery()
	{
	}

	template<class NORM>
	std::list<KNNCandidate>& ParallelKNNQuery<NORM>::query(const ZCurveEntry& targetPoint)
	{
		currentKDistanceEntry.store( this->largeNumber );
			
		this->nodesVisited = 0;
		IndexInterval fullRangeInterval(0, this->dataset->sizeOfDataset() - 1);

		NDPoint first(targetPoint.value.size(), 0);
		NDPoint last(targetPoint.value.size(), 255);

		this->candidates.clear();

		HyperCube activeRange(first, last);
		recursiveKnnSearch(targetPoint, activeRange, fullRangeInterval);

		threadPool->waitForJobsToFinish();
		logQuery();
		threadPool->clearFinishedTasks();
		return this->candidates;
	}

	template<class NORM>
	bool ParallelKNNQuery<NORM>::recursiveKnnSearch(const ZCurveEntry& targetPoint, HyperCube& activeRange, IndexInterval datasetInterval)
	{
		if (datasetInterval.last >= (int) this->dataset->sizeOfDataset() || datasetInterval.last < datasetInterval.first)
			return false;

		bool didImproveDistance = false;

		if ( datasetInterval.length() <= this->minimumIntervalLengthToUseRecursion )
			return this->useBruteForceSearch( targetPoint, activeRange, datasetInterval );

		const unsigned int splitDimension = activeRange.findDimensionWhereHighestBitDiffersBetweenFirstAndLast();
		const unsigned int splitPosition = activeRange.findPositionWhereHighestBitDiffersBetweenFirstAndLast();

		const float leftDistance = this->calculateDistanceToLeftSubrange( targetPoint, activeRange, splitPosition, splitDimension );
		const float rightDistance = this->calculateDistanceToRightSubrange( targetPoint, activeRange, splitPosition, splitDimension );

		IndexInterval lowerInterval = this->getLowerInterval(activeRange, datasetInterval, splitPosition, splitDimension);
		IndexInterval upperInterval = this->getUpperInterval(activeRange, datasetInterval, splitPosition, splitDimension);

		if (lowerInterval.length() > this->minimumIntervalLengthToUseParallelism && upperInterval.length() > minimumIntervalLengthToUseParallelism)
		{
			if (rightDistance >= leftDistance)
				this->traverseLeftRightAsync(targetPoint, activeRange, datasetInterval, lowerInterval, upperInterval, splitPosition, splitDimension, leftDistance, rightDistance);
			else
				this->traverseRightLeftAsync(targetPoint, activeRange, datasetInterval, lowerInterval, upperInterval, splitPosition, splitDimension, leftDistance, rightDistance);
		}
		else
		{
			if (rightDistance >= leftDistance)
				didImproveDistance |= this->traverseLeftRight(targetPoint, activeRange, datasetInterval, lowerInterval, upperInterval, splitPosition, splitDimension, rightDistance);
			else
				didImproveDistance |= this->traverseRightLeft(targetPoint, activeRange, datasetInterval, lowerInterval, upperInterval, splitPosition, splitDimension, leftDistance);
		}
		return didImproveDistance;
	}

	template<class NORM>
	float ParallelKNNQuery<NORM>::kDistanceEntry()
	{
		return currentKDistanceEntry.load();
	}

	template<class NORM>
	void ParallelKNNQuery<NORM>::addCandidate(KNNCandidate candidate)
	{
		candidatesMutex.lock();
		ZCurveKNNQuery<NORM>::addCandidate( candidate );
		float newKDistanceEntry = ZCurveKNNQuery<NORM>::kDistanceEntry();
		currentKDistanceEntry.store( newKDistanceEntry );
		candidatesMutex.unlock();
	}

	template<class NORM>
	void ParallelKNNQuery<NORM>::traverseLeftRightAsync(const ZCurveEntry& targetPoint, HyperCube& activeRange, IndexInterval datasetInterval, IndexInterval lowerInterval, IndexInterval upperInterval, unsigned int splitPosition, unsigned int splitDimension, float leftDistance, float rightDistance)
	{
		const unsigned char maxInDimension = activeRange.last[splitDimension];
		activeRange.last[splitDimension] = splitPosition;
		asyncKnnSearch(targetPoint, activeRange, lowerInterval, leftDistance );
		activeRange.last[splitDimension] = maxInDimension;

		if (rightDistance >= kDistanceEntry())
			return;

		const unsigned char minInDimension = activeRange.first[splitDimension];
		activeRange.first[splitDimension] = splitPosition + 1;
		asyncKnnSearch(targetPoint, activeRange, upperInterval, rightDistance );
		activeRange.first[splitDimension] = minInDimension;
	}

	template<class NORM>
	void ParallelKNNQuery<NORM>::traverseRightLeftAsync(const ZCurveEntry& targetPoint, HyperCube& activeRange, IndexInterval datasetInterval, IndexInterval lowerInterval, IndexInterval upperInterval, unsigned int splitPosition, unsigned int splitDimension, float leftDistance, float rightDistance)
	{
		const unsigned char minInDimension = activeRange.first[splitDimension];
		activeRange.first[splitDimension] = splitPosition + 1;
		asyncKnnSearch(targetPoint, activeRange, upperInterval, rightDistance );
		activeRange.first[splitDimension] = minInDimension;

		if (leftDistance >= kDistanceEntry())
			return;

		const unsigned char maxInDimension = activeRange.last[splitDimension];
		activeRange.last[splitDimension] = splitPosition;
		asyncKnnSearch(targetPoint, activeRange, lowerInterval, leftDistance );
		activeRange.last[splitDimension] = maxInDimension;
	}

	template<class NORM>
	void ParallelKNNQuery<NORM>::asyncKnnSearch(const ZCurveEntry& targetPoint, HyperCube& activeRange, IndexInterval datasetInterval, float distance )
	{
		auto job = new KNNQueryJob<NORM>( this, targetPoint, activeRange, datasetInterval, distance );
		this->threadPool->enqueue(job);
	}

	template<class NORM>
	void ParallelKNNQuery<NORM>::logQuery()
	{
		ZCurveKNNQuery<NORM>::logQuery();
		this->log->setNJobs( threadPool->numberOfProcessedJobs() );
	}

	template class KNNQueryJob< L1Distance< NDPoint> >;
	template class KNNQueryJob< L2Distance< NDPoint> >;
	template class KNNQueryJob< L2DistanceWithWeight< NDPoint> >;

	template class ParallelKNNQuery< L1Distance< NDPoint> >;
	template class ParallelKNNQuery< L2Distance< NDPoint> >;
	template class ParallelKNNQuery< L2DistanceWithWeight< NDPoint> >;

} // namespace inpainting