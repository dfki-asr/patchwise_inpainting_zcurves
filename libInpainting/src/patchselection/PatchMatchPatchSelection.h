#pragma once

#include "math/Vec3.h"
#include "PatchSelection.h"
#include "Log.h"
#include "ProgressReporter.h"

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
		class Vec3;

        typedef std::vector<unsigned char> Patch;

        class PatchMatchPatchSelection : public PatchSelection
        {
        public:
			PatchMatchPatchSelection
			(
				DictionaryBasedCostFunctionKernel* costFunction, 
				ByteVolume* dataVolume,
				ByteVolume* maskVolume, 
				ByteVolume* dictionaryVolume, 
				std::vector<unsigned int>& dictionaryPatches, 
				Vec3ui patchSize,
				InpaintingDebugParameters* debugParameterSet, 
				InpaintingOptimizationParameters* optimizationParameters,
				IndexOptions* indexOptions, 
				CostFunctionOptions* costFunctionOptions,
				bool shouldUseBruteForce,
				ProgressReporter* progress );

            virtual ~PatchMatchPatchSelection();

			virtual Vec3i adjustTargetPatchPosition(Vec3i targetPatchCenter) override;
            Vec3i selectCenterOfBestPatch(Vec3i centerOfTargetPatch) override;

		public:
			void computeReliableSourceRegion();
			void initMaskStatus();
			bool isSourceUnreliable(Vec3ui coord);
			float computeSSD(Vec3i targetCoord, Vec3i sourceCoord);
			float computeSSDnoCheck(Vec3i targetCoord, Vec3i sourceCoord);
			void computeCostAfterIteration();
			bool initializeNewIteration();
			void doubleCheckUsingBruteForce(Vec3i targetPatchCenter);
			void setNNFofNeighboringTargetPixels(Vec3i targetCoord);

		protected:
			void propagateNearestNeighborField(Vec3i targetPatchCenter);
			float performRandomSearch(Vec3i targetPatchCenter);

			ettention::Vec3i getRandomCoordinateInsideSource();
			void initNearestNeighborField();
			void initUnreliablePixelsNNF();

			void insertUnreliableRegionPixels();
		public:
			void saveReliableMaskStatusToVolume(std::string filename);

        public:
			Vec3i ensureTargetPatchPositionInsideSafeRegion(Vec3i targetPatchCenter);

		protected:
            Vec3i patchSize;
			Vec3i centerOfTargetPatch;
            Vec3i bestCoordinate;

            InpaintingDebugParameters* debugParameters;
            InpaintingOptimizationParameters* optimizationParameters;
			Log* log;
			ProgressReporter* progress;

		protected:
			std::vector<Vec3i> nearestNeighborField;
			std::vector<Vec3i> reliableSourceCoordinates;
			std::vector<Vec3i> unreliableCoordinates;
			std::vector<float> nearestNeighborFieldCosts;

			std::vector<unsigned int> dictionaryPatches;
			std::vector<unsigned char> reliableMaskStatus;
			std::vector<unsigned char> maskStatus; //to reduce number of Volume lookups

			Vec3i resolutionInt;

			bool verbose = true;
			float totalCostOfCurrentNNF = (std::numeric_limits<float>::max)();
			float iterateIfBelowFactorInitUnreliable = 0.9F;
			float iterateIfBelowFactorInpaint = 0.9F;
        };
    }
}