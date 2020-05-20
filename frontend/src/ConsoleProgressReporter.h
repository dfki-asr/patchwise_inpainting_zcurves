#pragma once

#include <mutex>

#include "time/Timer.h"

#include "ProgressReporter.h"

namespace ettention
{
    namespace inpainting 
    {
        class ConsoleProgressReporter : public ProgressReporter
        {
        public: 
            ConsoleProgressReporter( float stepsize = 1.0f );
            virtual ~ConsoleProgressReporter();

            virtual void reportTaskStart(std::string task, float totalProgress);
            virtual void reportTaskEnd() override;

            void setStepSize( float newStepSize );

        protected:
            void handleProgressReport() override;
            std::string estimatedDurationAsString();
            void convertTime(std::ostringstream &stream, int time);
            void setConsoleTitle(std::string title);

        protected:
            std::mutex mutex;
            float stepSize;
            float lastReportedProgress;

            Timer currentTime;
            float progress;
        };


    } // namespace inpainting
} // namespace ettention