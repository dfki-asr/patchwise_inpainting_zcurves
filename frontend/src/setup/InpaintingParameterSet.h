#pragma once

#include "libmmv/math/Vec3.h"

namespace libCFG
{
    class ParameterSource;
}

namespace inpainting
{
    class InpaintingParameterSet
    {
    public:

    public:
        InpaintingParameterSet();
        ~InpaintingParameterSet();

        void setDefaultParameters();
        void initializeParameters(libCFG::ParameterSource* parameterSource);

        libmmv::Vec3ui patchSize;
        std::vector<float> costWeight;
        std::filesystem::path sparseFileName;
        std::filesystem::path maskFileName;
        std::filesystem::path dictionaryFileName;
        std::filesystem::path dictionaryMaskFileName;
        std::filesystem::path outputFileName;
        std::filesystem::path denseFileName;
        bool shoudlUseCache;
        std::filesystem::path errorFileName;
        
        bool shouldOutputError;
        bool shouldUseBlendOperation;
        bool denseFileGiven;
		bool dictionaryMaskGiven;
        bool regularizeConfidence;
        bool printTraversalInformation;

        unsigned int maximumNumberOfReturnedIntervalsPerTraversal;

        float denseScanWeight;
        float inpaintingProgressStepSize;
        
        std::string costFunctionName;
        std::string priorityName;

    };
}
