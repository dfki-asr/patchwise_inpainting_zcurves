#include "stdafx.h"

#include "gtest/gtest.h"
#include "InpaintingTest.h"

#include "computeorder/CriminisiOrder.h"
#include "costfunction/DictionaryBasedCostFunctionKernel.h"
#include "costfunction/L2CostFunction.h"
#include "dictionary/FullDictionaryBuilder.h"
#include "dictionary/Dictionary.h"

#include "InpaintingOperator.h"
#include "Problem.h"

#include "libmmv/io/deserializer/VolumeDeserializer.h"
#include "libmmv/io/serializer/VolumeSerializer.h"
#include "libmmv/evaluation/StackComparator.h"

#include "setup/parameterset/OutputParameterSet.h"
#include "setup/IndexOptions.h"

#include "patchselection/CurveBasedPatchSelection.h"
#include "dummies/DummyPatchSelection.h"
#include "dummies/DummyPriority.h"
#include "ProgressReporter.h"

#include "setup/InpaintingDebugParameters.h"
#include "setup/InpaintingOptimizationParameters.h"
#include "setup/CostFunctionOptions.h"

#include "costfunction/LPDistance.h"

using namespace inpainting;

class UpdateStepTest: public InpaintingTest
{
public:
    void loadVolumes()
    {
        std::string data = "/data/unitTestData/updatestep/testVolumeLine32_filtered_noise.mrc";
        std::string mask = "/data/unitTestData/updatestep/mask_square_32.mrc";
        std::string dictionary = "/data/unitTestData/updatestep/testVolumeLine32_filtered_noise_dictionary.mrc";

        InpaintingTest::loadVolumes(data, mask, dictionary );
    }
};

TEST_F(UpdateStepTest, Data_And_Mask_Update)
{
    std::string updatedMaskFile = std::string(TESTDATA_DIR) + "/work/updatedMask.mrc";
    std::string updatedDataFile = std::string(TESTDATA_DIR) + "/work/updatedData.mrc";

    std::string referenceMaskFile = std::string(TESTDATA_DIR) + "/data/unitTestData/updatestep/updatedMask.mrc";
    std::string referenceDataFile = std::string(TESTDATA_DIR) + "/data/unitTestData/updatestep/updatedData.mrc";

    loadVolumes( );

    libmmv::Vec3ui centerOfSourcePatch = libmmv::Vec3ui(1, 1, 1) + (problem->patchSize / 2);
    libmmv::Vec3ui centerOfPatchToInpaint = libmmv::Vec3ui(10, 10, 10);

    DummyPriority priority( problem->mask );
    DummyPatchSelection selection;
    InpaintingDebugParameters debugParameters;
    InpaintingOptimizationParameters optimizationParameters;

    InpaintingOperator* inpainting = new InpaintingOperator( problem, &priority, &selection, &progress, false, &debugParameters, &optimizationParameters );
    inpainting->setCenterOfSourcePatch( centerOfSourcePatch );
    inpainting->updateDataAndMask( centerOfPatchToInpaint );

    writeOutVolume( problem->data, updatedDataFile );
    writeOutVolume( problem->mask, updatedMaskFile );

    delete inpainting;

    libmmv::StackComparator::assertVolumesAreEqual(updatedMaskFile, referenceMaskFile);
    libmmv::StackComparator::assertVolumesAreEqual(updatedDataFile, referenceDataFile);
}

