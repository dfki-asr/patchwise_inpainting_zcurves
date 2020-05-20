#pragma once

#include "setup/parameterset/ParameterSet.h"

namespace ettention
{
    class ParameterSource;

    namespace inpainting
    {
        class InpaintingDebugParameters : public ParameterSet
        {
        public:
            InpaintingDebugParameters(const ParameterSource* parameterSource);
            InpaintingDebugParameters();
            ~InpaintingDebugParameters();

            bool shouldOutputErrorVolumes();
            std::string getDebugInfoPath();
			std::string getLogFileName();
            unsigned int shouldOutputDebugVolumesEveryNIterations();
			bool shouldDoubleCheckUsingBruteForce();
			bool shouldLogAcceleration();

			bool shouldOutputData() const;
			bool shouldOutputMask() const;
			bool shouldOutputConfidenceTerm() const;
			bool shouldOutputDataTerm() const;
			bool shouldOutputPriority() const;
			bool shouldOutputGradient() const;
			bool shouldOutputMaskNormal() const;
			bool shouldOutputFillFront() const;

		public:
			bool outputErrorVolume;
            std::filesystem::path debugInfoPath;
			std::string logFileName;
            unsigned int outputDebugVolumesEveryNIterations;
			bool doubleCheckUsingBruteForce;
			bool logAcceleration;
			bool outputData;
			bool outputMask;
			bool outputConfidenceTerm;
			bool outputDataTerm;
			bool outputPriority;
			bool outputGradient;
			bool outputMaskNormal;
			bool outputFillFront;
		};
    }
}