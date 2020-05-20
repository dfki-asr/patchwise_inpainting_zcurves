#pragma once

#include "setup/parameterset/ParameterSet.h"

namespace ettention
{
    class ParameterSource;

    namespace inpainting
    {
        class InpaintingOptimizationParameters : public ParameterSet
        {
        public:
            InpaintingOptimizationParameters(const ParameterSource* parameterSource);
            InpaintingOptimizationParameters();
            ~InpaintingOptimizationParameters();

			bool shouldUseParallelComputation();
			unsigned int getMinimalNumberOfDataPointsToUseDifferentCore();
			unsigned int getThresholdForZCurveQuery();
			float getTypicalCostFunctionValue();
			float getCostFunctionRelaxationIncrement();

        private:
			bool useParallelComputation;
			unsigned int minimalNumberOfDataPointsToUseDifferentCore;
			unsigned int thresholdForZCurveQuery;
			float typicalCostFunctionValue;
			float costFunctionRelaxationIncrement;

        };
    }
}