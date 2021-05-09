#include "stdafx.h"

#include "gtest/gtest.h"

#include "common/TestBase.h"

#include "libmmv/evaluation/DistanceBetweenConsecutiveSlabs.h"
#include "libmmv/evaluation/StackComparator.h"
#include "libmmv/io/serializer/VolumeSerializer.h"
#include "libmmv/io/deserializer/VolumeDeserializer.h"
#include "libmmv/algorithm/Coordinates.h"

#include "setup/parameterset/OutputParameterSet.h"

#include "ProgressReporter.h"
#include "dictionary/FullDictionaryBuilder.h"
#include "dictionary/Dictionary.h"

#include "zcurve/ZCurveOperator.h"
#include "zcurve/PresortedDataSet.h"
#include "index/PatchAccess8Bit.h"
#include "index/Index.h"
#include "patchselection/CurveBasedPatchSelection.h"
#include "test/DictionaryDebugOutput.h"
#include "setup/InpaintingDebugParameters.h"
#include "setup/InpaintingOptimizationParameters.h"
#include "index/Index.h"
#include "pca/IdentitySubspace.h"

using namespace inpainting;

class ZCurveTest: public TestBase
{
public:
    void SetUp() override
    {
        TestBase::SetUp();
        workDirectory = std::string(TESTDATA_DIR) + "/work/";
        dataDirectory = std::string(TESTDATA_DIR) + "/data/";        
        volumeResolution = libmmv::Vec3ui(7, 7, 3);
        patchSize = libmmv::Vec3ui(3, 3, 1);
    }

    void TearDown() override
    {
        TestBase::TearDown();
    }

    libmmv::ByteVolume* initVolume()
    {
        auto volume = new libmmv::ByteVolume(volumeResolution, 0.0f);

        libmmv::Vec3ui position(0, 0, 0);
        for (position.z = 0; position.z < volumeResolution.z; position.z++)
            for (position.y = 0; position.y < volumeResolution.y; position.y++)
                for (position.x = 0; position.x < volumeResolution.x; position.x++)
                {
                    size_t index = Flatten3D(position, volumeResolution);
                    volume->setVoxelToValue(index,(float) (position.x + 2 * position.y + 3 * position.z ) );
                }
        return volume;
    }

    libmmv::ByteVolume* loadByteVolume(std::string filename)
    {
        auto volume = (libmmv::ByteVolume*) libmmv::VolumeDeserializer::load(filename, libmmv::Voxel::DataType::UCHAR_8);
        volumeResolution = volume->getProperties().getVolumeResolution();
        return volume;
    }

    std::vector<unsigned int> listPossiblePatchPositionsInVolume()
    {
        std::vector<unsigned int> result;
        libmmv::Vec3ui position(0, 0, 0);
        for (position.z = patchSize.z / 2; position.z < volumeResolution.z - patchSize.z / 2; position.z++)
            for (position.y = patchSize.y / 2; position.y < volumeResolution.y - patchSize.y / 2; position.y++)
                for (position.x = patchSize.x / 2; position.x < volumeResolution.x - patchSize.x / 2; position.x++)
                {
                    unsigned int index = Flatten3D(position, volumeResolution);
                    result.push_back(index);
                }
        return result;
    }

    std::vector<unsigned int> listValidPatchPositionsInVolume()
    {
        std::vector<unsigned int> result;
        for (unsigned int i = 0; i < 5; i++)
        {
            result.push_back(Flatten3D(libmmv::Vec3ui(2, 2, i), volumeResolution));
            result.push_back(Flatten3D(libmmv::Vec3ui(6, 2, i), volumeResolution));
            result.push_back(Flatten3D(libmmv::Vec3ui(10, 2, i), volumeResolution));
            result.push_back(Flatten3D(libmmv::Vec3ui(2, 6, i), volumeResolution));
            result.push_back(Flatten3D(libmmv::Vec3ui(6, 6, i), volumeResolution));
            result.push_back(Flatten3D(libmmv::Vec3ui(10, 6, i), volumeResolution));
            result.push_back(Flatten3D(libmmv::Vec3ui(2, 10, i), volumeResolution));
            result.push_back(Flatten3D(libmmv::Vec3ui(6, 10, i), volumeResolution));
            result.push_back(Flatten3D(libmmv::Vec3ui(10, 10, i), volumeResolution));
        }
        return result;
    }

