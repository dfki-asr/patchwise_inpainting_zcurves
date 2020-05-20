#include "stdafx.h"

#include <gtest/gtest.h>

#include "common/TestBase.h"

#include "costfunction/L2CostFunction.h"
#include "dictionary/FullDictionaryBuilder.h"
#include "dictionary/Dictionary.h"

#include "computeorder/CriminisiOrder.h"
#include "computeorder/ConfidenceOrder.h"
#include "InpaintingOperator.h"

#include "patchselection/CurveBasedPatchSelection.h"
#include "setup/InpaintingDebugParameters.h"

#include "patchselection/CurveBasedPatchSelection.h"

#include "io/deserializer/VolumeDeserializer.h"
#include "io/serializer/VolumeSerializer.h"
#include "setup/parameterset/OutputParameterSet.h"
#include "setup/InpaintingDebugParameters.h"
#include "setup/IndexOptions.h"
#include "evaluation/StackComparator.h"
#include "ProgressReporter.h"
#include "Problem.h"

#include "test/DictionaryDebugOutput.h"
#include "setup/InpaintingDebugParameters.h"
#include "setup/InpaintingOptimizationParameters.h"
#include "setup/IndexOptions.h"
#include "setup/CostFunctionOptions.h"

using namespace ettention;
using namespace ettention::inpainting;

class IntegrationTest: public TestBase
{
public:
    void SetUp() override
    {
        TestBase::SetUp();
        problem = new Problem();
        problem->patchSize = Vec3ui(7, 7, 5);
    }

    void TearDown() override
    {
        delete problem;
        TestBase::TearDown();
    }

    void writeOutVolume(Volume* volume, std::string filename)
    {
        OutputParameterSet parameter;
        VolumeSerializer serializer;
        serializer.write(volume, filename, parameter.getVoxelType(), parameter.getOrientation() );
    }

    void loadVolumes(std::string mask, std::string data, std::string dictionary)
    {
        problem->mask = dynamic_cast<ByteVolume*>(ettention::VolumeDeserializer::load(mask, Voxel::DataType::UCHAR_8));

        problem->data = dynamic_cast<ByteVolume*>(ettention::VolumeDeserializer::load(data, Voxel::DataType::UCHAR_8));

        problem->dictionaryVolume = dynamic_cast<ByteVolume*>(ettention::VolumeDeserializer::load(dictionary, Voxel::DataType::UCHAR_8));
    }

    void releaseVolumes( )
    {
    }

    void performInpaintingUsingZCurve(std::string maskFile, std::string dataFile, std::string dictionaryFile, std::string inpaintedFile, std::string referenceFile, bool shouldUseBruteForce, bool regularizeConfidence)
    {
        loadVolumes(maskFile, dataFile, dictionaryFile);

        CriminisiOrder* priority = new CriminisiOrder( problem, &progress );
		priority->init();
        FullDictionaryBuilder builder(problem);
        auto dictionary = builder.createDictionary( );
        dictionary->compressDictionary();

        auto costFunction = new L2CostFunction( problem, dictionary );

        auto dictionaryPatches = dictionary->getCompressedDictionary();

        InpaintingDebugParameters debugParameters;
		debugParameters.outputDebugVolumesEveryNIterations = 1;
		InpaintingOptimizationParameters optimizationParameters;
		IndexOptions indexOptions;
		indexOptions.indexType = IndexOptions::IndexType::ZCURVE_3D;
		CostFunctionOptions costFunctionOptions;
        auto selection = new CurveBasedPatchSelection(costFunction, problem->data, problem->mask, problem->dictionaryVolume, dictionaryPatches, problem->patchSize, &debugParameters, &optimizationParameters, &indexOptions, &costFunctionOptions, shouldUseBruteForce, &progress );

        InpaintingOperator* inpainting = new InpaintingOperator(problem, priority, selection, &progress, false, &debugParameters, &optimizationParameters);
        inpainting->run();
        writeOutVolume(problem->data, inpaintedFile);

        delete inpainting;
        delete selection;
        delete costFunction;
        delete dictionary;
        delete priority;

        releaseVolumes();
    }

    Volume* costFunctionVolume;
    GPUMappedVolume* costFunctionVolumeOnGPU;

    Problem* problem;

    ProgressReporter progress;
};


TEST_F(IntegrationTest, InpaintUsingDictionary)
{
    std::string inpaintedFile = std::string(TESTDATA_DIR) + "/work/stripes_after_dictionary_inpainting.mrc";
    std::string referenceFile = std::string(TESTDATA_DIR) + "/data/unitTestData/stripes/stripes_after_dictionary_inpainting.mrc";
    
    problem->patchSize = Vec3ui(7, 7, 5);

    performInpaintingUsingZCurve(   std::string(TESTDATA_DIR) + "/data/unitTestData/stripes/mask_square_32.mrc",
                                    std::string(TESTDATA_DIR) + "/data/unitTestData/stripes/stripes.mrc",
                                    std::string(TESTDATA_DIR) + "/data/unitTestData/stripes/stripes_dictionary.mrc",
                                    inpaintedFile, referenceFile, true, false);

    StackComparator::assertVolumesAreEqual(inpaintedFile, referenceFile);
}

