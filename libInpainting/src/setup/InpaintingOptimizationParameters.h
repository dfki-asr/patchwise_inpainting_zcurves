#pragma once

#include "libCFG/setup/parameterset/ParameterSet.h"

namespace libCFG 
{
	class ParameterSource;
}

namespace inpainting
{
    class InpaintingOptimizationParameters : public libCFG::ParameterSet
    {
    public:
        InpaintingOptimizationParameters(const libCFG::ParameterSource* parameterSource);
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
