#include "stdafx.h"

#include <gtest/gtest.h>

#include "InpaintingTest.h"
#include "common/TestBase.h"
#include "model/volume/Volume.h"
#include "model/volume/ByteVolume.h"

#include "dictionary/FullDictionaryBuilder.h"
#include "dictionary/Dictionary.h"

#include "io/deserializer/VolumeDeserializer.h"
#include "setup/parameterset/OutputParameterSet.h"
#include "evaluation/StackComparator.h"

#include "Problem.h"

using namespace ettention;
using namespace ettention::inpainting;

void InpaintingTest::SetUp() 
{
    TestBase::SetUp();

    problem = new Problem();
    problem->patchSize = Vec3ui(7, 7, 3);

    problem->mask = nullptr;
    problem->data = nullptr;
    problem->dictionaryVolume = nullptr;
}

void InpaintingTest::TearDown() 
{
    releaseVolumes();
    if( dictionary != nullptr )
        delete dictionary;
    TestBase::TearDown();
}

void InpaintingTest::writeOutVolume(Volume* volume, std::string filename)
{
    OutputParameterSet parameter( VoxelValueType::IO_VOXEL_TYPE_UNSIGNED_8 );
    VolumeSerializer serializer;
    serializer.write(volume, filename, parameter.getVoxelType(), parameter.getOrientation() );
}

void InpaintingTest::loadVolumes(std::string data, std::string mask, std::string dictionary)
{
    std::string maskFile = std::string(TESTDATA_DIR) + mask;
    std::string dataFile = std::string(TESTDATA_DIR) + data;
    std::string dictionaryFile = std::string(TESTDATA_DIR) + dictionary;

    problem->mask = dynamic_cast<ByteVolume*>(ettention::VolumeDeserializer::load(maskFile, Voxel::DataType::UCHAR_8));
    problem->data = dynamic_cast<ByteVolume*>(ettention::VolumeDeserializer::load(dataFile, Voxel::DataType::UCHAR_8));
    problem->dictionaryVolume = dynamic_cast<ByteVolume*>(ettention::VolumeDeserializer::load(dictionaryFile, Voxel::DataType::UCHAR_8));
}

void InpaintingTest::loadDense(std::string dense)
{
    std::string denseFile = std::string(TESTDATA_DIR) + dense;
    problem->denseScan = dynamic_cast<ByteVolume*>(ettention::VolumeDeserializer::load(denseFile, Voxel::DataType::UCHAR_8));
}

void InpaintingTest::instantiateDictionary()
{
    FullDictionaryBuilder builder(problem);
    dictionary = builder.createDictionary();
    dictionary->compressDictionary();
}

void InpaintingTest::setPatchSize(Vec3ui patchSize)
{
    problem->patchSize = patchSize;
}

void InpaintingTest::releaseVolumes()
{
    delete problem;
}