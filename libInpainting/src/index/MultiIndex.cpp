#include "stdafx.h"

#include "MultiIndex.h"
#include "idistance/IDistanceIndex.h"
#include "setup/IndexOptions.h"
#include "ProgressReporter.h"
#include "parallel/ThreadPool.h"

namespace inpainting
{

	IndexBuildJob::IndexBuildJob(ettention::ThreadPool* threadPool, libmmv::ByteVolume* dataVolume, libmmv::ByteVolume* maskVolume, libmmv::ByteVolume* dictionaryVolume, std::vector<unsigned int>& dictionaryPatches, libmmv::Vec3i patchSize, DimensionSelection::PreferredDirection preferredDirection, IndexOptions* options, ProgressReporter* progress, Log* log, CostFunctionOptions* costFunctionOptions)
		: ettention::Job(0)
		, threadPool(threadPool)
		, dataVolume(dataVolume)
		, maskVolume(maskVolume)
		, dictionaryVolume(dictionaryVolume)
		, dictionaryPatches(dictionaryPatches)
		, patchSize(patchSize)
		, preferredDirection(preferredDirection)
		, options(options)
		, progress(progress)
		, log(log)
		, costFunctionOptions(costFunctionOptions)
		, result(nullptr)
	{
	}

	ZCurveIndexBuildJob::ZCurveIndexBuildJob(ettention::ThreadPool* threadPool, libmmv::ByteVolume* dataVolume, libmmv::ByteVolume* maskVolume, libmmv::ByteVolume* dictionaryVolume, std::vector<unsigned int>& dictionaryPatches, libmmv::Vec3i patchSize, DimensionSelection::PreferredDirection preferredDirection, IndexOptions* options, ProgressReporter* progress, Log* log, CostFunctionOptions* costFunctionOptions)
		: IndexBuildJob(threadPool, dataVolume, maskVolume, dictionaryVolume, dictionaryPatches, patchSize, preferredDirection, options, progress, log, costFunctionOptions)
	{
	}

	void ZCurveIndexBuildJob::performWork()
	{
		result = MultiIndex::createZCurveIndex(threadPool, dataVolume, maskVolume, dictionaryVolume, dictionaryPatches, patchSize, preferredDirection, options, progress, log, costFunctionOptions );
	}

	IDistanceIndexBuildJob::IDistanceIndexBuildJob(ettention::ThreadPool* threadPool, libmmv::ByteVolume* dataVolume, libmmv::ByteVolume* maskVolume, libmmv::ByteVolume* dictionaryVolume, std::vector<unsigned int>& dictionaryPatches, libmmv::Vec3i patchSize, DimensionSelection::PreferredDirection preferredDirection, IndexOptions* options, ProgressReporter* progress, Log* log, CostFunctionOptions* costFunctionOptions)
		: IndexBuildJob(threadPool, dataVolume, maskVolume, dictionaryVolume, dictionaryPatches, patchSize, preferredDirection, options, progress, log, costFunctionOptions)
	{
	}

	void IDistanceIndexBuildJob::performWork()
	{
		result = MultiIndex::createIdistanceIndex(dataVolume, maskVolume, dictionaryVolume, dictionaryPatches, patchSize, preferredDirection, options, log );
	}

