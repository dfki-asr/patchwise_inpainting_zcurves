#include "stdafx.h"

#include "ZCurveIndex.h"

#include "costfunction/L1Distance.h"
#include "costfunction/L2Distance.h"
#include "costfunction/L2DistanceWithWeight.h"

#include "zcurve/ZCurveOperator.h"
#include "zcurve/PresortedDataSet.h"
#include "zcurve/ZCurveKNNQuery.h"
#include "zcurve/ParallelKNNQuery.h"
#include "patchselection/PatchSelectionStatistics.h"
#include "patchselection/Log.h"
#include "NDSpaceMapping.h"
#include "ProgressReporter.h"

namespace inpainting
{

	ZCurveIndex::ZCurveIndex( ettention::ThreadPool* threadPool, libmmv::ByteVolume* data, libmmv::ByteVolume* mask, libmmv::ByteVolume* dictionary, std::vector<unsigned int>& patchIds, libmmv::Vec3ui patchSize, DimensionSelection::PreferredDirection preferredDirection, int unprojectedSize, int subspaceDimensions, int filterSize, int minRecursionSize, bool useParallelization, int minParallelizationSize, ProgressReporter* progress, Log* log, CostFunctionOptions* costFunctionOptions)
		: MaskedIndex( mask, patchSize, preferredDirection, unprojectedSize )
		, data(data)
		, dictionary(dictionary)
		, subspaceDimensions(subspaceDimensions), filterSize(filterSize), preferredDirection(preferredDirection), progress(progress), log(log)
		, costFunctionOptions(costFunctionOptions)
	{
		subspace = new PCASubspace<unsigned char>(data, dictionary, patchSize, permutation, patchIds, subspaceDimensions);
		progress->reportIncrementalProgress( 0.1f );
		auto dataPoints = getDictionaryPointSetInPrincipalSpace( subspace, patchIds );
		progress->reportIncrementalProgress( 0.2f );
		sortDictionary( dataPoints );
		progress->reportIncrementalProgress( 0.7f );
		dataset = new PresortedDataSet<ZCurveEntry>( dataPoints );
		initQuery(threadPool, minRecursionSize, useParallelization, minParallelizationSize, progress, log);
		initFilterAndRefine();
	}

	ZCurveIndex::ZCurveIndex( std::istream& is, ettention::ThreadPool* threadPool, libmmv::ByteVolume* data, libmmv::ByteVolume* mask, libmmv::ByteVolume* dictionary, int filterSize, int minRecursionSize, bool useParallelization, int minParallelizationSize, ProgressReporter* progress, Log* log, CostFunctionOptions* costFunctionOptions )
		: MaskedIndex( mask )
		, data(data)
		, dictionary(dictionary)
		, filterSize(filterSize), progress(progress), log(log)
		, costFunctionOptions(costFunctionOptions)
	{
		loadFromStream( is );
		initQuery(threadPool, minRecursionSize, useParallelization, minParallelizationSize, progress, log);
		initFilterAndRefine();
	}

	ZCurveIndex::~ZCurveIndex()
	{
		delete knnQuery;
		delete dataset;
		delete subspace;
	}

	std::tuple<libmmv::Vec3i, float> ZCurveIndex::query(libmmv::Vec3i targetPatchCenter )
	{
		auto targetPoint = subspace->getDataPointInPrincipalSpace( targetPatchCenter );
		log->setIndexName( to_string() );
		log->setTargetPointInPrincipalSpace(targetPoint);
		ZCurveEntry targetEntry( 0, targetPoint );
		auto candidates = knnQuery->query(targetEntry);
		return filterAndRefine->selectBestPatch( targetPatchCenter, extractKeys(candidates) );
	}

	KNNQuery* ZCurveIndex::getQuery(libmmv::Vec3i targetPatchUpperLeft)
	{
		return knnQuery;
	}

	std::string ZCurveIndex::to_string()
	{
		return DimensionSelection::to_string(preferredDirection);
	}

