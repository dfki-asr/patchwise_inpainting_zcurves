#pragma once

#include "libmmv/math/Vec3.h"

namespace libmmv
{
	class Volume;
}

namespace inpainting
{
	class Gradient3D
	{
	public:
		Gradient3D();
		~Gradient3D();

		libmmv::Vec3f computeGradient(libmmv::Volume* volume, libmmv::Vec3i coordinate, bool fromMask );
		libmmv::Vec3f computeGradientOfPatch(libmmv::Volume* volume, libmmv::Vec3i coordinate);
		float compute3DOrientation(libmmv::Vec3f gradient);
		float computeAverageOrientationOfPatch(libmmv::Volume* volume, libmmv::Vec3i centerCoordinateOfPatch, libmmv::Vec3i patchSize);
		libmmv::Vec3f computeOrthogonalGradient(libmmv::Volume* volume, libmmv::Vec3i coordinate, bool fromMask);

	protected:
		float isStatusTargetRegion(unsigned char statusValue);
		void initSobelStencil();

		float nanToValue( float oldValue, float newValue );

	protected:
		struct SobelStencil
		{
			libmmv::Vec3f value[27];
		} sobelStencil3D;
	};

} // namespace inpainting

