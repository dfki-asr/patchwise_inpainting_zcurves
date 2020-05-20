#pragma once

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

#include "parallel/Job.h"
#include "ZCurveKNNQuery.h"

namespace ettention
{
	class ThreadPool;

	namespace inpainting
	{
		class BinarySearch;

		template<class NORM>
		class ParallelKNNQuery;

		class InpaintingOptimizationParameters;

		template<class NORM>
		class KNNQueryJob : public Job
		{
		public:
			KNNQueryJob( ParallelKNNQuery<NORM>* parentQuery, const ZCurveEntry& targetPoint, HyperCube& activeRange, IndexInterval datasetInterval, float priority );
			virtual void performWork() override;

			ParallelKNNQuery<NORM>* parentQuery;
			const ZCurveEntry& targetPoint;
			HyperCube activeRange;
			IndexInterval datasetInterval;
		};

		template<class NORM>
        class ParallelKNNQuery : public ZCurveKNNQuery<NORM>
        {
        public:
			ParallelKNNQuery<NORM>( ThreadPool* threadPool, DataSet<ZCurveEntry>* dataset, int k, unsigned int minimumIntervalLengthToUseRecursion, unsigned int minimumIntervalLengthToUseParallelism, Log* log);
			virtual ~ParallelKNNQuery<NORM>();

			virtual std::list<KNNCandidate>& query(const ZCurveEntry& targetPoint) override;
			virtual bool recursiveKnnSearch(const ZCurveEntry& targetPoint, HyperCube& activeRange, IndexInterval datasetInterval) override;
			virtual float kDistanceEntry() override;
			virtual void addCandidate(KNNCandidate candidate) override;

		protected:
			void traverseLeftRightAsync(const ZCurveEntry& targetPoint, HyperCube& activeRange, IndexInterval datasetInterval, IndexInterval lowerInterval, IndexInterval upperInterval, unsigned int splitPosition, unsigned int splitDimension, float leftDistance, float rightDistance);
			void traverseRightLeftAsync(const ZCurveEntry& targetPoint, HyperCube& activeRange, IndexInterval datasetInterval, IndexInterval lowerInterval, IndexInterval upperInterval, unsigned int splitPosition, unsigned int splitDimension, float leftDistance, float rightDistance);
			void asyncKnnSearch( const ZCurveEntry& targetPoint, HyperCube& activeRange, IndexInterval datasetInterval, float distance );

			virtual void logQuery() override;
        public:
			ThreadPool* threadPool;
			unsigned int minimumIntervalLengthToUseParallelism;
			std::atomic<float> currentKDistanceEntry;
			std::mutex candidatesMutex;
		};

	} // namespace inpainting
} // namespace ettention