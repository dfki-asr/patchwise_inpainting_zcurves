#pragma once

#include "DictionaryBasedCostFunctionKernel.h"
#include "L2Distance.h"

namespace inpainting 
{
    class FusingL1CostFunction: public DictionaryBasedCostFunctionKernel 
    {
    public:
        FusingL1CostFunction( Problem* problem, Dictionary* dictionary, float denseWeight);
        ~FusingL1CostFunction();

        virtual void computeCostForInterval( IndexInterval interval ) override;

        float updateDistance(float min, float max, float value);
        float computeCostFunction( unsigned int patchCoordinate );
            
    protected:
        float denseWeight;
        BytePatchAccess8Bit denseDataAccess;
    };
}
