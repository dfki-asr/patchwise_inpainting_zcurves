#include "stdafx.h"

#include "ExternalIncludes.h"
#include "gtest/gtest.h"
#include "common/TestBase.h"

#include "libmmv/io/deserializer/VolumeDeserializer.h"
#include "libmmv/io/serializer/VolumeSerializer.h"
#include "libCFG/setup/parameterset/OutputParameterSet.h"
#include "libmmv/evaluation/StackComparator.h"

#include "computeorder/ComputeFront.h"
#include "computeorder/ComputeFrontEntry.h"
#include "computeorder/ComputeFrontInitializer.h"
#include "dummies/DummyPriority.h"
#include "ProgressReporter.h"
#include "StatusFlags.h"

#include "InpaintingTest.h"

//for Patch Match
#include "patchselection/PatchMatchPatchSelection.h"

using namespace inpainting;

class ComputeFrontTest : public InpaintingTest
{
public:

	void initFrontWith6Values()
	{
		front.addEntry(libmmv::Vec3ui(1, 2, 3), 1.0f);
		front.addEntry(libmmv::Vec3ui(1, 2, 4), 0.8f);
		front.addEntry(libmmv::Vec3ui(1, 2, 5), 0.8f);
		front.addEntry(libmmv::Vec3ui(1, 2, 6), 1.4f);
		front.addEntry(libmmv::Vec3ui(1, 2, 7), 0.8f);
		front.addEntry(libmmv::Vec3ui(1, 2, 8), 0.9f);
	}

	ComputeFront front;
	ProgressReporter progress;
};

TEST_F(ComputeFrontTest, ComputeFrontEntryCompare_Handle_Priority)
{
	CompareByPriority compare;
	ComputeFrontEntry a = ComputeFrontEntry(libmmv::Vec3ui(1, 1, 1), 1.0);
	ComputeFrontEntry b = ComputeFrontEntry(libmmv::Vec3ui(1, 1, 1), 2.0);
	ASSERT_TRUE(compare(a, b));
	ASSERT_FALSE(compare(b, a));
}

TEST_F(ComputeFrontTest, ComputeFrontEntryCompare_Handle_Coordinate)
{
	CompareByPriority compare;
	ComputeFrontEntry a = ComputeFrontEntry(libmmv::Vec3ui(0, 0, 0), 1.0);
	ComputeFrontEntry b = ComputeFrontEntry(libmmv::Vec3ui(1, 1, 1), 1.0);
	ASSERT_TRUE(compare(a, b));
	ASSERT_FALSE(compare(b, a));
}

TEST_F(ComputeFrontTest, ComputeFrontEntryCompare_Handle_Equality)
{
	CompareByPriority compare;
	ComputeFrontEntry a = ComputeFrontEntry(libmmv::Vec3ui(1, 1, 1), 2.0);
	ComputeFrontEntry b = ComputeFrontEntry(libmmv::Vec3ui(1, 1, 1), 2.0);
	ASSERT_FALSE(compare(a, b));
	ASSERT_FALSE(compare(b, a));
}

TEST_F(ComputeFrontTest, ComputeFront_Constructor_NoMemoryLeak)
{
	ComputeFront* front = new ComputeFront();
	delete front;
}

TEST_F(ComputeFrontTest, ComputeFront_Add_Remove_Count_and_AccessByCoordinate)
{
	ASSERT_EQ(front.count(), 0);
	front.addEntry(libmmv::Vec3ui(1, 2, 3), 1.0f);
	ASSERT_EQ(front.count(), 1);

	front.addEntry(libmmv::Vec3ui(1, 2, 4), 0.0f);
	front.addEntry(libmmv::Vec3ui(1, 2, 5), 0.0f);
	front.addEntry(libmmv::Vec3ui(1, 2, 6), 0.0f);

	ASSERT_EQ(front.count(), 4);

	front.removeEntry(libmmv::Vec3ui(1, 2, 4));
	ASSERT_EQ(front.count(), 3);

	front.removeEntry(libmmv::Vec3ui(1, 2, 6));
	ASSERT_EQ(front.count(), 2);
}

TEST_F(ComputeFrontTest, ComputeFront_RemoveNonexistentThrows)
{
	front.addEntry(libmmv::Vec3ui(1, 2, 3), 1.0f);
	front.addEntry(libmmv::Vec3ui(1, 2, 4), 0.0f);
	front.addEntry(libmmv::Vec3ui(1, 2, 5), 0.0f);
}

TEST_F(ComputeFrontTest, ComputeFront_PopHighestPriorityEntry)
{
	initFrontWith6Values();

	ASSERT_EQ(front.count(), 6);

	ComputeFrontEntry entry = front.popEntryWithHighestPriority();
	ASSERT_EQ(front.count(), 5);

	ASSERT_EQ(entry.coordinate, libmmv::Vec3ui(1, 2, 6));
	ASSERT_EQ(entry.priority, 1.4f);
}