TEST_F(IntegrationTest, InpaintUsingZCurve)
{
    std::string inpaintedFile = std::string(TESTDATA_DIR) + "/work/stripes_after_zcurve_inpainting.mrc";
    std::string referenceFile = std::string(TESTDATA_DIR) + "/data/unitTestData/stripes/stripes_after_dictionary_inpainting.mrc";

    problem->patchSize = Vec3ui(7, 7, 5);

    performInpaintingUsingZCurve(std::string(TESTDATA_DIR) + "/data/unitTestData/stripes/mask_square_32.mrc",
                                 std::string(TESTDATA_DIR) + "/data/unitTestData/stripes/stripes.mrc",
                                 std::string(TESTDATA_DIR) + "/data/unitTestData/stripes/stripes_dictionary.mrc", 
                                 inpaintedFile, referenceFile, false, false);

    StackComparator::assertVolumesAreEqual(inpaintedFile, referenceFile);
}

TEST_F(IntegrationTest, InpaintUsingDictionary_Debug)
{
    std::string inpaintedFile = std::string(TESTDATA_DIR) + "/work/stripes_3D_inpainted_forDebugging.mrc";
    std::string referenceFile = std::string(TESTDATA_DIR) + "/data/integrationTestData/stripes_3D_inpainted_forDebugging.mrc";

    problem->patchSize = Vec3ui(3, 3, 3);

    performInpaintingUsingZCurve(   std::string(TESTDATA_DIR) + "/data/integrationTestData/stripes_3D_mask_forDebugging.mrc",
                                    std::string(TESTDATA_DIR) + "/data/integrationTestData/stripes_3D_data_forDebugging.mrc",
                                    std::string(TESTDATA_DIR) + "/data/integrationTestData/stripes_3D_dictionary_forDebugging3.mrc",
                                    inpaintedFile, referenceFile, true, false);

    StackComparator::assertVolumesAreEqual(inpaintedFile, referenceFile);
}

TEST_F(IntegrationTest, groundTruthInpainting_2D)
{
    std::string inpaintedFile = std::string(TESTDATA_DIR) + "/work/duke_2D_inpainted.mrc";
    std::string referenceFile = std::string(TESTDATA_DIR) + "/data/integrationTestData/duke_2D_perfectInpainting.mrc";

    problem->patchSize = Vec3ui(9, 9, 1);
    performInpaintingUsingZCurve(   std::string(TESTDATA_DIR) + "/data/integrationTestData/duke_2D_mask_withBorder.mrc",
                                    std::string(TESTDATA_DIR) + "/data/integrationTestData/duke_2D_data_withBorder.mrc",
                                    std::string(TESTDATA_DIR) + "/data/integrationTestData/duke_2D_dictionary_withBorder.mrc",
                                    inpaintedFile, referenceFile, true, false);

    StackComparator::assertVolumesAreEqual(inpaintedFile, referenceFile);
}

TEST_F(IntegrationTest, groundTruthInpainting_3D_big)
{
    std::string inpaintedFile = std::string(TESTDATA_DIR) + "/work/duke_3D_big_inpainted.mrc";
    std::string referenceFile = std::string(TESTDATA_DIR) + "/data/integrationTestData/duke_3D_big_perfectInpainting.mrc";

    problem->patchSize = Vec3ui(9, 9, 9);
    performInpaintingUsingZCurve(   std::string(TESTDATA_DIR) + "/data/integrationTestData/duke_3D_big_mask_withBorder.mrc",
                                    std::string(TESTDATA_DIR) + "/data/integrationTestData/duke_3D_big_data_withBorder.mrc",
                                    std::string(TESTDATA_DIR) + "/data/integrationTestData/duke_3D_big_dictionary_withBorder.mrc",
                                    inpaintedFile, referenceFile, true, false);

    StackComparator::assertVolumesAreEqual(inpaintedFile, referenceFile);
}

TEST_F(IntegrationTest, groundTruthInpainting_2D_mini)
{
    std::string inpaintedFile = std::string(TESTDATA_DIR) + "/work/data_inpainted.mrc";
    std::string referenceFile = std::string(TESTDATA_DIR) + "/data/integrationTestData/data_inpainted.mrc";

    problem->patchSize = Vec3ui(5, 5, 1);
    performInpaintingUsingZCurve(   std::string(TESTDATA_DIR) + "/data/integrationTestData/mask.mrc",
                                    std::string(TESTDATA_DIR) + "/data/integrationTestData/data.mrc",
                                    std::string(TESTDATA_DIR) + "/data/integrationTestData/dictionary.mrc",
                                    inpaintedFile, referenceFile, true, false);

    StackComparator::assertVolumesAreEqual(inpaintedFile, referenceFile);
}

TEST_F(IntegrationTest, BorderHandling)
{
    std::string inpaintedFile = std::string(TESTDATA_DIR) + "/work/stripes_after_dictionary_inpainting_border.mrc";
    std::string referenceFile = std::string(TESTDATA_DIR) + "/data/unitTestData/stripes/stripes_after_dictionary_inpainting.mrc";

    problem->patchSize = Vec3ui(7, 7, 7);

    performInpaintingUsingZCurve(   std::string(TESTDATA_DIR) + "/data/unitTestData/stripes/mask_border_32.mrc",
                                    std::string(TESTDATA_DIR) + "/data/unitTestData/stripes/stripes_border_input.mrc",
                                    std::string(TESTDATA_DIR) + "/data/unitTestData/stripes/stripes_border_dictionary3.mrc",
                                    inpaintedFile, referenceFile, true, false);

    StackComparator::assertVolumesAreEqual(inpaintedFile, referenceFile);
}