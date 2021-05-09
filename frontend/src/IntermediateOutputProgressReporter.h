#pragma once

#include "libmmv/math/Vec3.h"

#include "ConsoleProgressReporter.h"

namespace libmmv
{
	class Volume;
}

namespace inpainting 
{
	class CriminisiOrder;

	class IntermediateOutputProgressReporter : public ConsoleProgressReporter
	{
	protected:
		class OutputTarget 
		{
		public:
			libmmv::Volume* volume;
			std::filesystem::path path;
			float stepSize;
			float lastOutput;
			unsigned int counter;
		};

	public: 
		IntermediateOutputProgressReporter(float stepsize);
		virtual ~IntermediateOutputProgressReporter();

		void addOutputTarget(libmmv::Volume* volume, std::filesystem::path, float stepSize);
		void setPriority(CriminisiOrder* priority);

	protected:
		void handleProgressReport() override;

	protected:
		std::vector<OutputTarget* > targets;
		CriminisiOrder* priority;
	};


} // namespace inpainting
