#pragma once

#include <gtest/gtest.h>

#include "libmmv/model/volume/Volume.h"

namespace ettention 
{
    class MemoryListener;
}

namespace inpainting
{
    
    class TestBase : public ::testing::Test
    {
    public:
        TestBase();
        TestBase(const std::string dataPath, const std::string inputPath, const std::string outputPath);
        virtual ~TestBase();

        virtual void SetUp() override;
        virtual void TearDown() override;

        // virtual std::string runTestReconstruction(const std::string xmlFile);

        void writeOutVolume(const std::string &filename, libmmv::Volume* volume, libmmv::CoordinateOrder order = libmmv::ORDER_XZY);

    protected:
        std::string pathToData;
        std::string pathToInput;
        std::string pathToOutput;

    private:
        unsigned int beginState;
        ettention::MemoryListener* listener;
        bool setUpCalled;
        bool tearDownCalled;
    };
}