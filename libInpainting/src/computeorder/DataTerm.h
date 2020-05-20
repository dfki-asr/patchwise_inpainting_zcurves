#pragma once

#include "math/Vec3.h"

#include "Gradient3D.h"

namespace ettention
{
	class Volume;

	namespace inpainting
	{
		class InpaintingDebugParameters;

		class DataTerm
		{
		public:
			virtual float computeDataTermForOneVoxel( Vec3ui voxelCoord, float alpha ) = 0;
			virtual void outputDebugVolumes(std::string pathToDebugFolder, unsigned int iterationNumber, InpaintingDebugParameters* parameters) = 0;
		};

	} // namespace inpainting
} // namespace ettention