    std::vector<unsigned int> buildCompressedDictionary(libmmv::ByteVolume* dictionaryVolume)
    {
        auto progress = new ProgressReporter();
        auto dictionaryBuilder = new FullDictionaryBuilder( dictionaryVolume, patchSize );
        auto uncompressedDictionary = dictionaryBuilder->extractValidPatchPositions();
        delete dictionaryBuilder;
        delete progress;
        return Dictionary::flatten(uncompressedDictionary, dictionaryVolume->getProperties().getVolumeResolution());
    }

    float patchWiseL2NormDifference( std::vector<unsigned int> positions )
    {
        auto debugVolume = DictionaryDebugOutput::extractDictionaryToVolume( dictionaryVolume, patchSize, positions );
        float l2norm = libmmv::DistanceBetweenConsecutiveSlabs::l2NormDistance(debugVolume, 1);
        delete debugVolume;
        return l2norm;
    }

    libmmv::ByteVolume* targetVolume;
    libmmv::ByteVolume* dictionaryVolume;
    libmmv::Vec3ui volumeResolution;
    libmmv::Vec3ui patchSize;
    std::string workDirectory;
    std::string dataDirectory;
};

TEST_F(ZCurveTest, ZCurveOperator_WrongDimensions)
{
    std::vector<unsigned short> a(3);
    std::vector<unsigned short> b(2);
    ASSERT_THROW(compareRelativeToZCurve<std::vector<unsigned short>>(a, b), std::runtime_error);

    std::vector<unsigned short> c(2);
    std::vector<unsigned short> d(3);
    ASSERT_THROW(compareRelativeToZCurve<std::vector<unsigned short>>(c, d), std::runtime_error);
}

TEST_F(ZCurveTest, ZCurveOperator_Equal)
{
    std::vector<unsigned short> a = { (256 + 54)   };
    std::vector<unsigned short> b = { (256 + 54) };

    ASSERT_FALSE(compareRelativeToZCurve<std::vector<unsigned short>>( a, b ) == LESS);
    ASSERT_FALSE(compareRelativeToZCurve<std::vector<unsigned short>>( b, a ) == LESS);

    std::vector<unsigned short> c = { (256 + 54), 11, (256 + 129), 32 };
    std::vector<unsigned short> d = { (256 + 54), 11, (256 + 129), 32 };
    ASSERT_FALSE(compareRelativeToZCurve<std::vector<unsigned short>>( c, d ) == LESS);
    ASSERT_FALSE(compareRelativeToZCurve<std::vector<unsigned short>>( d, c ) == LESS);
}

TEST_F(ZCurveTest, ZCurveOperator_HighestDimensionHighestBit)
{
    std::vector<unsigned short> a = { 255*256, 0, 0, 0 };
    std::vector<unsigned short> b = { 127 * 256, 0, 0, 0 };
    ASSERT_TRUE(compareRelativeToZCurve<std::vector<unsigned short>>(b, a) == LESS);
    ASSERT_FALSE(compareRelativeToZCurve<std::vector<unsigned short>>(a, b) == LESS);

    std::vector<unsigned short> c = { 255 * 256,   0,   0,   0 };
    std::vector<unsigned short> d = { 127 * 256, 255 * 256, 255 * 256, 255 * 256 };
    ASSERT_TRUE(compareRelativeToZCurve<std::vector<unsigned short>>(b, a) == LESS);
    ASSERT_FALSE(compareRelativeToZCurve<std::vector<unsigned short>>(a, b) == LESS);
}

