#pragma once

namespace ettention
{
	namespace inpainting 
	{
		class ProgressReporter
		{
		public: 
			virtual void reportTotalProgress(float progress);
			virtual void reportIncrementalProgress(float progress);
			virtual void reportTaskStart(std::string task, float totalProgress);
			virtual void reportTaskEnd();

			virtual void setTotalAmountOfWork(float amount);
			virtual float getProgressInPercent();
		protected:
			virtual void handleProgressReport();

		protected:
			float totalProgress;
			float currentProgress;
		};


	} // namespace inpainting
} // namespace ettention