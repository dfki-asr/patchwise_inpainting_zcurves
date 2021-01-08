#include "stdafx.h"

#include "InpaintingFrontEnd.h"

#include "error/ParameterNotFoundException.h"

#include "io/deserializer/VolumeDeserializer.h"
#include "io/serializer/VolumeSerializer.h"

#include "setup/parameterset/OutputParameterSet.h"
#include "setup/parametersource/CommandLineParameterSource.h"
#include "setup/parametersource/ParameterSource.h"
#include "setup/parametersource/XMLParameterSource.h"
#include "setup/parametersource/CascadingParameterSource.h"
#include "setup/InpaintingParameterSource.h"

#include "InpaintingOperator.h"
#include "Problem.h"

#include "dictionary/Dictionary.h"
#include "costfunction/L1CostFunction.h"
#include "costfunction/L2CostFunction.h"
#include "costfunction/L2CostFunctionWithWeight.h"
#include "costfunction/FusingL1CostFunction.h"
#include "costfunction/FusingL2CostFunction.h"
#include "dictionary/FullDictionaryBuilder.h"
#include "dictionary/MaskedDictionaryBuilder.h"

#include "patchselection/CurveBasedPatchSelection.h"
#include "patchselection/PatchMatchPatchSelection.h"
#include "computeorder/ConfidenceOrder.h"
#include "computeorder/BWCriminisiOrder.h"
#include "computeorder/MultiChannelCriminisiOrder.h"
#include "computeorder/HighConfidenceOrder.h"
#include "computeorder/MissingWedgeOrder.h"

#include "setup/InpaintingDebugParameters.h"
#include "setup/InpaintingOptimizationParameters.h"
#include "setup/IndexOptions.h"
#include "setup/CostFunctionOptions.h"

#include "IntermediateOutputProgressReporter.h"

/*
	Handles Configuration file input to set up inpainting procedure.
*/

namespace ettention
{
    namespace inpainting 
    {
        InpaintingFrontEnd::InpaintingFrontEnd(int argc, char* argv[])
        {
            parameterSource = handleCommandLine(argc, argv);
            parameterStorage.initializeParameters(parameterSource);

            optimizationParameters = new InpaintingOptimizationParameters(parameterSource);

            initializeProblem();
            debugParameters = new InpaintingDebugParameters(parameterSource);
			indexOptions = new IndexOptions(parameterSource);
			costFunctionOptions = new CostFunctionOptions(parameterSource);
            instantiateDictionary();

            progress = new IntermediateOutputProgressReporter(5.0f);
            costFunction = instantiateCostFunction();
            priority = instantiatePriority();

            patchSelection = instantiatePatchSelection();

            inpainter = new InpaintingOperator(problem, priority, patchSelection, progress, parameterStorage.shouldUseBlendOperation, debugParameters, optimizationParameters );
        }

        InpaintingFrontEnd::~InpaintingFrontEnd()
        {
            delete inpainter;
            delete patchSelection;
            delete priority;
            delete costFunction;

            delete progress;

            delete dictionary;
            delete problem;

			delete indexOptions;
            delete debugParameters;
            delete optimizationParameters;

            delete parameterSource;
        }
        
        void InpaintingFrontEnd::handlePrintDevicesCommand(ParameterSource* parameterSource)
        {
            if( parameterSource->parameterExists("devices") )
            {
                exit(0);
            }
        }

        DictionaryBasedCostFunctionKernel* InpaintingFrontEnd::instantiateCostFunction()
        {
			if (parameterStorage.costFunctionName == "L2")
			{
				return new L2CostFunction( problem, dictionary );
			}
            if (parameterStorage.costFunctionName == "L2_with_weight")
            {
                return new L2CostFunctionWithWeight(problem, dictionary);
            }
			if (parameterStorage.costFunctionName == "L1")
			{
                return new L1CostFunction(problem, dictionary);
            }
            if( parameterStorage.costFunctionName == "fusing_l1" )
            {
                return new FusingL1CostFunction( problem, dictionary, parameterStorage.denseScanWeight );
            }
            if( parameterStorage.costFunctionName == "fusing_l2" )
            {
                return new FusingL2CostFunction( problem, dictionary, parameterStorage.denseScanWeight );
            }
            throw std::runtime_error("illegal cost function name: " + parameterStorage.costFunctionName);
        }

        PatchSelection* InpaintingFrontEnd::instantiatePatchSelection()
        {
            if ( indexOptions->getIndexType() == IndexOptions::IndexType::NONE )
            {
                auto& compressedDictionary = dictionary->getCompressedDictionary();
				return new CurveBasedPatchSelection(costFunction, problem->data, problem->mask, problem->dictionaryVolume, compressedDictionary, problem->patchSize, debugParameters, optimizationParameters, indexOptions, costFunctionOptions, true, progress);
			}
			else if ( indexOptions->getIndexType() == IndexOptions::IndexType::PATCH_MATCH )
			{
				// compressed dictionary only placeholder till implemented
				auto& compressedDictionary = dictionary->getCompressedDictionary();
				return new PatchMatchPatchSelection(costFunction, problem->data, problem->mask, problem->dictionaryVolume, compressedDictionary, problem->patchSize, debugParameters, optimizationParameters, indexOptions, costFunctionOptions, true, progress);
			}
			else 
            {
                auto& compressedDictionary = dictionary->getCompressedDictionary();
				return new CurveBasedPatchSelection(costFunction, problem->data, problem->mask, problem->dictionaryVolume, compressedDictionary, problem->patchSize, debugParameters, optimizationParameters, indexOptions, costFunctionOptions, false, progress);
			}
        }