TEST_F(ZCurveTest, ZCurveOperator_OtherDimensionHighestBit)
{
    std::vector<unsigned short> a = { 164 * 256, 164 * 256, 255 * 256, 164 * 256 };
    std::vector<unsigned short> b = { 164 * 256, 164 * 256, 127 * 256, 164 * 256 };
    ASSERT_TRUE(compareRelativeToZCurve<std::vector<unsigned short>>(b, a) == LESS);
    ASSERT_FALSE(compareRelativeToZCurve<std::vector<unsigned short>>(a, b) == LESS);

    std::vector<unsigned short> c = { 164, 164, 127, 164 };
    std::vector<unsigned short> d = { 164, 164, 127, 164 };
    ASSERT_FALSE(compareRelativeToZCurve<std::vector<unsigned short>>(c, d) == LESS);
    ASSERT_FALSE(compareRelativeToZCurve<std::vector<unsigned short>>(d, c) == LESS);
}

TEST_F(ZCurveTest, ZCurveOperator_HighestDimension5thBit)
{
    unsigned char bit5to8 = 128 + 64 + 32 + 16;
    unsigned char bit4to8 = bit5to8 + 8;
    std::vector<unsigned short> a = { bit4to8, bit5to8, bit5to8, bit5to8 };
    std::vector<unsigned short> b = { bit5to8, bit5to8, bit5to8, bit5to8 };
    ASSERT_TRUE(compareRelativeToZCurve<std::vector<unsigned short>>(b, a) == LESS);
    ASSERT_FALSE(compareRelativeToZCurve<std::vector<unsigned short>>(a, b) == LESS);
}

TEST_F(ZCurveTest, ZCurveOperator_OtherDimension5thBit)
{
    unsigned char bit5to8 = 128 + 64 + 32 + 16;
    unsigned char bit4to8 = bit5to8 + 8;
    std::vector<unsigned short> a = { bit5to8, bit5to8, bit4to8, bit5to8 };
    std::vector<unsigned short> b = { bit5to8, bit5to8, bit5to8, bit5to8 };
    ASSERT_TRUE(compareRelativeToZCurve<std::vector<unsigned short>>(b, a) == LESS);
    ASSERT_FALSE(compareRelativeToZCurve<std::vector<unsigned short>>(a, b) == LESS);
}

TEST_F(ZCurveTest, PatchByteAccess)
{
    dictionaryVolume = initVolume();

    // patch center at 2/2/0
    unsigned int patchID = Flatten3D(libmmv::Vec3ui(2, 2, 0), volumeResolution );
    BytePatchAccess8Bit patch(dictionaryVolume, patchSize, DimensionSelection::standardPermutation( patchSize ) );
    patch.setPatchId( patchID );

    ASSERT_TRUE( patch[0] == 3 );
    ASSERT_TRUE( patch[1] == 4 );
    ASSERT_TRUE( patch[2] == 5 );
    ASSERT_TRUE( patch[3] == 5 );
    ASSERT_TRUE( patch[4] == 6 );
    ASSERT_TRUE( patch[5] == 7 );
    ASSERT_TRUE( patch[6] == 7 );
    ASSERT_TRUE( patch[7] == 8 );
    ASSERT_TRUE( patch[8] == 9 );

    delete dictionaryVolume;
}

TEST_F(ZCurveTest, SortRelativeToZCurve_775_Sum)
{
    dictionaryVolume = initVolume();
    auto positions = listPossiblePatchPositionsInVolume();

    PatchLessOnZCurve less(dictionaryVolume, patchSize, DimensionSelection::standardPermutation(patchSize) );
    std::sort( positions.begin(), positions.end(), less );

    DictionaryDebugOutput::writeDebugVolume( dictionaryVolume, patchSize, workDirectory + "zcurve/zcurve_debug.mrc", positions );
    libmmv::StackComparator::assertVolumesAreEqual(workDirectory + "zcurve/zcurve_debug.mrc", dataDirectory + "zcurve/zcurve_775_sum.mrc");
    
    delete dictionaryVolume;
}

