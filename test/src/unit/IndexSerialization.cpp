#include "stdafx.h"

#include "gtest/gtest.h"

#include "common/TestBase.h"
#include "parallel/ThreadPool.h"
#include "libmmv/algorithm/Coordinates.h"
#include "libmmv/io/deserializer/VolumeDeserializer.h"

#include "pca/PCASubspace.h"
#include "index/DimensionSelection.h"
#include "index/MultiIndex.h"
#include "dictionary/FullDictionaryBuilder.h"
#include "ProgressReporter.h"
#include "patchselection/Log.h"
#include "zcurve/ZCurveIndex.h"
#include "setup/IndexOptions.h"

using namespace inpainting;

class IndexSerialization: public TestBase
{
public:
    void SetUp() override
    {
        TestBase::SetUp();
        workDirectory = std::string(TESTDATA_DIR) + "/work/";
        dataDirectory = std::string(TESTDATA_DIR) + "/data/";        
        patchSize = libmmv::Vec3ui(5, 5, 1);
    }

    void TearDown() override
    {
        TestBase::TearDown();
    }

	libmmv::ByteVolume* loadByteVolume(std::string filename)
    {
        auto volume = (libmmv::ByteVolume*) libmmv::VolumeDeserializer::load(filename, libmmv::Voxel::DataType::UCHAR_8);
        return volume;
    }

	libmmv::Vec3ui patchSize;
    std::string workDirectory;
    std::string dataDirectory;
};

TEST_F(IndexSerialization, WriteThenReadPCASubspace)
{
	auto data       = loadByteVolume(dataDirectory + "unitTestData/dotFileGeneration/input_data_16_reduced.mrc");
	auto dictionary = loadByteVolume(dataDirectory + "unitTestData/dotFileGeneration/input_dictionary_16_reduced.mrc");

	auto permutation = DimensionSelection::customPermutation( patchSize, DimensionSelection::TOPLEFT, 20 );
	FullDictionaryBuilder builder( dictionary, patchSize );
	auto patchKeys = builder.extractValidPatchIndices();

	auto writtenSubspace = new PCASubspace<unsigned char>(data, dictionary, patchSize, permutation, patchKeys, 10);
	std::ofstream ofs( workDirectory + "topleft.pca", std::ofstream::binary );
	writtenSubspace->writeToStream(ofs);
	ofs.close();
	delete writtenSubspace;

	std::ifstream ifs(workDirectory + "topleft.pca", std::ofstream::binary);
	auto readSubspace = new PCASubspace<unsigned char>( ifs, data );
	ifs.close();

	delete readSubspace;

	delete data;
	delete dictionary;
}

TEST_F(IndexSerialization, WriteThenReadZCurveIndex)
{
	auto data = loadByteVolume(dataDirectory + "unitTestData/dotFileGeneration/input_data_16_reduced.mrc");
	auto mask = loadByteVolume(dataDirectory + "unitTestData/dotFileGeneration/input_mask_16_reduced.mrc");
	auto dictionary = loadByteVolume(dataDirectory + "unitTestData/dotFileGeneration/input_dictionary_16_reduced.mrc");

	auto permutation = DimensionSelection::customPermutation(patchSize, DimensionSelection::TOPLEFT, 20);
	FullDictionaryBuilder builder(dictionary, patchSize);
	auto patchKeys = builder.extractValidPatchIndices();

	ProgressReporter progress;
	Log log;
	ettention::ThreadPool threadPool( false );
	CostFunctionOptions costFunctionOptions;

	auto writtenIndex = new ZCurveIndex( &threadPool, data, mask, dictionary, patchKeys, patchSize, DimensionSelection::TOPLEFT, 20, 10, 50, 128, false, 128, &progress, &log, &costFunctionOptions );

	std::ofstream ofs(workDirectory + "topleft.zcurve", std::ofstream::binary);
	writtenIndex->writeToStream(ofs);
	ofs.close();
	delete writtenIndex;

	std::ifstream ifs(workDirectory + "topleft.zcurve", std::ofstream::binary);
	auto readIndex = new ZCurveIndex(ifs, &threadPool, data, mask, dictionary, 50, 128, false, 128, &progress, &log, &costFunctionOptions );
	ifs.close();
	delete readIndex;

	delete data;
	delete mask;
	delete dictionary;
}

TEST_F(IndexSerialization, WriteMultiIndex)
{
	auto data = loadByteVolume(dataDirectory + "unitTestData/dotFileGeneration/input_data_16_reduced.mrc");
	auto mask = loadByteVolume(dataDirectory + "unitTestData/dotFileGeneration/input_mask_16_reduced.mrc");
	auto dictionary = loadByteVolume(dataDirectory + "unitTestData/dotFileGeneration/input_dictionary_16_reduced.mrc");

	auto permutation = DimensionSelection::customPermutation(patchSize, DimensionSelection::TOPLEFT, 20);
	FullDictionaryBuilder builder(dictionary, patchSize);
	auto patchKeys = builder.extractValidPatchIndices();

	ProgressReporter progress;
	Log log;
	IndexOptions options;
	CostFunctionOptions costFunctionOptions;
	auto writtenIndex = new MultiIndex( data, mask, dictionary, patchKeys, patchSize, &options, &progress, &log, &costFunctionOptions );

	std::ofstream ofs(workDirectory + "multi.index", std::ofstream::binary);
	writtenIndex->writeToStream(ofs);
	ofs.close();
	delete writtenIndex;

	std::ifstream ifs(workDirectory + "multi.index", std::ofstream::binary);
	auto readIndex = new MultiIndex(ifs, data, mask, dictionary, &options, &progress, &log, &costFunctionOptions );
	ifs.close();
	delete readIndex;

	delete data;
	delete mask;
	delete dictionary;
}
