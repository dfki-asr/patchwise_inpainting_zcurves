#pragma once

#include "setup/CostFunctionOptions.h"
#include "index/MaskedIndex.h"
#include "index/FilterAndRefine.h"
#include "index/PatchAccess8Bit.h"
#include "zcurve/PresortedDataSet.h"
#include "zcurve/ZCurveEntry.h"
#include "pca/PCASubspace.h"

namespace libmmv
{
	class ByteVolume;
}
	
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
		ZCurveIndex( ettention::ThreadPool* threadPool, libmmv::ByteVolume* data, libmmv::ByteVolume* mask, libmmv::ByteVolume* dictionary, std::vector<unsigned int>& patchIds, libmmv::Vec3ui patchSize, DimensionSelection::PreferredDirection preferredDirection, int unprojectedSize, int subspaceDimensions, int filterSize, int minRecursionSize, bool useParallelization, int minParallelizationSize, ProgressReporter* progress, Log* log, CostFunctionOptions* costFunctionOptions );
		ZCurveIndex( std::istream& is, ettention::ThreadPool* threadPool, libmmv::ByteVolume* data, libmmv::ByteVolume* mask, libmmv::ByteVolume* dictionary, int filterSize, int minRecursionSize, bool useParallelization, int minParallelizationSize, ProgressReporter* progress, Log* log, CostFunctionOptions* costFunctionOptions );
		virtual ~ZCurveIndex();

		virtual std::tuple<libmmv::Vec3i, float> query(libmmv::Vec3i targetPatchCenter) override;
		virtual KNNQuery* getQuery(libmmv::Vec3i targetPatchUpperLeft);
		virtual std::string to_string() override;

	protected:
		void initFilterAndRefine();
		void initQuery(ettention::ThreadPool* threadPool, int minRecursionSize, bool useParallelization, int minParallelizationSize, ProgressReporter* progress, Log* log);
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

		libmmv::ByteVolume* data;
		libmmv::ByteVolume* dictionary;

		KNNQuery *knnQuery;
		FilterAndRefine* filterAndRefine;
		int subspaceDimensions;
		int filterSize;
		ProgressReporter* progress;

		Log* log;
	};

}
