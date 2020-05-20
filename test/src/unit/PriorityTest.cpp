#include "stdafx.h"

#include "gtest/gtest.h"
#include "common/TestBase.h"

#include "io/deserializer/VolumeDeserializer.h"
#include "io/serializer/VolumeSerializer.h"
#include "setup/parameterset/OutputParameterSet.h"
#include "evaluation/StackComparator.h"

#include "computeorder/ComputeFront.h"
#include "computeorder/ComputeFrontEntry.h"
#include "computeorder/ComputeFrontInitializer.h"
#include "computeorder/CriminisiOrder.h"
#include "computeorder/Gradient3D.h"

#include "ProgressReporter.h"
#include "Problem.h"

#include "InpaintingTest.h"

using namespace ettention;
using namespace ettention::inpainting;

class PriorityTest: public InpaintingTest
{
public:
    void loadVolumes()
    {
        std::string mask = "/data/unitTestData/computeorder/mask_square_32.mrc";
        std::string data = "/data/unitTestData/computeorder/testVolumeLine32.mrc";

        InpaintingTest::loadVolumes(data, mask, data);
    }
    
    ProgressReporter progress;
};

TEST_F(PriorityTest, Plot_Confidence_To_Volume)
{
    setPatchSize(Vec3ui(5, 5, 5));
    std::string confidenceFile = std::string(TESTDATA_DIR) + "/work/confidenceVolume.mrc";
    std::string referenceFile = std::string(TESTDATA_DIR) + "/data/unitTestData/computeorder/confidenceVolume.mrc";

    loadVolumes();

    CriminisiOrder priority( problem, &progress);
	priority.init();

    Volume* confidenceVolume = priority.plotConfidenceToVolume();
    writeOutVolume(confidenceVolume, confidenceFile);
    delete confidenceVolume;

    StackComparator::assertVolumesAreEqual(confidenceFile, referenceFile);
}

TEST_F(PriorityTest, GradientFromMask)
{
    setPatchSize(Vec3ui(5, 5, 5));
    std::string gradientFile = std::string(TESTDATA_DIR) + "/work/gradient.mrc";

    std::string maskFile = std::string(TESTDATA_DIR) + "/data/unitTestData/computeFront/mask_square_32.mrc";
    Volume* maskVolume = ettention::VolumeDeserializer::load(maskFile, Voxel::DataType::UCHAR_8);
    
    Gradient3D gradient;
    Vec3f maskGradient;

    // default case
    maskGradient = gradient.computeGradient(maskVolume, Vec3ui(10, 10, 5), true );
    ASSERT_EQ(maskGradient, Vec3f(0.0f, 0.0f, 16.0f));
    maskGradient = gradient.computeGradient(maskVolume, Vec3ui(5, 10, 15), true);
    ASSERT_EQ(maskGradient, Vec3f(16.0f, 0.0f, 0.0f));

    delete maskVolume;
 }

TEST_F(PriorityTest, Plot_Data_Term_To_Volume)
{
    setPatchSize(Vec3ui(5, 5, 5));
    std::string priorityFile = std::string(TESTDATA_DIR) + "/work/priority.mrc";
    std::string imageGradientFile = std::string(TESTDATA_DIR) + "/work/imageGradient.mrc";
    std::string maskNormalFile = std::string(TESTDATA_DIR) + "/work/maskNormal.mrc";
    std::string dataTermFile = std::string(TESTDATA_DIR) + "/work/dataTerm.mrc";
    std::string referenceFile = std::string(TESTDATA_DIR) + "/data/unitTestData/computeorder/dataTerm.mrc";

    loadVolumes();

    CriminisiOrder priority( problem, &progress);
	priority.init();

    Volume* priorityVolume = priority.plotPriorityToVolume();
    writeOutVolume(priorityVolume, priorityFile);
    delete priorityVolume;

    Volume* confidenceVolume = priority.plotDataTermToVolume();
    writeOutVolume(confidenceVolume, dataTermFile);
    delete confidenceVolume;

    StackComparator::assertVolumesAreEqual(dataTermFile, referenceFile);
}

TEST_F(PriorityTest, GradientOfPatchTest)
{
    FloatVolume* volume = new FloatVolume(Vec3ui(3, 3, 3), 0.0F);

    volume->setVoxelToValueByCoord(Vec3ui(0, 0, 0), 1.0F);
    volume->setVoxelToValueByCoord(Vec3ui(0, 1, 0), 0.0F);
    volume->setVoxelToValueByCoord(Vec3ui(0, 2, 0), 3.0F);

    volume->setVoxelToValueByCoord(Vec3ui(1, 0, 0), 2.0F);
    volume->setVoxelToValueByCoord(Vec3ui(1, 1, 0), 0.0F);
    volume->setVoxelToValueByCoord(Vec3ui(1, 2, 0), 1.0F);

    volume->setVoxelToValueByCoord(Vec3ui(2, 0, 0), 1.0F);
    volume->setVoxelToValueByCoord(Vec3ui(2, 1, 0), 0.0F);
    volume->setVoxelToValueByCoord(Vec3ui(2, 2, 0), 1.0F);

    volume->setVoxelToValueByCoord(Vec3ui(0, 0, 1), 1.0F);
    volume->setVoxelToValueByCoord(Vec3ui(0, 1, 1), 0.0F);
    volume->setVoxelToValueByCoord(Vec3ui(0, 2, 1), 2.0F);

    volume->setVoxelToValueByCoord(Vec3ui(1, 0, 1), 1.0F);
    volume->setVoxelToValueByCoord(Vec3ui(1, 1, 1), 0.0F);
    volume->setVoxelToValueByCoord(Vec3ui(1, 2, 1), 2.0F);

    volume->setVoxelToValueByCoord(Vec3ui(2, 0, 1), 3.0F);
    volume->setVoxelToValueByCoord(Vec3ui(2, 1, 1), 0.0F);
    volume->setVoxelToValueByCoord(Vec3ui(2, 2, 1), 1.0F);

    volume->setVoxelToValueByCoord(Vec3ui(0, 0, 2), 1.0F);
    volume->setVoxelToValueByCoord(Vec3ui(0, 1, 2), 0.0F);
    volume->setVoxelToValueByCoord(Vec3ui(0, 2, 2), 1.0F);

    volume->setVoxelToValueByCoord(Vec3ui(1, 0, 2), 2.0F);
    volume->setVoxelToValueByCoord(Vec3ui(1, 1, 2), 0.0F);
    volume->setVoxelToValueByCoord(Vec3ui(1, 2, 2), 1.0F);

    volume->setVoxelToValueByCoord(Vec3ui(2, 0, 2), 3.0F);
    volume->setVoxelToValueByCoord(Vec3ui(2, 1, 2), 0.0F);
    volume->setVoxelToValueByCoord(Vec3ui(2, 2, 2), 1.0F);

    Gradient3D gradient;
    Vec3f computedGradient;

    computedGradient = gradient.computeGradient(volume, Vec3i(1, 1, 1), false);
    ASSERT_EQ(computedGradient, Vec3f(-2.0f, 2.0f, 0.0f));

    computedGradient = gradient.computeGradientOfPatch(volume, Vec3i(1, 1, 1));
    ASSERT_EQ(computedGradient, Vec3f(-2.0f, 2.0f, 0.0f));

    delete volume;
}
