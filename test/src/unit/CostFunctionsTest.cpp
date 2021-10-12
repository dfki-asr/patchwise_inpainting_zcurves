#include "stdafx.h"

#include "gtest/gtest.h"
#include "common/TestBase.h"

#include "costfunction/DictionaryBasedCostFunctionKernel.h"
#include "costfunction/L1CostFunction.h"
#include "costfunction/L2CostFunction.h"
#include "costfunction/FusingL1CostFunction.h"
#include "costfunction/FusingL2CostFunction.h"
#include "dictionary/FullDictionaryBuilder.h"
#include "dictionary/Dictionary.h"
#include "computeorder/CriminisiOrder.h"
#include "InpaintingOperator.h"

#include "patchselection/PatchSelection.h"

#include "libmmv/io/deserializer/VolumeDeserializer.h"
#include "libmmv/io/serializer/VolumeSerializer.h"
#include "libCFG/setup/parameterset/OutputParameterSet.h"
#include "libmmv/evaluation/StackComparator.h"

#include "test/DictionaryDebugOutput.h"
#include "setup/InpaintingDebugParameters.h"
#include "setup/InpaintingOptimizationParameters.h"
#include "setup/IndexOptions.h"
#include "patchselection/CurveBasedPatchSelection.h"
#include "dictionary/MaskedDictionaryBuilder.h"

#include "ProgressReporter.h"
#include "Problem.h"

#include "InpaintingTest.h"

using namespace inpainting;

class DictionaryBasedTest : public InpaintingTest
{
public:
    libmmv::Volume* costFunctionVolume;
    ProgressReporter progress;
};


TEST_F(DictionaryBasedTest, FullDictionaryBuilder)
{
    loadVolumes("/data/unitTestData/searchtree/lowres_structure.mrc", "/data/unitTestData/searchtree/mask_square_32.mrc", "/data/unitTestData/searchtree/lowres_structure.mrc");
    instantiateDictionary();

    size_t numberOfValidPatchesInTestVolume = 26 * 26 * 30;
    ASSERT_EQ(dictionary->getCompressedDictionary().size(), numberOfValidPatchesInTestVolume);
}

TEST_F(DictionaryBasedTest, DictionaryBasedCostFunctionKernel)
{
    setPatchSize(libmmv::Vec3ui(5, 5, 1));

    loadVolumes("/data/unitTestData/costFunctions/8patches_data_toFill.mrc", "/data/unitTestData/costFunctions/8patches_mask_toFill.mrc", "/data/unitTestData/costFunctions/8patches_dictionary_toFill.mrc" );
    instantiateDictionary();

    libmmv::Vec3ui patchUpperLeft = libmmv::Vec3ui(0, 0, 0);

	L2Distance<BytePatchAccess8Bit> distance;
    L2CostFunction* ssdCostFunction = new L2CostFunction( problem, dictionary );
    ssdCostFunction->setCenterOfTargetPatch(patchUpperLeft + problem->patchSize / 2);
    std::vector<IndexInterval> intervals;
    intervals.push_back(IndexInterval(0, (unsigned int) dictionary->getCompressedDictionary().size()-1));

    ssdCostFunction->computeCostForAllIntervals(intervals);

    auto& costVector = ssdCostFunction->getResult();

	const float epsilon = 1.0e-5f;
	EXPECT_NEAR(costVector[0], 0.0f,    epsilon);
	EXPECT_NEAR(costVector[1], 0.0f,    epsilon);
	EXPECT_NEAR(costVector[6], 1020.0f, epsilon);
	EXPECT_NEAR(costVector[7], 1020.0f, epsilon);

    delete ssdCostFunction;
}

TEST_F(DictionaryBasedTest, DictionaryBased_Fusing_L1)
{
    problem->patchSize = libmmv::Vec3ui(5, 5, 1);

    loadVolumes("/data/unitTestData/costFunctions/8patches_dense_data_toFill.mrc", "/data/unitTestData/costFunctions/8patches_dense_mask_toFill.mrc", "/data/unitTestData/costFunctions/8patches_dense_dictionary_toFill.mrc" );
    loadDense("/data/unitTestData/costFunctions/8patches_dense_data_toFill.mrc");
    instantiateDictionary();

    libmmv::Vec3ui patchUpperLeft = libmmv::Vec3ui(0, 0, 2);

    FusingL1CostFunction* ssdCostFunction = new FusingL1CostFunction( problem, dictionary, 0.1f );
    ssdCostFunction->setCenterOfTargetPatch(patchUpperLeft + problem->patchSize / 2);

    std::vector<IndexInterval> intervals;
    intervals.push_back(IndexInterval(0, (unsigned int)dictionary->getCompressedDictionary().size()-1));

    ssdCostFunction->computeCostForAllIntervals( intervals );

    auto& costVector = ssdCostFunction->getResult();

    ASSERT_FLOAT_EQ(costVector[0], 101.999985f  );
    ASSERT_FLOAT_EQ(costVector[1], 101.999985f  );
    ASSERT_FLOAT_EQ(costVector[2], 0.0f         );
    ASSERT_FLOAT_EQ(costVector[3], 133.874985f  );
    ASSERT_FLOAT_EQ(costVector[4], 175.312485f  );
    ASSERT_FLOAT_EQ(costVector[5], 140.249985f  );
    ASSERT_FLOAT_EQ(costVector[6], 38.249996f   );
    ASSERT_FLOAT_EQ(costVector[7], 143.437485f  );

    delete ssdCostFunction;
}

TEST_F(DictionaryBasedTest, DictionaryBased_Fusing_L2)
{
    problem->patchSize = libmmv::Vec3ui(5, 5, 1);

    loadVolumes("/data/unitTestData/costFunctions/8patches_dense_data_toFill.mrc", "/data/unitTestData/costFunctions/8patches_dense_mask_toFill.mrc", "/data/unitTestData/costFunctions/8patches_dense_dictionary_toFill.mrc" );
    loadDense("/data/unitTestData/costFunctions/8patches_dense_data_toFill.mrc");
    instantiateDictionary();

    libmmv::Vec3ui patchUpperLeft = libmmv::Vec3ui(0, 0, 2);

    FusingL2CostFunction* ssdCostFunction = new FusingL2CostFunction( problem, dictionary, 0.1f);
    ssdCostFunction->setCenterOfTargetPatch(patchUpperLeft + problem->patchSize / 2);

    std::vector<IndexInterval> intervals;
    intervals.push_back(IndexInterval(0, (unsigned int)dictionary->getCompressedDictionary().size() - 1));

    ssdCostFunction->computeCostForAllIntervals(intervals);

    auto& costVector = ssdCostFunction->getResult();

    ASSERT_FLOAT_EQ(costVector[0], 26009.9961f);
    ASSERT_FLOAT_EQ(costVector[1], 26009.9961f);
    ASSERT_FLOAT_EQ(costVector[2], 0.0f);
    ASSERT_FLOAT_EQ(costVector[3], 34138.1211f);
    ASSERT_FLOAT_EQ(costVector[4], 44704.6836f);
    ASSERT_FLOAT_EQ(costVector[5], 35763.7461f);
    ASSERT_FLOAT_EQ(costVector[6], 9753.74902f);
    ASSERT_FLOAT_EQ(costVector[7], 36576.5586f);

    delete ssdCostFunction;
}


