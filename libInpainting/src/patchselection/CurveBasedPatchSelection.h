#pragma once

#include "math/Vec3.h"
#include "IndexInterval.h"
#include "PatchSelection.h"
#include "zcurve/ZCurveOperator.h" 
#include "Log.h" 

namespace ettention
{
    class FloatVolume;
    class ByteVolume; 

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
            CurveBasedPatchSelection(DictionaryBasedCostFunctionKernel* costFunction, ByteVolume* dataVolume, ByteVolume* maskVolume, ByteVolume* dictionaryVolume, std::vector<unsigned int>& dictionaryPatches, Vec3ui patchSize, InpaintingDebugParameters* debugParameterSet, InpaintingOptimizationParameters* optimizationParameters, IndexOptions* indexOptions, CostFunctionOptions* costFunctionOptions, bool shouldUseBruteForce, ProgressReporter* progress );
            virtual ~CurveBasedPatchSelection();

			virtual Vec3i adjustTargetPatchPosition(Vec3i targetPatchCenter) override;
            Vec3i selectCenterOfBestPatch(Vec3i centerOfTargetPatch) override;

			void activateBruteForceFallback() override;

        public: // public for testing purpose
            void performNearestNeighborSearch();
            void performBruteForceSearch();            
			void doubleCheckUsingBruteForce();

        protected:
			Vec3i ensureTargetPatchPositionInsideSafeRegion(Vec3i targetPatchCenter);
			IndexInterval getSearchIntervalForBruteForceSearch();

            Vec3ui findMinimumInResult();
            void outputZeroCostPosition();
            void setCenterOfTargetPatch(Vec3i centerOfTargetPatch );

		protected:
			Index* index;

            NDSpaceMapping* ndSpaceMapping;
            IndexInterval searchResult;
			std::vector<IndexInterval> dictionaryIntervals;
            int totalDataPointsSelected;
            HyperCube zeroDistanceRange;

            DictionaryBasedCostFunctionKernel* costFunction;
            std::vector<unsigned int>& dictionaryPatches;
            Vec3i patchSize;
			Vec3i centerOfTargetPatch;
            VolumeStatistics volumeStatistics;
            unsigned int bestIndexInResultVector;
			unsigned int bestIndexInDictionary;
            Vec3i bestCoordinate;
            InpaintingDebugParameters* debugParameters;
            InpaintingOptimizationParameters* optimizationParameters;
            size_t totalPatchesSelected, totalPatchesBruteForce;
            float offset, relaxedOffset;
			bool shouldWriteReport;
			Log* log;
        };
    }
}