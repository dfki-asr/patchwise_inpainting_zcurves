#include "stdafx.h"

#include "gtest/gtest.h"

#include "common/TestBase.h"
#include "index/DimensionSelection.h"
#include "index/PatchAccess8Bit.h"
#include "InpaintingTest.h"
#include "setup/parameterset/OutputParameterSet.h"
#include "setup/IndexOptions.h"
#include "evaluation/StackComparator.h"
#include "idistance/IDistanceIndex.h"
#include "index/MaskedIndex.h"
#include "index/MultiIndex.h"
#include "io/deserializer/VolumeDeserializer.h"
#include "StatusFlags.h"
#include "dictionary/FullDictionaryBuilder.h"
#include "dictionary/Dictionary.h"
#include "patchselection/Log.h"

using namespace ettention;
using namespace ettention::inpainting;

class MultiIndexTest : public InpaintingTest
{
public:
	void SetUp() override
	{
		TestBase::SetUp();
	}

	void TearDown() override
	{
		TestBase::TearDown();
	}
};

TEST_F(MultiIndexTest, IndexPatternsAreCorrect)
{
	Vec3i patchSize(9, 9, 3);
	unsigned int fullSize = patchSize.x * patchSize.y * patchSize.z;
	unsigned int indexSize = fullSize * 6 / 10;
	indexSize = (indexSize / 3) * 3;

	for (auto direction : DimensionSelection::allIndexDirections())
	{
		auto permutation = DimensionSelection::customPermutation(patchSize, direction, indexSize);
		ByteVolume volume(patchSize, 0.0f);
		for (unsigned int i = 0; i < (unsigned int)permutation.size(); i++)
		{
			unsigned int index = permutation[i];
			volume.nativeVoxelValue(index) = 255;
		}

		std::string indexMaskFile = std::string(TESTDATA_DIR) + "/work/index_" + DimensionSelection::to_string(direction) + ".mrc";
		std::string indexMaskFileReference = std::string(TESTDATA_DIR) + "/data/multiindex/index_" + DimensionSelection::to_string(direction) + ".mrc";
		writeOutVolume(&volume, indexMaskFile);

		StackComparator::assertVolumesAreEqual(indexMaskFile, indexMaskFileReference);
	}
}

TEST_F(MultiIndexTest, MultiIndexCanSelectPatch)
{
	Vec3i patchSize(9, 9, 3);

	std::string dataFile = std::string(TESTDATA_DIR) + "/data/multiindex/data.mrc";
	ByteVolume* data = (ByteVolume*)VolumeDeserializer::load(dataFile, Voxel::DataType::UCHAR_8);

	std::string maskFile = std::string(TESTDATA_DIR) + "/data/multiindex/mask.mrc";
	ByteVolume* mask = (ByteVolume*)VolumeDeserializer::load(maskFile, Voxel::DataType::UCHAR_8);

	std::string dictionaryFile = std::string(TESTDATA_DIR) + "/data/multiindex/dictionary.mrc";
	ByteVolume* dictionary = (ByteVolume*)VolumeDeserializer::load(dictionaryFile, Voxel::DataType::UCHAR_8);

	FullDictionaryBuilder builder(mask, patchSize);
	auto patchIndices = builder.extractValidPatchIndices();

	IndexOptions indexOptions;
	Log log;
	CostFunctionOptions costFunctionOptions;
	MultiIndex index(data, mask, dictionary, patchIndices, patchSize, &indexOptions, &progress, &log, &costFunctionOptions );

	Vec3i targetPatchPosition(14, 14, 1);
	targetPatchPosition = index.adjustTargetPatchPosition(targetPatchPosition);

	ASSERT_TRUE(targetPatchPosition == Vec3i(14, 13, 1));

	auto result = index.query(targetPatchPosition);

	ASSERT_TRUE(std::get<0>(result) == Vec3i(14, 13, 1));

	delete dictionary;
	delete mask;
	delete data;
}