TEST_F(ZCurveTest, SortRelativeToZCurve_Synthetic)
{
    dictionaryVolume = loadByteVolume( dataDirectory + "/zcurve/zcurve_synthetic_dictionary.mrc");
    auto positions = listValidPatchPositionsInVolume();

    DictionaryDebugOutput::writeDebugVolume( dictionaryVolume, patchSize, workDirectory + "zcurve/zcurve_synthetic_unsorted.mrc", positions);
    float unsortedL2Norm = patchWiseL2NormDifference(positions);
    std::cout << "unsorted n2norm: " << unsortedL2Norm << std::endl;

    PatchLessOnZCurve less( dictionaryVolume, patchSize, DimensionSelection::standardPermutation(patchSize) );
    std::sort(positions.begin(), positions.end(), less);

    DictionaryDebugOutput::writeDebugVolume( dictionaryVolume, patchSize, workDirectory + "zcurve/zcurve_synthetic_sorted.mrc", positions );
    float sortedL2Norm = patchWiseL2NormDifference(positions);
    std::cout << "sorted n2norm:   " << sortedL2Norm  << std::endl;
    ASSERT_TRUE( unsortedL2Norm > sortedL2Norm );
    ASSERT_TRUE( 35.0f > sortedL2Norm );

    delete dictionaryVolume;
}

TEST_F(ZCurveTest, DictionaryBuilder_SortUniform)
{
    patchSize = libmmv::Vec3ui(3, 3, 1);

    dictionaryVolume = loadByteVolume(dataDirectory + "/zcurve/uniform_patches_dictionary.mrc");

    auto compressed = buildCompressedDictionary(dictionaryVolume);
    float unsortedL2Norm = patchWiseL2NormDifference(compressed);
    std::cout << "unsorted n2norm: " << unsortedL2Norm << std::endl;
    DictionaryDebugOutput::writeDebugVolume( dictionaryVolume, patchSize, workDirectory + "zcurve/uniform_patches_unsorted.mrc", compressed);

    PatchLessOnZCurve less(dictionaryVolume, patchSize, DimensionSelection::standardPermutation(patchSize) );
    std::sort(compressed.begin(), compressed.end(), less);
    float sortedL2Norm = patchWiseL2NormDifference(compressed);
    std::cout << "sorted n2norm: " << sortedL2Norm << std::endl;

    DictionaryDebugOutput::writeDebugVolume( dictionaryVolume, patchSize,  workDirectory + "zcurve/uniform_patches_sorted.mrc", compressed);
    libmmv::StackComparator::assertVolumesAreEqual(workDirectory + "zcurve/uniform_patches_sorted.mrc", dataDirectory + "/zcurve/uniform_patches_sorted.mrc");

    ASSERT_TRUE(unsortedL2Norm > sortedL2Norm);
    ASSERT_TRUE(10.0f > sortedL2Norm);

    delete dictionaryVolume;
}

TEST_F(ZCurveTest, DictionaryBuilder_SortUniform_MultipleOccurances)
{
    patchSize = libmmv::Vec3ui(3, 3, 1);

    dictionaryVolume = loadByteVolume(dataDirectory + "/zcurve/uniform_patches_dictionary_x3.mrc");

    auto compressed = buildCompressedDictionary(dictionaryVolume);
    float unsortedL2Norm = patchWiseL2NormDifference(compressed);
    std::cout << "unsorted n2norm: " << unsortedL2Norm << std::endl;
    DictionaryDebugOutput::writeDebugVolume(dictionaryVolume, patchSize, workDirectory + "zcurve/uniform_patches_unsorted_x3.mrc", compressed);
	
    PatchLessOnZCurve less(dictionaryVolume, patchSize, DimensionSelection::standardPermutation(patchSize) );
    std::sort(compressed.begin(), compressed.end(), less);
    float sortedL2Norm = patchWiseL2NormDifference(compressed);
    std::cout << "sorted n2norm: " << sortedL2Norm << std::endl;

    DictionaryDebugOutput::writeDebugVolume(dictionaryVolume, patchSize, workDirectory + "zcurve/uniform_patches_sorted_x3.mrc", compressed);
    libmmv::StackComparator::assertVolumesAreEqual(workDirectory + "zcurve/uniform_patches_sorted_x3.mrc", dataDirectory + "/zcurve/uniform_patches_sorted_x3.mrc");

    ASSERT_TRUE(unsortedL2Norm > sortedL2Norm);
    ASSERT_TRUE(10.0f > sortedL2Norm);

    delete dictionaryVolume;
}