	void ZCurveIndex::initFilterAndRefine()
	{
		if (costFunctionOptions->getCostFunctionType() == CostFunctionOptions::CostFunctionType::L1)
			filterAndRefine = new FilterAndRefineImpl< L1Distance< BytePatchAccess8Bit > >( data, mask, dictionary, patchSize );
		else if (costFunctionOptions->getCostFunctionType() == CostFunctionOptions::CostFunctionType::L2)
			filterAndRefine = new FilterAndRefineImpl< L2Distance< BytePatchAccess8Bit > >( data, mask, dictionary, patchSize );
		else if (costFunctionOptions->getCostFunctionType() == CostFunctionOptions::CostFunctionType::L2WithWeight)
			filterAndRefine = new FilterAndRefineImpl< L2DistanceWithWeight< BytePatchAccess8Bit > >(data, mask, dictionary, patchSize);
		else
			throw std::runtime_error("illegal cost function type");
	}

	void ZCurveIndex::initQuery( ettention::ThreadPool* threadPool, int minRecursionSize, bool useParallelization, int minParallelizationSize, ProgressReporter* progress, Log* log )
	{
		if (useParallelization)
		{
			if (costFunctionOptions->getCostFunctionType() == CostFunctionOptions::CostFunctionType::L1)
				knnQuery = new ParallelKNNQuery< L1Distance< NDPoint > >(threadPool, dataset, filterSize, minRecursionSize, minParallelizationSize, log);
			else if (costFunctionOptions->getCostFunctionType() == CostFunctionOptions::CostFunctionType::L2)
				knnQuery = new ParallelKNNQuery< L2Distance< NDPoint > >(threadPool, dataset, filterSize, minRecursionSize, minParallelizationSize, log);
			else if (costFunctionOptions->getCostFunctionType() == CostFunctionOptions::CostFunctionType::L2WithWeight)
				knnQuery = new ParallelKNNQuery< L2DistanceWithWeight< NDPoint > >(threadPool, dataset, filterSize, minRecursionSize, minParallelizationSize, log);
			else
				throw std::runtime_error("illegal cost function type");
		}
		else
		{
			if (costFunctionOptions->getCostFunctionType() == CostFunctionOptions::CostFunctionType::L1)
				knnQuery = new ZCurveKNNQuery< L1Distance< NDPoint > >(dataset, filterSize, minRecursionSize, log);
			else if (costFunctionOptions->getCostFunctionType() == CostFunctionOptions::CostFunctionType::L2)
				knnQuery = new ZCurveKNNQuery< L2Distance< NDPoint > >(dataset, filterSize, minRecursionSize, log);
			else if (costFunctionOptions->getCostFunctionType() == CostFunctionOptions::CostFunctionType::L2WithWeight)
				knnQuery = new ZCurveKNNQuery< L2DistanceWithWeight< NDPoint > >(dataset, filterSize, minRecursionSize, log);
			else
				throw std::runtime_error("illegal cost function type");
		}
	}

	void ZCurveIndex::sortDictionary(std::vector<ZCurveEntry>& dataPoints)
	{
		IndexEntryLessOnZCurve less;
		std::sort(dataPoints.begin(), dataPoints.end(), std::ref(less));
	}

	std::vector<ZCurveEntry> ZCurveIndex::getDictionaryPointSetInPrincipalSpace( PCASubspace<unsigned char>* subspace, std::vector<unsigned int>& patchIds)
	{
		std::vector<ZCurveEntry> result( patchIds.size() );
		for (size_t i = 0; i < patchIds.size(); i++)
		{
			result[i].key = patchIds[i];
			result[i].value = subspace->getDictionaryPointInPrincipalSpace( patchIds[i] );
		}
		return result;
	}

	std::vector<int> ZCurveIndex::extractKeys(std::list<KNNCandidate>& candidates)
	{
		std::vector<int> keys(candidates.size() );
		int i = 0;
		for (auto it : candidates)
		{
			keys[i] = it.key;
			i++;
		}
		return keys;
	}

	void ZCurveIndex::writeToStream(std::ostream& os)
	{
		binary_write(os, "ZCurveIndex");

		binary_write(os, patchSize);
		binary_write(os, preferredDirection);
		binary_write(os, permutation);
		subspace->writeToStream(os);
		dataset->writeToStream(os);
	}

	void  ZCurveIndex::loadFromStream(std::istream& is)
	{			
		ensure_binary_read(is, "ZCurveIndex");
		binary_read(is, patchSize);
		MaskedIndex::patchSize = patchSize;
		binary_read(is, preferredDirection);
		binary_read(is, permutation);
		subspace = new PCASubspace<unsigned char>( is, data );
		dataset = new PresortedDataSet<ZCurveEntry>(is);

		initMaskAccess();
	}

} // namespace
