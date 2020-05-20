#pragma once

#include "math/Vec2.h"

namespace ettention
{
	class Volume;
	class ByteVolume;

	namespace inpainting
	{
		class Gradient2D
		{
		public:
			Gradient2D();
			~Gradient2D();

			Vec2f computeGradient(Volume* volume, ByteVolume* mask, Vec3i coordinate, bool fromMask );
			Vec2f computeSimpleGradient(Volume* volume, ByteVolume* mask, Vec3i coordinate, bool fromMask);
// NOT NEEDED?			Vec2f computeGradientOfPatch(Volume* volume, ByteVolume* mask, Vec3i coordinate, bool fromMask);
			Vec2f computeOrthogonalGradient(Volume* volume, ByteVolume* mask, Vec3i coordinate, bool fromMask);

		protected:
			float isStatusTargetRegion(unsigned char statusValue);
			void initSobelStencil();

		protected:
			struct SobelStencil
			{
				Vec2f value[9];
			} sobelStencil2D;
		};

	} // namespace inpainting
} // namespace ettention
