#pragma once

#include "setup/CostFunctionOptions.h"
#include "index/MaskedIndex.h"
#include "index/FilterAndRefine.h"
#include "index/PatchAccess8Bit.h"
#include "zcurve/PresortedDataSet.h"
#include "zcurve/ZCurveEntry.h"
#include "pca/PCASubspace.h"

namespace ettention
{

	class ByteVolume;
	
	namespace inpainting 
	{
		class InpaintingDebugParameters;
		class InpaintingOptimizationParameters;
		class TraversalReport;
		class NDSpaceMapping;
		class FilterStep;
		class ProgressReporter;

		class ZCurveIndex : public MaskedIndex, public Serializable
		{
		public:
			ZCurveIndex( ThreadPool* threadPool, ByteVolume* data, ByteVolume* mask, ByteVolume* dictionary, std::vector<unsigned int>& patchIds, Vec3ui patchSize, DimensionSelection::PreferredDirection preferredDirection, int unprojectedSize, int subspaceDimensions, int filterSize, int minRecursionSize, bool useParallelization, int minParallelizationSize, ProgressReporter* progress, Log* log, CostFunctionOptions* costFunctionOptions );
			ZCurveIndex( std::istream& is, ThreadPool* threadPool, ByteVolume* data, ByteVolume* mask, ByteVolume* dictionary, int filterSize, int minRecursionSize, bool useParallelization, int minParallelizationSize, ProgressReporter* progress, Log* log, CostFunctionOptions* costFunctionOptions );
			virtual ~ZCurveIndex();

			virtual std::tuple<Vec3i, float> query(Vec3i targetPatchCenter) override;
			virtual KNNQuery* getQuery(Vec3i targetPatchUpperLeft);
			virtual std::string to_string() override;

		protected:
			void initFilterAndRefine();
			void initQuery( ThreadPool* threadPool, int minRecursionSize, bool useParallelization, int minParallelizationSize, ProgressReporter* progress, Log* log);
			void sortDictionary( std::vector<ZCurveEntry>& dataPoints );
			std::vector<ZCurveEntry> getDictionaryPointSetInPrincipalSpace(PCASubspace<unsigned char>* subspace, std::vector<unsigned int>& patchIds);
			std::vector<int> extractKeys( std::list<KNNCandidate>& candidates );

		public:
			virtual void writeToStream(std::ostream& os) override;
			virtual void loadFromStream(std::istream& is) override;

		protected:
			CostFunctionOptions* costFunctionOptions;
			DimensionSelection::PreferredDirection preferredDirection;
			PCASubspace<unsigned char> *subspace;
			PresortedDataSet<ZCurveEntry>* dataset;

			ByteVolume* data;
			ByteVolume* dictionary;

			KNNQuery *knnQuery;
			FilterAndRefine* filterAndRefine;
			int subspaceDimensions;
			int filterSize;
			ProgressReporter* progress;

			Log* log;
		};

    }
}