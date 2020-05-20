#pragma once

#include "IndexInterval.h"
#include "index/NDPointPair.h"

namespace ettention 
{
    class ByteVolume;

    namespace inpainting 
    {
        class Problem;
        class Dictionary;
		class NDSpaceMapping;

        class DictionaryBasedCostFunctionKernel 
        {
        public:
            DictionaryBasedCostFunctionKernel( Problem* problem, Dictionary* dictionary );
            ~DictionaryBasedCostFunctionKernel();

            void computeCostForAllIntervals(std::vector< IndexInterval > all_intervals);
            virtual void computeCostForInterval( IndexInterval interval ) = 0;
            float computeCostFunction( unsigned int indexOfSourcePatch );

            std::vector<float>& getResult();

            virtual void setCenterOfTargetPatch (Vec3i centerOfTargetPatch );

        protected:
            Problem* problem;

            BytePatchAccess8Bit dictionaryAccess;
            BytePatchAccess8Bit dataAccess;
            BytePatchAccess8Bit maskAccess;

            Dictionary* dictionary;
            std::vector<float> resultCost;
            Vec3i centerOfTargetPatch;
            unsigned int indexOfTargetPatch;
        };
    }
}
