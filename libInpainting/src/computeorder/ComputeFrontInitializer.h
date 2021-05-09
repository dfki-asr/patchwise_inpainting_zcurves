#pragma once

#include "libmmv/math/Vec3.h"

namespace libmmv
{
	class Volume;
	class ByteVolume;
}

namespace inpainting
{
	class ComputeFront;
	class ComputeOrder;
	class ProgressReporter;

	class ComputeFrontInitializer
	{
	public:
		ComputeFrontInitializer( ComputeOrder* priority, libmmv::Vec3ui patchSize, ProgressReporter* progress);
		~ComputeFrontInitializer();

		ComputeFront* generateComputeFront( );
		size_t getSizeOfTargetArea();

	protected:
		libmmv::Vec3ui patchSize;
		ProgressReporter* progress;
		ComputeOrder* priority;
		size_t sizeOfTargetArea;
	};

} // namespace inpainting