TEST_F(ComputeFrontTest, ComputeFront_UpdatePriorities)
{
	initFrontWith6Values();

	ASSERT_EQ(front.count(), 6);

	front.updatePriority(libmmv::Vec3ui(1, 2, 6), 0.2f);
	ASSERT_EQ(front.count(), 6);

	front.updatePriority(libmmv::Vec3ui(1, 2, 3), 0.2f);

	ComputeFrontEntry entry = front.popEntryWithHighestPriority();
	ASSERT_EQ(front.count(), 5);

	ASSERT_EQ(entry.coordinate, libmmv::Vec3ui(1, 2, 8));
	ASSERT_EQ(entry.priority, 0.9f);
}

TEST_F(ComputeFrontTest, ComputeFront_PopFromEmptyFrontThrows)
{
	ASSERT_EQ(front.count(), 0);
	ASSERT_THROW(front.popEntryWithHighestPriority(), std::runtime_error );
}

TEST_F(ComputeFrontTest, ComputeFront_PlotToVolume)
{
	std::string computeFrontFile = std::string(TESTDATA_DIR) + "/work/frontVolume6Values.mrc";
	initFrontWith6Values();
	auto volume = front.plotToVolume(libmmv::Vec3ui(32, 32, 32));
	writeOutVolume(volume, computeFrontFile);
	delete volume;

	libmmv::StackComparator::assertVolumesAreEqual(std::string(TESTDATA_DIR) + "/data/unitTestData/computeFront/frontVolume6Values.mrc", std::string(TESTDATA_DIR) + "/work/frontVolume6Values.mrc");
}

TEST_F(ComputeFrontTest, ComputeFront_Initializer)
{
	std::string inputFile = std::string(TESTDATA_DIR) + "/data/unitTestData/computeFront/mask_square_32.mrc";
	std::string gradientFile = std::string(TESTDATA_DIR) + "/work/gradientVolume.mrc";
	std::string gradientReference = std::string(TESTDATA_DIR) + "/data/unitTestData/computeFront/gradientVolume.mrc";
	std::string computeFrontFile = std::string(TESTDATA_DIR) + "/work/frontVolume.mrc";
	std::string frontReference = std::string(TESTDATA_DIR) + "/data/unitTestData/computeFront/frontVolume.mrc";

	initFrontWith6Values();

	auto maskVolume = (libmmv::ByteVolume*)libmmv::VolumeDeserializer::load(inputFile, libmmv::Voxel::DataType::UCHAR_8);

	DummyPriority priority(maskVolume);
	ComputeFrontInitializer initializer( &priority, libmmv::Vec3ui(3, 3, 3), &progress);

	auto front = initializer.generateComputeFront();

	auto computeFrontVolume = front->plotToVolume(libmmv::Vec3ui(32, 32, 32));
	writeOutVolume(computeFrontVolume, computeFrontFile);
	libmmv::StackComparator::assertVolumesAreEqual(computeFrontFile, frontReference);
	delete computeFrontVolume;

	const int expectedFillFront = 25 * 25 * 25 - 23 * 23 * 23;
	ASSERT_EQ(front->count(), expectedFillFront);

	delete front;
	delete maskVolume;
}

TEST_F(ComputeFrontTest, ComputeFront_Progress)
{
	std::string inputFile = std::string(TESTDATA_DIR) + "/data/unitTestData/computeFront/mask_square_32.mrc";
	std::string gradientFile = std::string(TESTDATA_DIR) + "/work/gradientVolume.mrc";
	std::string gradientReference = std::string(TESTDATA_DIR) + "/data/unitTestData/computeFront/gradientVolume.mrc";
	std::string computeFrontFile = std::string(TESTDATA_DIR) + "/work/frontVolume_progressed.mrc";
	std::string frontReference = std::string(TESTDATA_DIR) + "/data/unitTestData/computeFront/frontVolume_progressed.mrc";

	initFrontWith6Values();

	auto maskVolume = (libmmv::ByteVolume*)libmmv::VolumeDeserializer::load(inputFile, libmmv::Voxel::DataType::UCHAR_8);

	DummyPriority priority(maskVolume);
	ComputeFrontInitializer initializer( &priority, libmmv::Vec3ui(3, 3, 3), &progress);

	auto front = initializer.generateComputeFront();
	
	libmmv::BoundingBox3i patchRegion( libmmv::Vec3i( 4, 2, 5), libmmv::Vec3i( 9,7,10) );
	for (int z = patchRegion.getMin().z; z <= patchRegion.getMax().z; z++)
		for (int y = patchRegion.getMin().y; y <= patchRegion.getMax().y; y++)
			for (int x = patchRegion.getMin().x; x <= patchRegion.getMax().x; x++)
			{
				size_t index = maskVolume->getVoxelIndex(libmmv::Vec3i(x,y,z) );
				unsigned char status = maskVolume->nativeVoxelValue(index);
				if ( status == TARGET_REGION )
					maskVolume->setVoxelToByteValue(index, INPAINTED_REGION);
			}
	priority.progressFront(patchRegion);

	auto computeFrontVolume = front->plotToVolume(libmmv::Vec3ui(32, 32, 32));
	writeOutVolume(computeFrontVolume, computeFrontFile);
	libmmv::StackComparator::assertVolumesAreEqual(computeFrontFile, frontReference);
	delete computeFrontVolume;

	const int expectedFillFront = 25 * 25 * 25 - 23 * 23 * 23;
	ASSERT_EQ(front->count(), expectedFillFront);

	delete front;
	delete maskVolume;
}

