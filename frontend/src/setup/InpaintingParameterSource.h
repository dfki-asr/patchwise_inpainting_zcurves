#pragma once

#include "libCFG/setup/parametersource/ParameterSource.h"
#include "libCFG/setup/parametersource/CommandLineParameterSource.h"

namespace inpainting
{
    enum class CLI_APP_STATE {
        DEFAULT = 0, RECONSTRUCT, DEVICES, DEVICESANDEXTENSIONS, HELP, HELPANDPARAMETERS
    };

    class InpaintingParameterSource : public libCFG::CommandLineParameterSource
    {
    public:
        InpaintingParameterSource(int argc, char* argv[]);
        virtual ~InpaintingParameterSource();

    protected:
        virtual void declareAcceptedOptions() override;

    private:
        CLI_APP_STATE currentState = CLI_APP_STATE::DEFAULT;
    };
}
