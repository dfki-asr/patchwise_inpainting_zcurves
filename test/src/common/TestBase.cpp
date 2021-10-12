#include "stdafx.h"

#include "TestBase.h"

#include "libmmv/io/datasource/MRCDataSource.h"
#include "libmmv/io/deserializer/ImageDeserializer.h"
#include "libmmv/io/serializer/MRCWriter.h"

#include "libCFG/setup/parameterset/OutputParameterSet.h"
#include "libCFG/setup/parametersource/XMLParameterSource.h"

namespace inpainting
{

    TestBase::TestBase()
        : setUpCalled(false)
        , tearDownCalled(false)
    {
    }

    TestBase::TestBase(const std::string dataPath, const std::string inputPath, const std::string outputPath)
        : TestBase()
    {
        pathToData = dataPath;
        pathToInput = inputPath;
        pathToOutput = outputPath;
    }

    TestBase::~TestBase() 
    {
    }

    void TestBase::SetUp()
    {
        setUpCalled = true;
        const ::testing::TestInfo* const test_info = ::testing::UnitTest::GetInstance()->current_test_info();
        listener = new memoryManager::MemoryListener(test_info->test_case_name() + std::string(".") + test_info->name());
    }

    void TestBase::TearDown()
    {
        tearDownCalled = true;
        if(listener->isMemoryLeaked())
        {
            std::stringstream stream;
            listener->printSummary(stream);
            delete listener;
            FAIL() << "Memory leak detected.\n" << stream.str();
        }
        else
        {
            delete listener;
        }
    }

    void TestBase::writeOutVolume(const std::string &filename, libmmv::Volume* volume, libmmv::CoordinateOrder order /* = ORDER_XZY */)
    {
        libmmv::MRCWriter writer;
        const libCFG::OutputParameterSet format(libmmv::IO_VOXEL_TYPE_FLOAT_32, order);
        writer.write(volume, filename, format.getVoxelType(), format.getOrientation());
    }

}