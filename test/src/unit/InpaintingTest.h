#pragma once

#include "ExternalIncludes.h"
#include "gtest/gtest.h"
#include "common/TestBase.h"
#include "math/Vec3.h"
#include "ProgressReporter.h"
#include "dictionary/Dictionary.h"

using namespace ettention;
using namespace ettention::inpainting;

namespace ettention
{
    class ByteVolume;
    class FloatVolume;
    namespace inpainting
    {
        class Problem;
    }
}

class InpaintingTest: public TestBase
{
public:
    void SetUp();
    void TearDown();

    void writeOutVolume(Volume* volume, std::string filename);
    void loadVolumes(std::string data, std::string mask, std::string dictionary);
    void loadDense(std::string dense);
    void releaseVolumes();
    void instantiateDictionary();
    void setPatchSize(Vec3ui patchSize);

    Problem* problem;
    Dictionary* dictionary = nullptr;
    ProgressReporter progress;
};
