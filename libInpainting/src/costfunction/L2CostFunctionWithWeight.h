#pragma once

#include <stack>
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
            void L2CostFunctionWithWeight::singleJob(std::mutex* mutex,
                                                     std::stack<std::pair<int, int>>* totalJob,
                                                     BytePatchAccess8Bit* ptrDictionaryAccess,
                                                     BytePatchAccess8Bit* ptrDataAccess,
                                                     BytePatchAccess8Bit* ptrMaskAccess);
        public:
            L2CostFunctionWithWeight( Problem* problem, Dictionary* dictionary);
            ~L2CostFunctionWithWeight();

            virtual void computeCostForInterval( IndexInterval interval ) override;
            float computeCostFunction(unsigned int indexOfSourcePatch,
                BytePatchAccess8Bit* ptrDictionaryAccess,
                BytePatchAccess8Bit* ptrDataAccess,
                BytePatchAccess8Bit* ptrMaskAccess);
        };
    }
}