	MultiIndex::MultiIndex(libmmv::ByteVolume* data, libmmv::ByteVolume* mask, libmmv::ByteVolume* dictionary, std::vector<unsigned int>& dictionaryPatches, libmmv::Vec3i patchSize, IndexOptions* options, ProgressReporter* progress, Log* log, CostFunctionOptions* costFunctionOptions)
		: MaskedIndex( mask, patchSize, DimensionSelection::STANDARD, 0 ), options(options), patchSize(patchSize)
		, data(data), dictionary(dictionary)
		, progress( progress )
		, log(log)
	{
		threadPool = new ettention::ThreadPool( true );
		unsigned int fullSize = patchSize.x * patchSize.y * patchSize.z;
			
		std::vector< std::future<Index*> > jobs;
		switch (options->getIndexType())
		{
		case IndexOptions::IndexType::IDISTANCE_RGB:
			progress->reportTaskStart("building 4 idistance indices", 4.0f);
			threadPool->enqueue(new IDistanceIndexBuildJob(threadPool, data, mask, dictionary, dictionaryPatches, patchSize, DimensionSelection::TOPLEFT, options, progress, log, costFunctionOptions));
			threadPool->enqueue(new IDistanceIndexBuildJob(threadPool, data, mask, dictionary, dictionaryPatches, patchSize, DimensionSelection::TOPRIGHT, options, progress, log, costFunctionOptions));
			threadPool->enqueue(new IDistanceIndexBuildJob(threadPool, data, mask, dictionary, dictionaryPatches, patchSize, DimensionSelection::BOTTOMLEFT, options, progress, log, costFunctionOptions));
			threadPool->enqueue(new IDistanceIndexBuildJob(threadPool, data, mask, dictionary, dictionaryPatches, patchSize, DimensionSelection::BOTTOMRIGHT, options, progress, log, costFunctionOptions));
			break;

		case IndexOptions::IndexType::ZCURVE_RGB:
			progress->reportTaskStart("building 8 zcurve indices", 8.0f);

			for (auto direction : {
				DimensionSelection::TOPLEFT ,   DimensionSelection::TOPRIGHT,
				DimensionSelection::BOTTOMLEFT, DimensionSelection::BOTTOMRIGHT,
				DimensionSelection::TOP,        DimensionSelection::BOTTOM,
				DimensionSelection::LEFT,       DimensionSelection::RIGHT }) 
			{
				threadPool->enqueue(new ZCurveIndexBuildJob( threadPool, data, mask, dictionary, dictionaryPatches, patchSize, direction, options, progress, log, costFunctionOptions ));
			}
			break;

		case IndexOptions::IndexType::ZCURVE_3D:
			progress->reportTaskStart("building 8 zcurve indices", 8.0f);
			threadPool->enqueue(new ZCurveIndexBuildJob(threadPool, data, mask, dictionary, dictionaryPatches, patchSize, DimensionSelection::FACE00P, options, progress, log, costFunctionOptions));
			threadPool->enqueue(new ZCurveIndexBuildJob(threadPool, data, mask, dictionary, dictionaryPatches, patchSize, DimensionSelection::FACE00N, options, progress, log, costFunctionOptions));
			threadPool->enqueue(new ZCurveIndexBuildJob(threadPool, data, mask, dictionary, dictionaryPatches, patchSize, DimensionSelection::FACE0N0, options, progress, log, costFunctionOptions));
			threadPool->enqueue(new ZCurveIndexBuildJob(threadPool, data, mask, dictionary, dictionaryPatches, patchSize, DimensionSelection::FACE0P0, options, progress, log, costFunctionOptions));
			threadPool->enqueue(new ZCurveIndexBuildJob(threadPool, data, mask, dictionary, dictionaryPatches, patchSize, DimensionSelection::FACEN00, options, progress, log, costFunctionOptions));
			threadPool->enqueue(new ZCurveIndexBuildJob(threadPool, data, mask, dictionary, dictionaryPatches, patchSize, DimensionSelection::FACEP00, options, progress, log, costFunctionOptions));
			threadPool->enqueue(new ZCurveIndexBuildJob(threadPool, data, mask, dictionary, dictionaryPatches, patchSize, DimensionSelection::MIDDLE, options, progress, log, costFunctionOptions));
			break;
		}

		threadPool->waitForJobsToFinish();

		for ( auto job : threadPool->getFinishedTasks() )
		{
			auto indexBuildJob = (IndexBuildJob*) job;
			indices.push_back(indexBuildJob->result );
		}

		progress->reportTaskEnd();
	}

