#pragma once

#include "MaskedIndex.h"
#include "zcurve/ZCurveIndex.h"

namespace ettention
{

	namespace inpainting 
	{
		class IndexOptions;
		class ProgressReporter;

		class MultiIndex : public MaskedIndex, public Serializable
		{
		public:
			MultiIndex( ByteVolume* data, ByteVolume* mask, ByteVolume* dictionary, std::vector<unsigned int>& dictionaryPatches, Vec3i patchSize, IndexOptions* options, ProgressReporter* progress, Log* log, CostFunctionOptions* costFunctionOptions);
			MultiIndex( std::istream& is, ByteVolume* data, ByteVolume* mask, ByteVolume* dictionary, IndexOptions* options, ProgressReporter* progress, Log* log, CostFunctionOptions* costFunctionOptions);
			virtual ~MultiIndex();

			virtual std::tuple<Vec3i, float> query( Vec3i targetPatchCenter ) override;
			virtual int rateQualityOfIndexForQuery( Vec3i targetPatchCenter ) override;
			virtual Vec3i adjustTargetPatchPosition(Vec3i targetPatchCenter) override;
			virtual std::string to_string() override;

		protected:
			static Index* createIdistanceIndex(ByteVolume* dataVolume, ByteVolume* maskVolume, ByteVolume* dictionaryVolume, std::vector<unsigned int>& dictionaryPatches, Vec3i patchSize, DimensionSelection::PreferredDirection preferredDirection, IndexOptions* options, Log* log);
			static Index* createZCurveIndex( ThreadPool* threadPool, ByteVolume* dataVolume, ByteVolume* maskVolume, ByteVolume* dictionaryVolume, std::vector<unsigned int>& dictionaryPatches, Vec3i patchSize, DimensionSelection::PreferredDirection preferredDirection, IndexOptions* options, ProgressReporter* progess, Log* log, CostFunctionOptions* costFunctionOptions);
			Index* selectBestIndex( Vec3i targetPatchCenter );
			Vec3i adjustTargetPatchPosition2D(Vec3i targetPatchCenter);
			Vec3i adjustTargetPatchPosition3D(Vec3i targetPatchCenter);

		public:
			virtual void writeToStream(std::ostream& os) override;
			virtual void loadFromStream(std::istream& is) override;

		protected:
			ByteVolume* data;
			ByteVolume* dictionary;
			
			Vec3i patchSize;
			std::list<Index*> indices;
			Index* bestIndex;
			IndexOptions* options;
			ProgressReporter* progress;
			Log* log;
			ThreadPool* threadPool;
			CostFunctionOptions* costFunctionOptions;
		};

    }
}