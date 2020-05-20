#pragma once

#include "DictionaryBasedCostFunctionKernel.h"
#include "L2Distance.h"

namespace ettention 
{
    class ByteVolume;

    namespace inpainting 
    {
        class L2CostFunction: public DictionaryBasedCostFunctionKernel 
        {
        public:
            L2CostFunction( Problem* problem, Dictionary* dictionary );
            ~L2CostFunction();

            virtual void computeCostForInterval( IndexInterval interval ) override;
            float computeCostFunction( unsigned int patchCoordinate );
        };
    }
}
