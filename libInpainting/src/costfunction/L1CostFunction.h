#pragma once

#include "DictionaryBasedCostFunctionKernel.h"
#include "L1Distance.h"

namespace libmmv
{
    class ByteVolume;
}

namespace inpainting 
{
    class L1CostFunction: public DictionaryBasedCostFunctionKernel 
    {
    public:
        L1CostFunction( Problem* problem, Dictionary* dictionary );
        ~L1CostFunction();

        virtual void computeCostForInterval( IndexInterval interval ) override;
        float computeCostFunction( unsigned int indexOfSourcePatch);

    protected:
        L1Distance< BytePatchAccess8Bit>* distance;
    };
}
