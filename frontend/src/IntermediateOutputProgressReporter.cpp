#include "stdafx.h"

#include <iomanip>

#include "IntermediateOutputProgressReporter.h"

#include "computeorder/ComputeOrder.h"
#include "computeorder/CriminisiOrder.h"

#include "libmmv/io/serializer/VolumeSerializer.h"
#include "setup/parameterset/OutputParameterSet.h"

namespace inpainting 
{

    IntermediateOutputProgressReporter::IntermediateOutputProgressReporter(float stepSize)
        : ConsoleProgressReporter(stepSize)
        , priority(nullptr)
    {

    }

    IntermediateOutputProgressReporter::~IntermediateOutputProgressReporter()
    {
        for (auto it : targets)
            delete it;
        targets.clear();
    }

    void IntermediateOutputProgressReporter::addOutputTarget(libmmv::Volume* volume, std::filesystem::path filepath, float stepSize)
    {
        auto newTarget = new OutputTarget();
        newTarget->volume = volume;
        newTarget->path = filepath;
        newTarget->stepSize = stepSize;
        newTarget->lastOutput = 0.0f;
        newTarget->counter = 0;
        targets.push_back(newTarget);
    }

    void IntermediateOutputProgressReporter::setPriority(CriminisiOrder* priority)
    {
        this->priority = priority;
    }

    void IntermediateOutputProgressReporter::handleProgressReport()
    {
        cfg::OutputParameterSet options;
        libmmv::VolumeSerializer serializer;

        for (auto it : targets)
        {
            if (getProgressInPercent() - it->lastOutput > it->stepSize)
            {
                std::stringstream filename;
                filename << it->path.string() << "_" << std::setw(3) << std::setfill('0') << it->counter << ".mrc";
                serializer.write(it->volume, filename.str(), options.getVoxelType(), options.getOrientation() );
                it->lastOutput = getProgressInPercent();
                if (priority)
                {
                    auto frontTerm = priority->plotFillFrontToVolume();
                    std::stringstream filename;
                    filename << it->path.string() << "_" << std::setw(3) << std::setfill('0') << it->counter << "_front.mrc"; 
                    serializer.write(frontTerm, filename.str(), options.getVoxelType(), options.getOrientation() );

                }
                it->counter++;
            }
        }

        ConsoleProgressReporter::handleProgressReport();
    }
} // namespace inpainting