TEST_F(UpdateStepTest, Update_With_Manually_Selected_Patch)
{
    std::string updatedMaskFile = std::string(TESTDATA_DIR) + "/work/updatedMask_AccordingToSSD.mrc";
    std::string updatedDataFile = std::string(TESTDATA_DIR) + "/work/updatedData_AccordingToSSD.mrc";

    std::string updatedMaskReferenceFile = std::string(TESTDATA_DIR) + "/data/unitTestData/updatestep/updatedMask_AccordingToSSD.mrc";
    std::string updatedDataReferenceFile = std::string(TESTDATA_DIR) + "/data/unitTestData/updatestep/updatedData_AccordingToSSD.mrc";

    std::string dataFile = "/data/unitTestData/updatestep/testVolumeLine32_black_and_white.mrc";
    std::string maskFile = "/data/unitTestData/updatestep/mask_square_32.mrc";
    std::string dictionaryFile = "/data/unitTestData/updatestep/testVolumeLine32_black_and_white.mrc";

    InpaintingTest::loadVolumes(dataFile, maskFile, dictionaryFile);

    libmmv::Vec3ui centerOfPatchToInpaint = libmmv::Vec3ui(16, 4, 25);

    DummyPriority priority( problem->mask );
    DummyPatchSelection selection;
    InpaintingDebugParameters debugParameters;
    InpaintingOptimizationParameters optimizationParameters;

    InpaintingOperator* inpainting = new InpaintingOperator(problem, &priority, &selection, &progress, false, &debugParameters, &optimizationParameters);
    inpainting->setCenterOfSourcePatch(libmmv::Vec3ui(12,17,2));
    inpainting->updateDataAndMask(centerOfPatchToInpaint);

    writeOutVolume(problem->data, updatedDataFile);
    writeOutVolume(problem->mask, updatedMaskFile);

    delete inpainting;

    libmmv::StackComparator::assertVolumesAreEqual(updatedDataFile, updatedDataReferenceFile);
    libmmv::StackComparator::assertVolumesAreEqual(updatedMaskFile, updatedMaskReferenceFile);
}

TEST_F(UpdateStepTest, Front_Progression)
{
    std::string originalFrontFile = std::string(TESTDATA_DIR) + "/work/fillfront_before_updatestep.mrc";
    std::string updatedFrontFile = std::string(TESTDATA_DIR) + "/work/fillfront_after_updatestep.mrc";

    std::string referenceBeforeUpdate = std::string(TESTDATA_DIR) + "/data/unitTestData/updatestep/fillfront_before_updatestep.mrc";
    std::string referenceFile = std::string(TESTDATA_DIR) + "/data/unitTestData/updatestep/fillfront_after_updatestep.mrc";

    loadVolumes();

    libmmv::Vec3ui centerOfPatchToInpaint = libmmv::Vec3ui(4, 3, 5);

    CriminisiOrder* priority = new CriminisiOrder( problem, &progress);
	priority->init();

    libmmv::Volume* originalFrontVolume = priority->plotComputeFrontToVolume();
    writeOutVolume(originalFrontVolume, originalFrontFile);
    delete originalFrontVolume;

    instantiateDictionary();

	L2Distance<BytePatchAccess8Bit> norm;
    DictionaryBasedCostFunctionKernel* costFunction = new L2CostFunction( problem, dictionary );

    auto dictionaryPatches = dictionary->getCompressedDictionary();

    InpaintingDebugParameters debugParameters;
    InpaintingOptimizationParameters optimizationParameters;

	IndexOptions indexOptions;
	CostFunctionOptions costFunctionOptions;

    CurveBasedPatchSelection selection(costFunction, problem->data, problem->mask, problem->dictionaryVolume, dictionaryPatches, problem->patchSize, &debugParameters, &optimizationParameters, &indexOptions, &costFunctionOptions, true, &progress);
    InpaintingOperator* inpainting = new InpaintingOperator( problem, priority, &selection, &progress, false, &debugParameters, &optimizationParameters );

    libmmv::Vec3ui centerOfSourcePatch = selection.selectCenterOfBestPatch(centerOfPatchToInpaint);
	inpainting->setCenterOfSourcePatch( centerOfSourcePatch );
    inpainting->updateDataAndMask( centerOfPatchToInpaint );
    inpainting->updatePriority( centerOfPatchToInpaint );

    libmmv::Volume* frontVolume = priority->plotComputeFrontToVolume();
    writeOutVolume(frontVolume, updatedFrontFile);
    delete frontVolume;

    delete costFunction;
    delete priority;
    delete inpainting;

    libmmv::StackComparator::assertVolumesAreEqual(originalFrontFile, referenceBeforeUpdate);
    libmmv::StackComparator::assertVolumesAreEqual(updatedFrontFile, referenceFile);
}