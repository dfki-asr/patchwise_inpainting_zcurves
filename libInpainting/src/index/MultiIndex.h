#pragma once

#include "libmmv/math/Vec3.h"

#include "MaskedIndex.h"
#include "zcurve/ZCurveIndex.h"

#include "parallel/Job.h"

namespace inpainting 
{
	class IndexOptions;
	class ProgressReporter;

	class IndexBuildJob : public ettention::Job
	{
	public:
		IndexBuildJob(ettention::ThreadPool* threadPool, 
					  libmmv::ByteVolume* dataVolume, 
					  libmmv::ByteVolume* maskVolume, 
					  libmmv::ByteVolume* dictionaryVolume, 
					  std::vector<unsigned int>& dictionaryPatches, 
					  libmmv::Vec3i patchSize, 
					  DimensionSelection::PreferredDirection preferredDirection, 
					  IndexOptions* options, 
					  ProgressReporter* progress, 
					  Log* log, 
					  CostFunctionOptions* costFunctionOptions);

	public:
		Index* result;

	protected:
		ettention::ThreadPool* threadPool;
		libmmv::ByteVolume* dataVolume;
		libmmv::ByteVolume* maskVolume;
		libmmv::ByteVolume* dictionaryVolume;
		std::vector<unsigned int>& dictionaryPatches;
		libmmv::Vec3i patchSize;
		DimensionSelection::PreferredDirection preferredDirection;
		IndexOptions* options;
		ProgressReporter* progress;
		Log* log;
		CostFunctionOptions* costFunctionOptions;
	};

	class ZCurveIndexBuildJob : public IndexBuildJob
	{
	public:
		ZCurveIndexBuildJob(ettention::ThreadPool* threadPool, libmmv::ByteVolume* dataVolume, libmmv::ByteVolume* maskVolume, libmmv::ByteVolume* dictionaryVolume, std::vector<unsigned int>& dictionaryPatches, libmmv::Vec3i patchSize, DimensionSelection::PreferredDirection preferredDirection, IndexOptions* options, ProgressReporter* progress, Log* log, CostFunctionOptions* costFunctionOptions);

	protected:
		virtual void performWork() override;
	};

	class IDistanceIndexBuildJob : public IndexBuildJob
	{
	public:
		IDistanceIndexBuildJob(ettention::ThreadPool* threadPool, libmmv::ByteVolume* dataVolume, libmmv::ByteVolume* maskVolume, libmmv::ByteVolume* dictionaryVolume, std::vector<unsigned int>& dictionaryPatches, libmmv::Vec3i patchSize, DimensionSelection::PreferredDirection preferredDirection, IndexOptions* options, ProgressReporter* progress, Log* log, CostFunctionOptions* costFunctionOptions);

	protected:
		virtual void performWork() override;
	};

	class MultiIndex : public MaskedIndex, public Serializable
	{
	public:
		MultiIndex(libmmv::ByteVolume* data, libmmv::ByteVolume* mask, libmmv::ByteVolume* dictionary, std::vector<unsigned int>& dictionaryPatches, libmmv::Vec3i patchSize, IndexOptions* options, ProgressReporter* progress, Log* log, CostFunctionOptions* costFunctionOptions);
		MultiIndex( std::istream& is, libmmv::ByteVolume* data, libmmv::ByteVolume* mask, libmmv::ByteVolume* dictionary, IndexOptions* options, ProgressReporter* progress, Log* log, CostFunctionOptions* costFunctionOptions);
		virtual ~MultiIndex();

		virtual std::tuple<libmmv::Vec3i, float> query(libmmv::Vec3i targetPatchCenter ) override;
		virtual int rateQualityOfIndexForQuery(libmmv::Vec3i targetPatchCenter ) override;
		virtual libmmv::Vec3i adjustTargetPatchPosition(libmmv::Vec3i targetPatchCenter) override;
		virtual std::string to_string() override;

		static Index* createIdistanceIndex(libmmv::ByteVolume* dataVolume, libmmv::ByteVolume* maskVolume, libmmv::ByteVolume* dictionaryVolume, std::vector<unsigned int>& dictionaryPatches, libmmv::Vec3i patchSize, DimensionSelection::PreferredDirection preferredDirection, IndexOptions* options, Log* log);
		static Index* createZCurveIndex( ettention::ThreadPool* threadPool, libmmv::ByteVolume* dataVolume, libmmv::ByteVolume* maskVolume, libmmv::ByteVolume* dictionaryVolume, std::vector<unsigned int>& dictionaryPatches, libmmv::Vec3i patchSize, DimensionSelection::PreferredDirection preferredDirection, IndexOptions* options, ProgressReporter* progess, Log* log, CostFunctionOptions* costFunctionOptions);

	protected:
		Index* selectBestIndex( libmmv::Vec3i targetPatchCenter );
		libmmv::Vec3i adjustTargetPatchPosition2D(libmmv::Vec3i targetPatchCenter);
		libmmv::Vec3i adjustTargetPatchPosition3D(libmmv::Vec3i targetPatchCenter);

	public:
		virtual void writeToStream(std::ostream& os) override;
		virtual void loadFromStream(std::istream& is) override;

	protected:
		libmmv::ByteVolume* data;
		libmmv::ByteVolume* dictionary;
			
		libmmv::Vec3i patchSize;
		std::list<Index*> indices;
		Index* bestIndex;
		IndexOptions* options;
		ProgressReporter* progress;
		Log* log;
		ettention::ThreadPool* threadPool;
		CostFunctionOptions* costFunctionOptions;
	};

}