        ComputeOrder* InpaintingFrontEnd::instantiatePriority()
        {
            if( parameterStorage.priorityName == "criminisi" )
            {
				auto priority = new BWCriminisiOrder( problem, progress, parameterStorage.regularizeConfidence );
				priority->init();
				return priority;
            }
            if (parameterStorage.priorityName == "confidence")
            {
				auto priority = new ConfidenceOrder( problem->data, problem->mask, problem->patchSize, progress );
				return priority;
			}
			if (parameterStorage.priorityName == "highconfidence")
			{
				auto innerOrder = new BWCriminisiOrder( problem, progress, parameterStorage.regularizeConfidence );
				auto priority = new HighConfidenceOrder(problem, innerOrder, 3);
				return priority;
			}
			if (parameterStorage.priorityName == "multichannel_criminisi")
			{
				auto priority = new MultiChannelCriminisiOrder( problem, progress, parameterStorage.regularizeConfidence );
				priority->init();
				return priority;
			}
			if (parameterStorage.priorityName == "missingwedge")
            {
                auto innerOrder = new BWCriminisiOrder( problem, progress, parameterStorage.regularizeConfidence );
				innerOrder->init();
				auto priority = new MissingWedgeOrder(problem, innerOrder);
				return priority;
			}
            throw std::runtime_error("illegal priority name: " + parameterStorage.priorityName);
        }

        void InpaintingFrontEnd::initializeProblem()
        {
            problem = new Problem();
            problem->patchSize = parameterStorage.patchSize;
            problem->costWeight = parameterStorage.costWeight;

            problem->data = dynamic_cast<ByteVolume*>(VolumeDeserializer::load(parameterStorage.sparseFileName.string(), Voxel::DataType::UCHAR_8));

            problem->mask = dynamic_cast<ByteVolume*>(VolumeDeserializer::load(parameterStorage.maskFileName.string(), Voxel::DataType::UCHAR_8));

			if ( parameterStorage.dictionaryFileName != parameterStorage.sparseFileName )
			{
				problem->dictionaryVolume = dynamic_cast<ByteVolume*>(VolumeDeserializer::load(parameterStorage.dictionaryFileName.string(), Voxel::DataType::UCHAR_8));
			} 
			else // use data volume as dictionary
			{
				problem->dictionaryVolume = problem->data;
			}

			if (parameterStorage.dictionaryMaskGiven)
			{
				if (parameterStorage.maskFileName != parameterStorage.dictionaryMaskFileName)
				{
					problem->dictionaryMask = dynamic_cast<ByteVolume*>(VolumeDeserializer::load(parameterStorage.dictionaryMaskFileName.string(), Voxel::DataType::UCHAR_8));
				}
				else // use same mask for dictionary and data
				{
					problem->dictionaryMask = problem->mask;
				}
			}

            if( problem->data->getProperties().getVolumeResolution() != problem->mask->getProperties().getVolumeResolution() )
            {
                throw std::runtime_error("Data and Mask have different resolution, check input!");
            }

            if( parameterStorage.denseFileGiven )
            {
                problem->denseScan = dynamic_cast<ByteVolume*>(VolumeDeserializer::load(parameterStorage.denseFileName.string(), Voxel::DataType::UCHAR_8));

                if( problem->data->getProperties().getVolumeResolution() != problem->denseScan->getProperties().getVolumeResolution() )
                {
                    throw std::runtime_error("Data and DenseScan have different resolution, check input!");
                }

                if( problem->mask->getProperties().getVolumeResolution() != problem->denseScan->getProperties().getVolumeResolution() )
                {
                    throw std::runtime_error("Mask and DenseScan have different resolution, check input!");
                }
            }
        }

        void InpaintingFrontEnd::instantiateDictionary()
        {
			if (parameterStorage.dictionaryMaskGiven)
			{
				MaskedDictionaryBuilder builder(problem);
				dictionary = builder.createDictionary( );
				dictionary->compressDictionary();
			}
			else
			{
				FullDictionaryBuilder builder(problem);
				dictionary = builder.createDictionary( );
				dictionary->compressDictionary();
			}
        }

        ParameterSource* InpaintingFrontEnd::handleCommandLine(int argc, char* argv[])
        {
            auto commandLineParameterSource = new InpaintingParameterSource(argc, argv);
            commandLineParameterSource->parse();

            if (!commandLineParameterSource->parameterExists("config"))
                return commandLineParameterSource;

            auto configFileName = commandLineParameterSource->getStringParameter("config");
            std::cout << "Loading configuration from XML file " << configFileName << std::endl;
            auto xmlParameterSource = new XMLParameterSource( configFileName );

            auto cascadingParameterSource = new CascadingParameterSource();
            cascadingParameterSource->addSource(commandLineParameterSource);
            cascadingParameterSource->parseAndAddSource(xmlParameterSource);

            return cascadingParameterSource;
        }

        void InpaintingFrontEnd::performInpainting()
        {
            progress->setStepSize(parameterStorage.inpaintingProgressStepSize);

			inpainter->run();

            OutputParameterSet options;
            VolumeSerializer volumeSerializer;
    		volumeSerializer.write(problem->data, parameterStorage.outputFileName.string() + ".mrc", options.getVoxelType(), options.getOrientation() );

            if( parameterStorage.shouldOutputError )
            {
                volumeSerializer.write(inpainter->getErrorVolume(), parameterStorage.errorFileName.string() + ".mrc", options.getVoxelType(), options.getOrientation() );
            }
        }
    } // namespace inpainting
} // namespace ettention