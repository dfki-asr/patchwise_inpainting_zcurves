#include "stdafx.h"

#include "InpaintingParameterSet.h"

#include "setup/parametersource/ParameterSource.h"

namespace ettention
{
    InpaintingParameterSet::InpaintingParameterSet()
    {
        setDefaultParameters();
    }

    InpaintingParameterSet::~InpaintingParameterSet()
    {
    }

    void InpaintingParameterSet::setDefaultParameters()
    {
        shoudlUseCache = false;
        shouldOutputError = false;
        shouldUseBlendOperation = true;
        denseScanWeight = 0.0f;
        inpaintingProgressStepSize = 5.0f;
        denseFileGiven = false;
		dictionaryMaskGiven = false;
        regularizeConfidence = false;
        costFunctionName = "L2";
        priorityName = "criminisi";
        maximumNumberOfReturnedIntervalsPerTraversal = 100;
        printTraversalInformation = false;
    }

    void InpaintingParameterSet::initializeParameters(ParameterSource* parameterSource)
    {
        patchSize = parameterSource->getVec3uiParameter("patchSize");
        sparseFileName = parameterSource->getPathParameter("sparseFile");
        maskFileName = parameterSource->getPathParameter("maskFile");
        dictionaryFileName = parameterSource->getPathParameter("dictionaryFile");
        outputFileName = parameterSource->getPathParameter("outputFile");

		if (parameterSource->parameterExists("dictionaryMaskFile"))
		{
			dictionaryMaskFileName = parameterSource->getPathParameter("dictionaryMaskFile");
			dictionaryMaskGiven = true;
		}

        if( parameterSource->parameterExists("denseFile") )
        {
            denseFileName = parameterSource->getPathParameter("denseFile");
            denseScanWeight = parameterSource->getFloatParameter("denseWeight");
            denseFileGiven = true;
        }

        if( parameterSource->parameterExists("errorFile") )
        {
            shouldOutputError = true;
            errorFileName = parameterSource->getPathParameter("errorFile");
        }

        if( parameterSource->parameterExists("disablePatchBlending") )
            shouldUseBlendOperation = false;

        if( parameterSource->parameterExists("inpaintingProgressStepSize") )
        {
            inpaintingProgressStepSize = parameterSource->getFloatParameter("inpaintingProgressStepSize");

        } else
        {
            inpaintingProgressStepSize = 1.0f;
        }

        if( parameterSource->parameterExists("costFunction") )
        {
            costFunctionName = parameterSource->getStringParameter("costFunction");
        }

        if (parameterSource->parameterExists("costWeight"))
        {
            costWeight = parameterSource->getVecFloatParameter("costWeight");
        }

        if( parameterSource->parameterExists("maximumNumberOfReturnedIntervalsPerTraversal") )
        {
            maximumNumberOfReturnedIntervalsPerTraversal = parameterSource->getUIntParameter("maximumNumberOfReturnedIntervalsPerTraversal");
        }

        if( parameterSource->parameterExists("priority") )
        {
            priorityName = parameterSource->getStringParameter("priority");
        }

        if( parameterSource->parameterExists("regularizeConfidence") )
        {
            regularizeConfidence = true;
        }

        if( parameterSource->parameterExists("printTraversalInformation"))
        {
            printTraversalInformation = true;
        }

    }

}