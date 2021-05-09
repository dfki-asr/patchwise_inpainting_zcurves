#pragma once

#include "libmmv/math/Vec3.h"

#include "Gradient3D.h"

class Volume;

namespace inpainting
{
	class InpaintingDebugParameters;

	class DataTerm
	{
	public:
		virtual float computeDataTermForOneVoxel(libmmv::Vec3ui voxelCoord, float alpha ) = 0;
		virtual void outputDebugVolumes(std::string pathToDebugFolder, unsigned int iterationNumber, InpaintingDebugParameters* parameters) = 0;
	};

} // namespace inpainting

