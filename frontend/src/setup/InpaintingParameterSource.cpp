#include "stdafx.h"

#include "setup/parametersource/anyoption.h"

#include "InpaintingParameterSource.h"

namespace ettention
{

    /*InpaintingParameterSource::InpaintingParameterSource(const std::vector<std::string>& argv)
        : CommandLineParameterSource( argc, argv )
    {
        declareAcceptedParameters();
    } */

    InpaintingParameterSource::InpaintingParameterSource(int argc, char* argv[])
        : CommandLineParameterSource(argc, argv)
    {
        // declareAcceptedOptions();
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