	MultiIndex::MultiIndex(std::istream& is, libmmv::ByteVolume* data, libmmv::ByteVolume* mask, libmmv::ByteVolume* dictionary, IndexOptions* options, ProgressReporter* progress, Log* log, CostFunctionOptions* costFunctionOptions )
		: MaskedIndex(mask)
		, data(data), dictionary(dictionary)
		, options(options)
		, progress(progress)
		, log(log) 
		, costFunctionOptions( costFunctionOptions )
	{
		threadPool = new ettention::ThreadPool( true );
		progress->reportTaskStart("loading indices", 100.0f );
		loadFromStream( is );
		progress->reportTaskEnd();
		initMaskAccess();
	}

	MultiIndex::~MultiIndex()
	{
		delete threadPool;
		for ( auto index : indices )
			delete index;
	}

	std::tuple<libmmv::Vec3i, float> MultiIndex::query(libmmv::Vec3i targetPatchCenter )
	{
		bestIndex = selectBestIndex( targetPatchCenter );
		return bestIndex->query( targetPatchCenter );
	}

	int MultiIndex::rateQualityOfIndexForQuery(libmmv::Vec3i targetPatchCenter )
	{
		bestIndex = selectBestIndex(targetPatchCenter);
		if ( bestIndex != nullptr )
			return bestIndex->rateQualityOfIndexForQuery( targetPatchCenter );
		return -1;
	}

	libmmv::Vec3i MultiIndex::adjustTargetPatchPosition(libmmv::Vec3i targetPatchCenter)
	{
		if (options->getIndexType() == IndexOptions::IndexType::ZCURVE_RGB 
			|| options->getIndexType() == IndexOptions::IndexType::IDISTANCE_RGB )
		{
			return  adjustTargetPatchPosition2D(targetPatchCenter);
		}
		else
		{
			return  adjustTargetPatchPosition3D(targetPatchCenter);
		}
	}

	std::string MultiIndex::to_string()
	{
		return "combined";
	}

	Index* MultiIndex::createIdistanceIndex(libmmv::ByteVolume* dataVolume, libmmv::ByteVolume* maskVolume, libmmv::ByteVolume* dictionaryVolume, std::vector<unsigned int>& dictionaryPatches, libmmv::Vec3i patchSize, DimensionSelection::PreferredDirection preferredDirection, IndexOptions* options, Log* log)
	{
		float patchSizeF = (float)(patchSize.x*patchSize.y*patchSize.z);
		int unprojectedSize = (int) ( patchSizeF * options->getFractionOfPixelsCoveredByEachIndex() );
		unprojectedSize = (unprojectedSize / 3) * 3;
		return new IDistanceIndex(dataVolume, maskVolume, dictionaryVolume, dictionaryPatches, patchSize, preferredDirection, unprojectedSize, options->getIndexDimensions(), options->getKNNFilterSize() );
	}

	Index* MultiIndex::createZCurveIndex( ettention::ThreadPool* threadPool, libmmv::ByteVolume* dataVolume, libmmv::ByteVolume* maskVolume, libmmv::ByteVolume* dictionaryVolume, std::vector<unsigned int>& dictionaryPatches, libmmv::Vec3i patchSize, DimensionSelection::PreferredDirection preferredDirection, IndexOptions* options, ProgressReporter* progress, Log* log, CostFunctionOptions* costFunctionOptions)
	{
		float patchSizeF = (float)(patchSize.x*patchSize.y*patchSize.z);
		int unprojectedSize = (int)(patchSizeF * options->getFractionOfPixelsCoveredByEachIndex());
		unprojectedSize = (unprojectedSize / 3) * 3;
		return new ZCurveIndex( threadPool, dataVolume, maskVolume, dictionaryVolume, dictionaryPatches, patchSize, preferredDirection, unprojectedSize, options->getIndexDimensions(), options->getKNNFilterSize(), options->getMinSizeForRecursion(), options->shouldUseParallelization(), options->getMinSizeForParallelization(), progress, log, costFunctionOptions );
	}

	Index* MultiIndex::selectBestIndex(libmmv::Vec3i targetPatchCenter )
	{
		int bestRatingSoFar = -1;
		Index* bestIndexSoFar = nullptr;
		int i = 0;
		int selectedIndexNumber = 0;
		for (auto index : indices)
		{
			int rating = index->rateQualityOfIndexForQuery( targetPatchCenter );
			if (rating > bestRatingSoFar)
			{
				bestRatingSoFar = rating;
				bestIndexSoFar = index;
				selectedIndexNumber = i;
			}
			i++;
		}
		return bestIndexSoFar;
	}

