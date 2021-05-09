#pragma once

#include "libmmv/math/Vec3.h"
#include "IndexInterval.h"
#include "PatchSelection.h"
#include "zcurve/ZCurveOperator.h" 
#include "Log.h" 

namespace libmmv
{
    class FloatVolume;
    class ByteVolume;
}

namespace inpainting {

	class Index;
	class IndexOptions;
	class Dictionary;
	class DictionaryBasedCostFunctionKernel;
    class InpaintingDebugParameters;
    class InpaintingOptimizationParameters;
	class ProgressReporter;
	class CostFunctionOptions;
    class NDSpaceMapping;

    typedef std::vector<unsigned char> Patch;

    class CurveBasedPatchSelection : public PatchSelection
    {
    public:
        CurveBasedPatchSelection(DictionaryBasedCostFunctionKernel* costFunction, libmmv::ByteVolume* dataVolume, libmmv::ByteVolume* maskVolume, libmmv::ByteVolume* dictionaryVolume, std::vector<unsigned int>& dictionaryPatches, libmmv::Vec3ui patchSize, InpaintingDebugParameters* debugParameterSet, InpaintingOptimizationParameters* optimizationParameters, IndexOptions* indexOptions, CostFunctionOptions* costFunctionOptions, bool shouldUseBruteForce, ProgressReporter* progress );
        virtual ~CurveBasedPatchSelection();

		virtual libmmv::Vec3i adjustTargetPatchPosition(libmmv::Vec3i targetPatchCenter) override;
        libmmv::Vec3i selectCenterOfBestPatch(libmmv::Vec3i centerOfTargetPatch) override;

		void activateBruteForceFallback() override;

    public: // public for testing purpose
        void performNearestNeighborSearch();
        void performBruteForceSearch();            
		void doubleCheckUsingBruteForce();

    protected:
        libmmv::Vec3i ensureTargetPatchPositionInsideSafeRegion(libmmv::Vec3i targetPatchCenter);
		IndexInterval getSearchIntervalForBruteForceSearch();

        libmmv::Vec3ui findMinimumInResult();
        void outputZeroCostPosition();
        void setCenterOfTargetPatch(libmmv::Vec3i centerOfTargetPatch );

	protected:
		Index* index;

        NDSpaceMapping* ndSpaceMapping;
        IndexInterval searchResult;
		std::vector<IndexInterval> dictionaryIntervals;
        int totalDataPointsSelected;
        HyperCube zeroDistanceRange;

        DictionaryBasedCostFunctionKernel* costFunction;
        std::vector<unsigned int>& dictionaryPatches;
        libmmv::Vec3i patchSize;
        libmmv::Vec3i centerOfTargetPatch;
        libmmv::VolumeStatistics volumeStatistics;
        unsigned int bestIndexInResultVector;
		unsigned int bestIndexInDictionary;
        libmmv::Vec3i bestCoordinate;
        InpaintingDebugParameters* debugParameters;
        InpaintingOptimizationParameters* optimizationParameters;
        size_t totalPatchesSelected, totalPatchesBruteForce;
        float offset, relaxedOffset;
		bool shouldWriteReport;
		Log* log;
    };
}
