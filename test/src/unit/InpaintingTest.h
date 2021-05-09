#pragma once

#include "ExternalIncludes.h"
#include "gtest/gtest.h"
#include "common/TestBase.h"
#include "libmmv/math/Vec3.h"
#include "ProgressReporter.h"
#include "dictionary/Dictionary.h"

namespace libmmv
{
    class Volume;
}

namespace inpainting
{
    class Problem;

    class InpaintingTest : public TestBase
    {
    public:
        void SetUp();
        void TearDown();

        void writeOutVolume(libmmv::Volume* volume, std::string filename);
        void loadVolumes(std::string data, std::string mask, std::string dictionary);
        void loadDense(std::string dense);
        void releaseVolumes();
        void instantiateDictionary();
        void setPatchSize(libmmv::Vec3ui patchSize);

        Problem* problem;
        Dictionary* dictionary = nullptr;
        ProgressReporter progress;
    };

}
