#pragma once

#include "math/Vec3.h"

namespace ettention
{
	class VolumeConvolutionOperator;
	class Volume;
	class ByteVolume;
	
	namespace inpainting
	{
		class ComputeFront;
		class ComputeOrder;
		class ProgressReporter;

		class ComputeFrontInitializer
		{
		public:
			ComputeFrontInitializer( ComputeOrder* priority, Vec3ui patchSize, ProgressReporter* progress);
			~ComputeFrontInitializer();

			ComputeFront* generateComputeFront( );
			size_t getSizeOfTargetArea();

		protected:
			Vec3ui patchSize;
			ProgressReporter* progress;
			ComputeOrder* priority;
			size_t sizeOfTargetArea;
		};

	} // namespace inpainting
} // namespace ettention
