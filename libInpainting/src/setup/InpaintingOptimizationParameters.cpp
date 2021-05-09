#include "stdafx.h"

#include "InpaintingOptimizationParameters.h"
#include "setup/parametersource/ParameterSource.h"

namespace inpainting
{

    InpaintingOptimizationParameters::InpaintingOptimizationParameters(const cfg::ParameterSource* parameterSource)
    {
		useParallelComputation = false;
		if (parameterSource->parameterExists("optimization.parallelComputation"))
		{
			useParallelComputation = parameterSource->getBoolParameter("optimization.parallelComputation");
		}

		minimalNumberOfDataPointsToUseDifferentCore = 512;
		if (parameterSource->parameterExists("optimization.minimalNumberOfDataPointsForParallelization"))
		{
			minimalNumberOfDataPointsToUseDifferentCore = parameterSource->getUIntParameter("optimization.minimalNumberOfDataPointsForParallelization");
		}

        thresholdForZCurveQuery = 32;
        if (parameterSource->parameterExists("optimization.thresholdForZCurveQuery"))
        {
            thresholdForZCurveQuery = parameterSource->getUIntParameter("optimization.thresholdForZCurveQuery");
        }

		typicalCostFunctionValue = 16.0f;
		if (parameterSource->parameterExists("optimization.typicalCostFunctionValue"))
		{
			typicalCostFunctionValue = parameterSource->getFloatParameter("optimization.typicalCostFunctionValue");
		}

		costFunctionRelaxationIncrement = 16.0f;
		if (parameterSource->parameterExists("optimization.relaxationIncrement"))
		{
			costFunctionRelaxationIncrement = parameterSource->getFloatParameter("optimization.relaxationIncrement");
		}
	}

    InpaintingOptimizationParameters::InpaintingOptimizationParameters()
    {
		useParallelComputation = false;
		minimalNumberOfDataPointsToUseDifferentCore = 512;
        thresholdForZCurveQuery = 32;
		typicalCostFunctionValue = 16.0f;
		costFunctionRelaxationIncrement = 4.0f;
    }

    InpaintingOptimizationParameters::~InpaintingOptimizationParameters()
    {
    }

	bool InpaintingOptimizationParameters::shouldUseParallelComputation()
	{
		return useParallelComputation;
	}

	unsigned int InpaintingOptimizationParameters::getMinimalNumberOfDataPointsToUseDifferentCore()
	{
		return minimalNumberOfDataPointsToUseDifferentCore;
	}

	unsigned int InpaintingOptimizationParameters::getThresholdForZCurveQuery()
    {
        return thresholdForZCurveQuery;
    }

	float InpaintingOptimizationParameters::getTypicalCostFunctionValue()
	{
		return typicalCostFunctionValue;
	}

	float InpaintingOptimizationParameters::getCostFunctionRelaxationIncrement()
	{
		return costFunctionRelaxationIncrement;
	}

}
