#include "stdafx.h"

#include <gtest/gtest.h>

#include "InpaintingTest.h"
#include "common/TestBase.h"

#include "libmmv/model/volume/Volume.h"
#include "libmmv/model/volume/ByteVolume.h"
#include "libmmv/io/deserializer/VolumeDeserializer.h"
#include "libmmv/evaluation/StackComparator.h"

#include "dictionary/FullDictionaryBuilder.h"
#include "dictionary/Dictionary.h"

#include "libCFG/setup/parameterset/OutputParameterSet.h"

#include "Problem.h"

using namespace inpainting;

void InpaintingTest::SetUp() 
{
    TestBase::SetUp();

    problem = new Problem();
    problem->patchSize = libmmv::Vec3ui(7, 7, 3);

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

void InpaintingTest::writeOutVolume(libmmv::Volume* volume, std::string filename)
{
    libCFG::OutputParameterSet parameter(libmmv::VoxelValueType::IO_VOXEL_TYPE_UNSIGNED_8 );
    libmmv::VolumeSerializer serializer;
    serializer.write(volume, filename, parameter.getVoxelType(), parameter.getOrientation() );
}

void InpaintingTest::loadVolumes(std::string data, std::string mask, std::string dictionary)
{
    std::string maskFile = std::string(TESTDATA_DIR) + mask;
    std::string dataFile = std::string(TESTDATA_DIR) + data;
    std::string dictionaryFile = std::string(TESTDATA_DIR) + dictionary;

    problem->mask = dynamic_cast<libmmv::ByteVolume*>(libmmv::VolumeDeserializer::load(maskFile, libmmv::Voxel::DataType::UCHAR_8));
    problem->data = dynamic_cast<libmmv::ByteVolume*>(libmmv::VolumeDeserializer::load(dataFile, libmmv::Voxel::DataType::UCHAR_8));
    problem->dictionaryVolume = dynamic_cast<libmmv::ByteVolume*>(libmmv::VolumeDeserializer::load(dictionaryFile, libmmv::Voxel::DataType::UCHAR_8));
}

void InpaintingTest::loadDense(std::string dense)
{
    std::string denseFile = std::string(TESTDATA_DIR) + dense;
    problem->denseScan = dynamic_cast<libmmv::ByteVolume*>(libmmv::VolumeDeserializer::load(denseFile, libmmv::Voxel::DataType::UCHAR_8));
}

void InpaintingTest::instantiateDictionary()
{
    FullDictionaryBuilder builder(problem);
    dictionary = builder.createDictionary();
    dictionary->compressDictionary();
}

void InpaintingTest::setPatchSize(libmmv::Vec3ui patchSize)
{
    problem->patchSize = patchSize;
}

void InpaintingTest::releaseVolumes()
{
    delete problem;
}