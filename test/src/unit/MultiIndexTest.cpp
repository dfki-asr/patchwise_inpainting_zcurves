#include "stdafx.h"

#include "gtest/gtest.h"

#include "common/TestBase.h"
#include "index/DimensionSelection.h"
#include "index/PatchAccess8Bit.h"
#include "InpaintingTest.h"
#include "setup/parameterset/OutputParameterSet.h"
#include "setup/IndexOptions.h"
#include "libmmv/evaluation/StackComparator.h"
#include "idistance/IDistanceIndex.h"
#include "index/MaskedIndex.h"
#include "index/MultiIndex.h"
#include "libmmv/io/deserializer/VolumeDeserializer.h"
#include "StatusFlags.h"
#include "dictionary/FullDictionaryBuilder.h"
#include "dictionary/Dictionary.h"
#include "patchselection/Log.h"

using namespace inpainting;

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
	libmmv::Vec3i patchSize(9, 9, 3);
	unsigned int fullSize = patchSize.x * patchSize.y * patchSize.z;
	unsigned int indexSize = fullSize * 6 / 10;
	indexSize = (indexSize / 3) * 3;

	for (auto direction : DimensionSelection::allIndexDirections())
	{
		auto permutation = DimensionSelection::customPermutation(patchSize, direction, indexSize);
		libmmv::ByteVolume volume(patchSize, 0.0f);
		for (unsigned int i = 0; i < (unsigned int)permutation.size(); i++)
		{
			unsigned int index = permutation[i];
			volume.nativeVoxelValue(index) = 255;
		}

		std::string indexMaskFile = std::string(TESTDATA_DIR) + "/work/index_" + DimensionSelection::to_string(direction) + ".mrc";
		std::string indexMaskFileReference = std::string(TESTDATA_DIR) + "/data/multiindex/index_" + DimensionSelection::to_string(direction) + ".mrc";
		writeOutVolume(&volume, indexMaskFile);

		libmmv::StackComparator::assertVolumesAreEqual(indexMaskFile, indexMaskFileReference);
	}
}

TEST_F(MultiIndexTest, MultiIndexCanSelectPatch)
{
	libmmv::Vec3i patchSize(9, 9, 3);

	std::string dataFile = std::string(TESTDATA_DIR) + "/data/multiindex/data.mrc";
	libmmv::ByteVolume* data = (libmmv::ByteVolume*)libmmv::VolumeDeserializer::load(dataFile, libmmv::Voxel::DataType::UCHAR_8);

	std::string maskFile = std::string(TESTDATA_DIR) + "/data/multiindex/mask.mrc";
	libmmv::ByteVolume* mask = (libmmv::ByteVolume*)libmmv::VolumeDeserializer::load(maskFile, libmmv::Voxel::DataType::UCHAR_8);

	std::string dictionaryFile = std::string(TESTDATA_DIR) + "/data/multiindex/dictionary.mrc";
	libmmv::ByteVolume* dictionary = (libmmv::ByteVolume*)libmmv::VolumeDeserializer::load(dictionaryFile, libmmv::Voxel::DataType::UCHAR_8);

	FullDictionaryBuilder builder(mask, patchSize);
	auto patchIndices = builder.extractValidPatchIndices();

	IndexOptions indexOptions;
	Log log;
	CostFunctionOptions costFunctionOptions;
	MultiIndex index(data, mask, dictionary, patchIndices, patchSize, &indexOptions, &progress, &log, &costFunctionOptions );

	libmmv::Vec3i targetPatchPosition(14, 14, 1);
	targetPatchPosition = index.adjustTargetPatchPosition(targetPatchPosition);

	ASSERT_TRUE(targetPatchPosition == libmmv::Vec3i(14, 13, 1));

	auto result = index.query(targetPatchPosition);

	ASSERT_TRUE(std::get<0>(result) == libmmv::Vec3i(14, 13, 1));

	delete dictionary;
	delete mask;
	delete data;
}