#pragma once

#include "DictionaryBasedCostFunctionKernel.h"
#include "L2Distance.h"

namespace ettention 
{
    class ByteVolume;

    namespace inpainting 
    {
        class FusingL2CostFunction: public DictionaryBasedCostFunctionKernel 
        {
        public:
            FusingL2CostFunction( Problem* problem, Dictionary* dictionary, float denseWeight);
            ~FusingL2CostFunction();

            virtual void computeCostForInterval( IndexInterval interval ) override;
            float computeCostFunction( unsigned int indexOfSourcePatch);

        protected:
            float denseWeight;
            BytePatchAccess8Bit denseDataAccess;
        };
    }
}
