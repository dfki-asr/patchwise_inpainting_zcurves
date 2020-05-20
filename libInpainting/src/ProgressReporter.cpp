#include "stdafx.h"

#include "ProgressReporter.h"

namespace ettention
{
	namespace inpainting 
	{

		void ProgressReporter::reportTotalProgress(float progress)
		{
			this->currentProgress = progress; 
			handleProgressReport();
		}

		void ProgressReporter::reportIncrementalProgress(float progress)
		{
			this->currentProgress += progress;
			handleProgressReport();
		}

		void ProgressReporter::reportTaskStart(std::string task, float totalProgress)
		{
			this->currentProgress = 0.0f;
			this->totalProgress = totalProgress;
		}

		void ProgressReporter::reportTaskEnd()
		{
		}

		void ProgressReporter::setTotalAmountOfWork(float amount)
		{
			totalProgress = amount;
		}

		float ProgressReporter::getProgressInPercent()
		{
			return 100.0f * ( currentProgress / totalProgress );
		}

		void ProgressReporter::handleProgressReport()
		{

		}

	} // namespace inpainting
} // namespace ettention