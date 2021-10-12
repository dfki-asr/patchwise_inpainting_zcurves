#include "stdafx.h"

#include "libCFG/setup/parametersource/anyoption.h"

#include "InpaintingParameterSource.h"

namespace inpainting
{

    InpaintingParameterSource::InpaintingParameterSource(int argc, char* argv[])
    {
    }

    InpaintingParameterSource::~InpaintingParameterSource()
    {
    }

    void InpaintingParameterSource::declareAcceptedOptions()
    {
        options->addUsage("usage: ");
        options->addUsage("");

		options->setOption("config", 'c');
        options->addUsage("  -c --config                              specify an xml config file, many options are accessible only via xml");
		options->setOption("sparseFile");
        options->addUsage("  -m --maskFile                            file containing sparse data");
        options->setOption("maskFile");
        options->addUsage("  -s --sparseFile                          file containing mask");
        options->setOption("sparseFile", 's');
        options->addUsage("  -d --dictionaryFile                      file containing dictionary data");
        options->setOption("dictionaryMaskFile");
        options->addUsage("     --dictionaryMaskFile                  file containing the mask of the dictionary");
        options->setOption("outputFile");
        options->addUsage("  -o --outputFile                          output file");
    }
}