#pragma once

#include "math/Vec3.h"

namespace ettention
{
    class FloatVolume;

    namespace inpainting 
    {
        class PatchSelection;
        class ComputeOrder;
        class ProgressReporter;
        class Problem;
        class InpaintingDebugParameters;
        class InpaintingOptimizationParameters;

        class InpaintingOperator
        {
        public:
            InpaintingOperator( Problem* problem, ComputeOrder* priority, PatchSelection* patchSelection, ProgressReporter* progress, bool shouldUseBlendOperation, InpaintingDebugParameters* debugParameters, InpaintingOptimizationParameters* optimizationParameters );
            virtual ~InpaintingOperator();

            void run();

            FloatVolume* getErrorVolume();

        public: // the following functions are public for testing purpose
            virtual void runOneIteration();

            virtual void inpaintPatch();
            virtual void updateDataAndMask(Vec3ui centerOfPatchToInpaint);
            virtual void updateDataAndMaskOfOneVoxel(Vec3ui coord);
            virtual void updatePriority(Vec3ui centerOfPatchToInpaint);
            virtual float calculateTargetValue(Vec3ui coord);

            ComputeOrder* getPriority();
            PatchSelection* getPatchSelection();

            Vec3ui getCenterOfLastInpaintedPatch();
            Vec3ui getCenterOfLastSourcePatch();

            void setCenterOfSourcePatch(Vec3i value);

        protected:
            void outputDebugVolumesIfRequired();
            void outputDebugVolumes();

        protected:
            Problem* problem;
            ProgressReporter* progress;
            ComputeOrder* priority;
            ComputeOrder* fillfront;
            PatchSelection* patchSelection;
            FloatVolume* error;
            Vec3i centerOfPatchToInpaint;
            Vec3i centerOfSourcePatch;
            unsigned int iterationNumber;
            bool shouldUseBlendOperation;
            unsigned int voxelsUpdated;
            Vec3i targetPatchUpperLeft;

            InpaintingDebugParameters* debugParameters;
            InpaintingOptimizationParameters* optimizationParameters;

			std::vector<Vec3i> selectedCoordinatesByCriminisi;
        };

    } // namespace inpainting
} // namespace ettention