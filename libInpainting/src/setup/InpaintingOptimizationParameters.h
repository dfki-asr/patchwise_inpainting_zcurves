#pragma once

#include "setup/parameterset/ParameterSet.h"

namespace cfg 
{
	class ParameterSource;
}

namespace inpainting
{
    class InpaintingOptimizationParameters : public cfg::ParameterSet
    {
    public:
        InpaintingOptimizationParameters(const cfg::ParameterSource* parameterSource);
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
