#include "stdafx.h"

#include "InpaintingDebugParameters.h"
#include "setup/parametersource/ParameterSource.h"

namespace inpainting
{

    InpaintingDebugParameters::InpaintingDebugParameters(const cfg::ParameterSource* parameterSource)
    {
        debugInfoPath = std::filesystem::path("debug");
        if (parameterSource->parameterExists("debug.infoPath"))
        {
            debugInfoPath = parameterSource->getPathParameter("debug.infoPath");
        }

		logFileName = std::string("acceleration.log");
		if (parameterSource->parameterExists("debug.logFileName"))
		{
			logFileName = parameterSource->getStringParameter("debug.logFileName");
		}

        outputErrorVolume = false;
        if (parameterSource->parameterExists("debug.outputErrorVolume"))
			outputErrorVolume = true;

		outputData = false;
		if (parameterSource->parameterExists("debug.outputDataVolume"))
			outputData = true;

		outputMask = false;
		if (parameterSource->parameterExists("debug.outputMaskVolume"))
			outputMask = true;

		outputConfidenceTerm = false;
		if (parameterSource->parameterExists("debug.outputConfidenceTerm"))
			outputConfidenceTerm = true;

		outputDataTerm = false;
		if (parameterSource->parameterExists("debug.outputDataTerm"))
			outputDataTerm = true;

		outputPriority = false;
		if (parameterSource->parameterExists("debug.outputPriority"))
			outputPriority = true;

		outputGradient = false;
		if (parameterSource->parameterExists("debug.outputGradient"))
			outputGradient = true;

		outputMaskNormal = false; 
		if (parameterSource->parameterExists("debug.outputMaskNormal"))
			outputMaskNormal = true;

		outputFillFront = false;
		if (parameterSource->parameterExists("debug.outputFillFront"))
			outputFillFront = true;

        outputDebugVolumesEveryNIterations = std::numeric_limits<int>::max();
        if (parameterSource->parameterExists("debug.outputEveryNIterations"))
        {
            outputDebugVolumesEveryNIterations = parameterSource->getIntParameter("debug.outputEveryNIterations");
        }

		doubleCheckUsingBruteForce = false;
		if (parameterSource->parameterExists("debug.doubleCheckUsingBruteForce"))
			doubleCheckUsingBruteForce = true;

		logAcceleration = false;
		if (parameterSource->parameterExists("debug.logAcceleration"))
		{
			logAcceleration = true;
		}
	}

    InpaintingDebugParameters::InpaintingDebugParameters()
    {
        debugInfoPath = std::filesystem::path("debug");
        outputErrorVolume = false;
        outputDebugVolumesEveryNIterations = std::numeric_limits<int>::max();
		doubleCheckUsingBruteForce = false;
        logAcceleration = false;

		outputData = false;
		outputMask = false;
		outputConfidenceTerm = false;
		outputDataTerm = false;
		outputPriority = false;
		outputGradient = false;
		outputMaskNormal = false;
		outputFillFront = false;
    }

    InpaintingDebugParameters::~InpaintingDebugParameters()
    {
    }

    bool InpaintingDebugParameters::shouldOutputErrorVolumes()
    {
        return outputErrorVolume;
    }

    std::string InpaintingDebugParameters::getDebugInfoPath()
    {
        return debugInfoPath.string();
    }

	std::string InpaintingDebugParameters::getLogFileName()
	{
		return logFileName;
	}

	unsigned int InpaintingDebugParameters::shouldOutputDebugVolumesEveryNIterations()
    {
        return outputDebugVolumesEveryNIterations;
    }

	bool InpaintingDebugParameters::shouldDoubleCheckUsingBruteForce()
	{
		return doubleCheckUsingBruteForce;
	}

	bool InpaintingDebugParameters::shouldLogAcceleration()
    {
        return logAcceleration;
    }

	bool InpaintingDebugParameters::shouldOutputData() const
	{
		return outputData;
	}

	bool InpaintingDebugParameters::shouldOutputMask() const
	{
		return outputMask;
	}

	bool InpaintingDebugParameters::shouldOutputConfidenceTerm() const
	{
		return outputConfidenceTerm;
	}

	bool InpaintingDebugParameters::shouldOutputDataTerm() const
	{
		return outputDataTerm;
	}

	bool InpaintingDebugParameters::shouldOutputPriority() const
	{
		return outputPriority;
	}

	bool InpaintingDebugParameters::shouldOutputGradient() const
	{
		return outputGradient;
	}

	bool InpaintingDebugParameters::shouldOutputMaskNormal() const
	{
		return outputMaskNormal;
	}

	bool InpaintingDebugParameters::shouldOutputFillFront() const
	{
		return outputFillFront;
	}

}
