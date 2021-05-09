#pragma once

#include "libmmv/math/Vec2.h"

namespace libmmv
{
	class Volume;
	class ByteVolume;
}

namespace inpainting
{
	class Gradient2D
	{
	public:
		Gradient2D();
		~Gradient2D();

		libmmv::Vec2f computeGradient(libmmv::Volume* volume, libmmv::ByteVolume* mask, libmmv::Vec3i coordinate, bool fromMask );
		libmmv::Vec2f computeSimpleGradient(libmmv::Volume* volume, libmmv::ByteVolume* mask, libmmv::Vec3i coordinate, bool fromMask);
		libmmv::Vec2f computeOrthogonalGradient(libmmv::Volume* volume, libmmv::ByteVolume* mask, libmmv::Vec3i coordinate, bool fromMask);

	protected:
		float isStatusTargetRegion(unsigned char statusValue);
		void initSobelStencil();

	protected:
		struct SobelStencil
		{
			libmmv::Vec2f value[9];
		} sobelStencil2D;
	};

} // namespace inpainting

