#include "stdafx.h"

#include "ConsoleProgressReporter.h"

#ifdef _WINDOWS
#include <windows.h>
#endif

namespace ettention
{
    namespace inpainting 
    {

        ConsoleProgressReporter::ConsoleProgressReporter(float stepSize)
            : stepSize(stepSize)
        {

        }

        ConsoleProgressReporter::~ConsoleProgressReporter()
        {

        }

        void ConsoleProgressReporter::reportTaskStart(std::string task, float totalProgress)
        {
            mutex.lock();
			currentTime.start();
			ProgressReporter::reportTaskStart(task, totalProgress);
            std::cout << task << " [";
            std::cout.flush();
            lastReportedProgress = 0.0f;
            mutex.unlock();
        }

        void ConsoleProgressReporter::reportTaskEnd()
        {
            mutex.lock();
			std::cout << "] ";
			currentTime.stop();
			std::cout << std::setprecision(3);
			std::cout << currentTime.getElapsedTimeInSec() << " s" << std::endl;
            mutex.unlock();
        }

        void ConsoleProgressReporter::setStepSize(float newStepSize)
        {
            stepSize = newStepSize;
        }

        void ConsoleProgressReporter::handleProgressReport()
        {
            mutex.lock();
            if (getProgressInPercent() - lastReportedProgress > stepSize)
            {
                std::cout << std::fixed << std::setprecision(0) << getProgressInPercent() << "% ";
                std::cout.flush();
                lastReportedProgress = getProgressInPercent();
				progress = getProgressInPercent();
				setConsoleTitle(estimatedDurationAsString());
			}
            mutex.unlock();
        }

        std::string ConsoleProgressReporter::estimatedDurationAsString()
        {
            auto secs = currentTime.getElapsedTimeInSec();
            float progressNormalized = progress / 100.0f;
            int eta = (int)(secs / progressNormalized * (1.0 - progressNormalized));
            std::ostringstream title;
            title << "Inpainting in progress... (" << std::fixed << std::setprecision(2) << progress << "%)";
            title << ", time passed: ";
            convertTime(title, (int)secs);
            if( progress >= 5 )
            {
                title << ", eta: ";
                convertTime(title, eta);
            } else
            {
                title << ". Awaiting 5% for better estimates.";
            }
            return title.str();
        }

        void ConsoleProgressReporter::convertTime(std::ostringstream &stream, int time)
        {
            if( time < 60 )
            {
                stream << "< 1m";
            } else if( time < 3600 )
            {
                stream << "~" << (time / 60) << "m.";
            } else
            {
                stream << "~" << (time / 3600) << "h " << ((time - 3600 * (time / 3600)) / 60) << "m.";
            }
        }

        void ConsoleProgressReporter::setConsoleTitle(std::string title)
        {
#ifdef _WINDOWS
            SetConsoleTitle(title.c_str());
#endif
        }
    } // namespace inpainting
} // namespace ettention