TEST_F(ZCurveTest, BinarySearch)
{
    dictionaryVolume = loadByteVolume(dataDirectory + "/zcurve/synthetic_search.mrc");
    libmmv::Vec3ui patchSize( 1, 2, 1 );
	// values are found in zcurv_unittest.svg
    //                                             A   B   C   D   E   F   G   H   I   J   K   L   M
    std::vector<unsigned int> dictionaryPatches { 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25 };
    auto permutation = DimensionSelection::standardPermutation(patchSize);
	IdentitySubspace<unsigned char> subspace( dictionaryVolume, dictionaryVolume, patchSize, permutation, dictionaryPatches, 2 );
	
	std::vector<ZCurveEntry> dataset( dictionaryPatches.size() );
	for (size_t i = 0; i < dictionaryPatches.size(); i++)
	{
		dataset[i] = ZCurveEntry( dictionaryPatches[i], subspace.getDictionaryPointInPrincipalSpace(dictionaryPatches[i]) );
	}

	PresortedDataSet<ZCurveEntry> search( dataset );

	NDPoint dataA_lower = { 0, 3 };
	NDPoint dataA_upper = { 7, 3 };
	auto resultA_lower = search.lower_bound(dataA_lower);
	auto resultA_upper = search.upper_bound(dataA_upper);

    GTEST_ASSERT_EQ (dictionaryPatches[resultA_lower], 14); // correct result: B-I
    GTEST_ASSERT_EQ( dictionaryPatches[resultA_upper], 21 );

	NDPoint dataB_lower = { 0, 5 };
	NDPoint dataB_upper = { 7, 5 };

	auto resultB_lower = search.lower_bound(dataB_lower);
	auto resultB_upper = search.upper_bound(dataB_upper);

    GTEST_ASSERT_EQ( dictionaryPatches[resultB_lower], 18); // correct result: F-M
    GTEST_ASSERT_EQ( dictionaryPatches[resultB_upper],  25);

    delete dictionaryVolume;
}

TEST_F(ZCurveTest, DISABLED_Index_Permutation)
{
	auto sequence = DimensionSelection::customSequence(libmmv::Vec3i(5, 5, 1), DimensionSelection::BOTTOMRIGHT);

	GTEST_ASSERT_EQ(sequence[0], libmmv::Vec3i(2, 2, 0));
	GTEST_ASSERT_EQ(sequence[3], libmmv::Vec3i(3, 3, 0));
	GTEST_ASSERT_EQ(sequence[8], libmmv::Vec3i(4, 4, 0));
	GTEST_ASSERT_EQ(sequence[24], libmmv::Vec3i(0, 0, 0));

	sequence = DimensionSelection::customSequence(libmmv::Vec3i(5, 5, 1), DimensionSelection::TOPRIGHT);

	GTEST_ASSERT_EQ(sequence[0], libmmv::Vec3i(2, 2, 0));
	GTEST_ASSERT_EQ(sequence[3], libmmv::Vec3i(3, 1, 0));
	GTEST_ASSERT_EQ(sequence[8], libmmv::Vec3i(4, 0, 0));
	GTEST_ASSERT_EQ(sequence[24], libmmv::Vec3i(0, 4, 0));

	sequence = DimensionSelection::customSequence(libmmv::Vec3i(5, 5, 1), DimensionSelection::CENTER_3D);
}