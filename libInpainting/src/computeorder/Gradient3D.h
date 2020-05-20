#pragma once

#include "math/Vec3.h"

namespace ettention
{

	class Volume;

	namespace inpainting
	{
		class Gradient3D
		{
		public:
			Gradient3D();
			~Gradient3D();

			Vec3f computeGradient(Volume* volume, Vec3i coordinate, bool fromMask );
			Vec3f computeGradientOfPatch(Volume* volume, Vec3i coordinate);
			float compute3DOrientation(Vec3f gradient);
			float computeAverageOrientationOfPatch(Volume* volume, Vec3i centerCoordinateOfPatch, Vec3i patchSize);
			Vec3f computeOrthogonalGradient(Volume* volume, Vec3i coordinate, bool fromMask);

		protected:
			float isStatusTargetRegion(unsigned char statusValue);
			void initSobelStencil();

			float nanToValue( float oldValue, float newValue );

		protected:
			struct SobelStencil
			{
				Vec3f value[27];
			} sobelStencil3D;
		};

	} // namespace inpainting
} // namespace ettention
