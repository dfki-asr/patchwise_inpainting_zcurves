#pragma once

#include "libmmv/math/Vec3.h"

namespace libmmv
{
    class FloatVolume;
}

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

        libmmv::FloatVolume* getErrorVolume();

    public: // the following functions are public for testing purpose
        virtual void runOneIteration();

        virtual void inpaintPatch();
        virtual void updateDataAndMask(libmmv::Vec3ui centerOfPatchToInpaint);
        virtual void updateDataAndMaskOfOneVoxel(libmmv::Vec3ui coord);
        virtual void updatePriority(libmmv::Vec3ui centerOfPatchToInpaint);
        virtual float calculateTargetValue(libmmv::Vec3ui coord);

        ComputeOrder* getPriority();
        PatchSelection* getPatchSelection();

        libmmv::Vec3ui getCenterOfLastInpaintedPatch();
        libmmv::Vec3ui getCenterOfLastSourcePatch();

        void setCenterOfSourcePatch(libmmv::Vec3i value);

    protected:
        void outputDebugVolumesIfRequired();
        void outputDebugVolumes();

    protected:
        Problem* problem;
        ProgressReporter* progress;
        ComputeOrder* priority;
        ComputeOrder* fillfront;
        PatchSelection* patchSelection;
        libmmv::FloatVolume* error;
        libmmv::Vec3i centerOfPatchToInpaint;
        libmmv::Vec3i centerOfSourcePatch;
        unsigned int iterationNumber;
        bool shouldUseBlendOperation;
        unsigned int voxelsUpdated;
        libmmv::Vec3i targetPatchUpperLeft;

        InpaintingDebugParameters* debugParameters;
        InpaintingOptimizationParameters* optimizationParameters;

		std::vector<libmmv::Vec3i> selectedCoordinatesByCriminisi;
    };

} // namespace inpainting