	libmmv::Vec3i MultiIndex::adjustTargetPatchPosition2D(libmmv::Vec3i targetPatchCenter)
	{
		libmmv::Vec3i displacementPatchSize = componentWiseMax(patchSize - libmmv::Vec3i(2, 2, 2), libmmv::Vec3i(1, 1, 1));
		auto shiftPermutation = DimensionSelection::customSequence(libmmv::Vec3i(displacementPatchSize.x, displacementPatchSize.y, 1), DimensionSelection::CENTER_2D);
		libmmv::Vec3i patchToProcess = targetPatchCenter;
		int i = 1;
		libmmv::Vec3i min = targetPatchCenter;
		libmmv::Vec3i max = targetPatchCenter;
		while (rateQualityOfIndexForQuery(patchToProcess) < 0)
		{
			libmmv::Vec2i offsetCenteredToZero = shiftPermutation[i].xy() - displacementPatchSize.xy() / 2;
			patchToProcess = targetPatchCenter + libmmv::Vec3i(offsetCenteredToZero.x, offsetCenteredToZero.y, 0);
			i++;
			min = componentWiseMin(min, patchToProcess);
			max = componentWiseMax(max, patchToProcess);
			if (i >= shiftPermutation.size())
			{
				throw IndexCannotHandlePositionError( targetPatchCenter, min, max );
			}
		}
		return patchToProcess;
	}

	libmmv::Vec3i MultiIndex::adjustTargetPatchPosition3D(libmmv::Vec3i targetPatchCenter)
	{
		libmmv::Vec3i displacementPatchSize = componentWiseMax( patchSize - libmmv::Vec3i(2,2,2), libmmv::Vec3i(1,1,1) );
			
		auto shiftPermutation = DimensionSelection::customSequence(displacementPatchSize, DimensionSelection::CENTER_3D);
		libmmv::Vec3i patchToProcess = targetPatchCenter;
		int i = 1;
		libmmv::Vec3i min = targetPatchCenter;
		libmmv::Vec3i max = targetPatchCenter;
		while (rateQualityOfIndexForQuery(patchToProcess) < 0)
		{
			libmmv::Vec3i offsetCenteredToZero = shiftPermutation[i] - displacementPatchSize / 2;
			patchToProcess = targetPatchCenter + offsetCenteredToZero;
			i++;
			min = componentWiseMin(min, patchToProcess);
			max = componentWiseMax(max, patchToProcess);
			if (i >= shiftPermutation.size())
			{
				std::cout << "(" << targetPatchCenter.x << "," << targetPatchCenter.y << "," << targetPatchCenter.z << ")" << std::endl;
				throw IndexCannotHandlePositionError(targetPatchCenter, min, max);
			}
		}
		return patchToProcess;
	}

	void MultiIndex::writeToStream(std::ostream& os)
	{
		binary_write(os, "MultiIndex");
		binary_write(os, patchSize);

		size_t size = indices.size();
		binary_write(os, size);

		for (auto it : indices)
			it->writeToStream( os );
	}

	void  MultiIndex::loadFromStream(std::istream& is)
	{
		ensure_binary_read(is, "MultiIndex");
		binary_read(is, patchSize);

		size_t size;
		binary_read(is, size);

		for (size_t i = 0; i < size; i++)
		{
			std::string classId;
			binary_lookahead(is, classId);
			if (classId == "ZCurveIndex")
			{
				auto index = new ZCurveIndex(is, threadPool, data, mask, dictionary, options->getKNNFilterSize(), options->getMinSizeForRecursion(), options->shouldUseParallelization(), options->getMinSizeForParallelization(), progress, log, costFunctionOptions );
				indices.push_back( index );
			}
			else
				std::ios_base::failure("unknown index type " + classId );
		}

	}
} // namespace

