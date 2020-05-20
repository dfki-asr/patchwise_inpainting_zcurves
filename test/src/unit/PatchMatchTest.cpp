#include "stdafx.h"

#include "gtest/gtest.h"
#include "common/TestBase.h"

#include "costfunction/DictionaryBasedCostFunctionKernel.h"
#include "costfunction/L2CostFunction.h"
#include "dictionary/FullDictionaryBuilder.h"
#include "dictionary/Dictionary.h"

#include "computeorder/CriminisiOrder.h"
#include "computeorder/ConfidenceOrder.h"
#include "InpaintingOperator.h"

#include "patchselection/PatchMatchPatchSelection.h"
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

class IntegrationTest : public TestBase
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

	//assumes that image and dictionary are the same
	void loadVolumes(std::string mask, std::string data)
	{
		problem->mask = dynamic_cast<ByteVolume*>(ettention::VolumeDeserializer::load(mask, Voxel::DataType::UCHAR_8));
		problem->data = dynamic_cast<ByteVolume*>(ettention::VolumeDeserializer::load(data, Voxel::DataType::UCHAR_8));
		problem->dictionaryVolume = dynamic_cast<ByteVolume*>(ettention::VolumeDeserializer::load(data, Voxel::DataType::UCHAR_8));
		problem->dictionaryMask = dynamic_cast<ByteVolume*>(ettention::VolumeDeserializer::load(mask, Voxel::DataType::UCHAR_8));
	}

	void computeReliableMaskUsingPatchMatch(std::string maskFile, std::string dataFile, std::string reliableMaskFile)
	{
		loadVolumes(maskFile, dataFile);

		CriminisiOrder* priority = new CriminisiOrder( problem, &progress);
		priority->init();
		FullDictionaryBuilder builder(problem);
		auto dictionary = builder.createDictionary();
		dictionary->compressDictionary();

		L2Distance<BytePatchAccess8Bit> norm;
		L2CostFunction* costFunction = new L2CostFunction( problem, dictionary );

		auto dictionaryPatches = dictionary->getCompressedDictionary();

		InpaintingDebugParameters debugParameters;
		debugParameters.outputDebugVolumesEveryNIterations = 1;
		InpaintingOptimizationParameters optimizationParameters;
		IndexOptions indexOptions;
		indexOptions.indexType = IndexOptions::IndexType::PATCH_MATCH;
		CostFunctionOptions costFunctionOptions;
		auto selection = new PatchMatchPatchSelection(costFunction, problem->data, problem->mask, problem->dictionaryVolume, dictionaryPatches, problem->patchSize, &debugParameters, &optimizationParameters, &indexOptions, &costFunctionOptions, false, &progress);

		selection->saveReliableMaskStatusToVolume(reliableMaskFile);

		delete selection;
		delete costFunction;
		delete dictionary;
		delete priority;
	}

	Problem* problem;
	ProgressReporter progress;
};


TEST_F(IntegrationTest, InpaintUsingDictionary)
{
	std::string inputData = std::string(TESTDATA_DIR) + "/data/patchMatch/data.mrc";
	std::string inputMask = std::string(TESTDATA_DIR) + "/data/patchMatch/mask.mrc";
	std::string reliableMaskFile = std::string(TESTDATA_DIR) + "/data/patchMatch/reliableMaskStatus.mrc";
	std::string outputFile = std::string(TESTDATA_DIR) + "/work/reliableMaskStatus.mrc";

	problem->patchSize = Vec3ui(9, 9, 3);

	computeReliableMaskUsingPatchMatch(inputMask, inputData, outputFile);

	StackComparator::assertVolumesAreEqual(reliableMaskFile, outputFile);
}