#include "stdafx.h"

#include "gtest/gtest.h"
#include "common/TestBase.h"

#include "libmmv/model/volume/FloatVolume.h"
#include "libmmv/io/deserializer/VolumeDeserializer.h"
#include "libmmv/io/serializer/VolumeSerializer.h"
#include "libmmv/evaluation/StackComparator.h"

#include "setup/parameterset/OutputParameterSet.h"

#include "computeorder/ComputeFront.h"
#include "computeorder/ComputeFrontEntry.h"
#include "computeorder/ComputeFrontInitializer.h"
#include "computeorder/CriminisiOrder.h"
#include "computeorder/Gradient3D.h"

#include "ProgressReporter.h"
#include "Problem.h"

#include "InpaintingTest.h"

using namespace inpainting;

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
    setPatchSize(libmmv::Vec3ui(5, 5, 5));
    std::string confidenceFile = std::string(TESTDATA_DIR) + "/work/confidenceVolume.mrc";
    std::string referenceFile = std::string(TESTDATA_DIR) + "/data/unitTestData/computeorder/confidenceVolume.mrc";

    loadVolumes();

    CriminisiOrder priority( problem, &progress);
	priority.init();

    libmmv::Volume* confidenceVolume = priority.plotConfidenceToVolume();
    writeOutVolume(confidenceVolume, confidenceFile);
    delete confidenceVolume;

    libmmv::StackComparator::assertVolumesAreEqual(confidenceFile, referenceFile);
}

TEST_F(PriorityTest, GradientFromMask)
{
    setPatchSize(libmmv::Vec3ui(5, 5, 5));
    std::string gradientFile = std::string(TESTDATA_DIR) + "/work/gradient.mrc";

    std::string maskFile = std::string(TESTDATA_DIR) + "/data/unitTestData/computeFront/mask_square_32.mrc";
    libmmv::Volume* maskVolume = libmmv::VolumeDeserializer::load(maskFile, libmmv::Voxel::DataType::UCHAR_8);
    
    Gradient3D gradient;
    libmmv::Vec3f maskGradient;

    // default case
    maskGradient = gradient.computeGradient(maskVolume, libmmv::Vec3ui(10, 10, 5), true );
    ASSERT_EQ(maskGradient, libmmv::Vec3f(0.0f, 0.0f, 16.0f));
    maskGradient = gradient.computeGradient(maskVolume, libmmv::Vec3ui(5, 10, 15), true);
    ASSERT_EQ(maskGradient, libmmv::Vec3f(16.0f, 0.0f, 0.0f));

    delete maskVolume;
 }

TEST_F(PriorityTest, Plot_Data_Term_To_Volume)
{
    setPatchSize(libmmv::Vec3ui(5, 5, 5));
    std::string priorityFile = std::string(TESTDATA_DIR) + "/work/priority.mrc";
    std::string imageGradientFile = std::string(TESTDATA_DIR) + "/work/imageGradient.mrc";
    std::string maskNormalFile = std::string(TESTDATA_DIR) + "/work/maskNormal.mrc";
    std::string dataTermFile = std::string(TESTDATA_DIR) + "/work/dataTerm.mrc";
    std::string referenceFile = std::string(TESTDATA_DIR) + "/data/unitTestData/computeorder/dataTerm.mrc";

    loadVolumes();

    CriminisiOrder priority( problem, &progress);
	priority.init();

    libmmv::Volume* priorityVolume = priority.plotPriorityToVolume();
    writeOutVolume(priorityVolume, priorityFile);
    delete priorityVolume;

    libmmv::Volume* confidenceVolume = priority.plotDataTermToVolume();
    writeOutVolume(confidenceVolume, dataTermFile);
    delete confidenceVolume;

    libmmv::StackComparator::assertVolumesAreEqual(dataTermFile, referenceFile);
}

TEST_F(PriorityTest, GradientOfPatchTest)
{
    libmmv::FloatVolume* volume = new libmmv::FloatVolume(libmmv::Vec3ui(3, 3, 3), 0.0F);

    volume->setVoxelToValueByCoord(libmmv::Vec3ui(0, 0, 0), 1.0F);
    volume->setVoxelToValueByCoord(libmmv::Vec3ui(0, 1, 0), 0.0F);
    volume->setVoxelToValueByCoord(libmmv::Vec3ui(0, 2, 0), 3.0F);

    volume->setVoxelToValueByCoord(libmmv::Vec3ui(1, 0, 0), 2.0F);
    volume->setVoxelToValueByCoord(libmmv::Vec3ui(1, 1, 0), 0.0F);
    volume->setVoxelToValueByCoord(libmmv::Vec3ui(1, 2, 0), 1.0F);

    volume->setVoxelToValueByCoord(libmmv::Vec3ui(2, 0, 0), 1.0F);
    volume->setVoxelToValueByCoord(libmmv::Vec3ui(2, 1, 0), 0.0F);
    volume->setVoxelToValueByCoord(libmmv::Vec3ui(2, 2, 0), 1.0F);

    volume->setVoxelToValueByCoord(libmmv::Vec3ui(0, 0, 1), 1.0F);
    volume->setVoxelToValueByCoord(libmmv::Vec3ui(0, 1, 1), 0.0F);
    volume->setVoxelToValueByCoord(libmmv::Vec3ui(0, 2, 1), 2.0F);

    volume->setVoxelToValueByCoord(libmmv::Vec3ui(1, 0, 1), 1.0F);
    volume->setVoxelToValueByCoord(libmmv::Vec3ui(1, 1, 1), 0.0F);
    volume->setVoxelToValueByCoord(libmmv::Vec3ui(1, 2, 1), 2.0F);

    volume->setVoxelToValueByCoord(libmmv::Vec3ui(2, 0, 1), 3.0F);
    volume->setVoxelToValueByCoord(libmmv::Vec3ui(2, 1, 1), 0.0F);
    volume->setVoxelToValueByCoord(libmmv::Vec3ui(2, 2, 1), 1.0F);

    volume->setVoxelToValueByCoord(libmmv::Vec3ui(0, 0, 2), 1.0F);
    volume->setVoxelToValueByCoord(libmmv::Vec3ui(0, 1, 2), 0.0F);
    volume->setVoxelToValueByCoord(libmmv::Vec3ui(0, 2, 2), 1.0F);

    volume->setVoxelToValueByCoord(libmmv::Vec3ui(1, 0, 2), 2.0F);
    volume->setVoxelToValueByCoord(libmmv::Vec3ui(1, 1, 2), 0.0F);
    volume->setVoxelToValueByCoord(libmmv::Vec3ui(1, 2, 2), 1.0F);

    volume->setVoxelToValueByCoord(libmmv::Vec3ui(2, 0, 2), 3.0F);
    volume->setVoxelToValueByCoord(libmmv::Vec3ui(2, 1, 2), 0.0F);
    volume->setVoxelToValueByCoord(libmmv::Vec3ui(2, 2, 2), 1.0F);

    Gradient3D gradient;
    libmmv::Vec3f computedGradient;

    computedGradient = gradient.computeGradient(volume, libmmv::Vec3i(1, 1, 1), false);
    ASSERT_EQ(computedGradient, libmmv::Vec3f(-2.0f, 2.0f, 0.0f));

    computedGradient = gradient.computeGradientOfPatch(volume, libmmv::Vec3i(1, 1, 1));
    ASSERT_EQ(computedGradient, libmmv::Vec3f(-2.0f, 2.0f, 0.0f));

    delete volume;
}
