#pragma once

#include "DictionaryBasedCostFunctionKernel.h"
#include "L2Distance.h"

namespace ettention 
{
    class ByteVolume;

    namespace inpainting 
    {
        class L2CostFunctionWithWeight: public DictionaryBasedCostFunctionKernel 
        {
        private:
            std::vector<float> costWeight;
        public:
            L2CostFunctionWithWeight( Problem* problem, Dictionary* dictionary);
            ~L2CostFunctionWithWeight();

            virtual void computeCostForInterval( IndexInterval interval ) override;
            float computeCostFunction( unsigned int patchCoordinate );
        };
    }
}
