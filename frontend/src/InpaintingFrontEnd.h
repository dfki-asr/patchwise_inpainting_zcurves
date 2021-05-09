#pragma once

#include "setup/InpaintingParameterSet.h"
#include "ConsoleProgressReporter.h"

namespace cfg
{
    class ParameterSource;
}
 
namespace inpainting
{
    class DictionaryBasedCostFunctionKernel;
    class DictionaryBasedSSDKernel;
    class ComputeOrder;
    class Problem;
    class Dictionary;
    class CostFunctionKernel;
    class PatchSelection;
    class InpaintingOperator;
    class InpaintingDebugParameters;
    class InpaintingOptimizationParameters;
	class IndexOptions;
	class CostFunctionOptions;

    class InpaintingFrontEnd
    {
    public: 
        InpaintingFrontEnd(int argc, char* argv[]);
        ~InpaintingFrontEnd();
        void handlePrintDevicesCommand(cfg::ParameterSource* parameterSource);
        DictionaryBasedCostFunctionKernel* instantiateCostFunction();
        PatchSelection* instantiatePatchSelection();
        ComputeOrder* instantiatePriority();
        void initializeProblem();
        void instantiateDictionary();
        cfg::ParameterSource* handleCommandLine(int argc, char* argv[]);
        void performInpainting();
        
    protected:
        Problem* problem;
        Dictionary* dictionary;
        ConsoleProgressReporter* progress;
        DictionaryBasedCostFunctionKernel* costFunction;
        ComputeOrder* priority;
        PatchSelection* patchSelection;
        InpaintingOperator* inpainter;
        cfg::ParameterSource* parameterSource;
        InpaintingParameterSet parameterStorage;
        InpaintingDebugParameters* debugParameters;
        InpaintingOptimizationParameters* optimizationParameters;
		IndexOptions* indexOptions;
		CostFunctionOptions* costFunctionOptions;
    };
        
} // namespace inpainting
