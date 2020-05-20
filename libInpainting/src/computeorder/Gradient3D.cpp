#include "stdafx.h"

#include "model/volume/ByteVolume.h"

#include "Gradient3D.h"
#include "../StatusFlags.h"

namespace ettention
{
	namespace inpainting
	{

		Gradient3D::Gradient3D()
		{
			initSobelStencil();
		}

		Gradient3D::~Gradient3D()
		{
		}

		Vec3f Gradient3D::computeGradient(Volume* volume, Vec3i coordinate, bool fromMask )
		{
			if (coordinate.x < 0 || coordinate.y < 0 || coordinate.z < 0 || coordinate.x >= (int) volume->getProperties().getVolumeResolution().x
				|| coordinate.y >= (int)volume->getProperties().getVolumeResolution().y || coordinate.z >= (int)volume->getProperties().getVolumeResolution().z)
				throw std::runtime_error("coordinates out of bounds");

			ByteVolume* byteVolume = nullptr;
			if (fromMask)
			{
				byteVolume = dynamic_cast<ByteVolume*>(volume);
				if (byteVolume == nullptr)
					throw std::runtime_error("gradient computation from mask requires byte volume");
			}

			Vec3f gradient = Vec3f(0.0f, 0.0f, 0.0f);

			float tmpValue = 0.0f;
			int i = 0;
			for (int z = coordinate.z - 1; z <= coordinate.z + 1; z++)
			{
				for (int y = coordinate.y - 1; y <= coordinate.y + 1; y++)
				{
					for (int x = coordinate.x - 1; x <= coordinate.x + 1; x++)
					{
                        if ( x < 0 || y < 0 || z < 0 || x >= (int) volume->getProperties().getVolumeResolution().x || y >= (int) volume->getProperties().getVolumeResolution().y || z >= (int) volume->getProperties().getVolumeResolution().z)
                            continue;

						if (fromMask)
						{
							size_t index = byteVolume->calculateVoxelIndex(Vec3ui(x, y, z));
							tmpValue = isStatusTargetRegion(byteVolume->nativeVoxelValue(index));
						}
						else
						{
							size_t index = volume->calculateVoxelIndex(Vec3ui(x, y, z));
							tmpValue = nanToValue(volume->getVoxelValue(index), 1.0f);
						}
						gradient += tmpValue * sobelStencil3D.value[i];
						i++;
					}
				}
			}

			return gradient;
		}

		Vec3f Gradient3D::computeGradientOfPatch(Volume* volume, Vec3i centerCoordinateOfPatch)
		{
			Vec3i sobelSize = Vec3i(3, 3, 3);
			Vec3i patchSizeHalf = (sobelSize / 2);

			if (centerCoordinateOfPatch.x < patchSizeHalf.x 
				|| centerCoordinateOfPatch.y < patchSizeHalf.y 
				|| centerCoordinateOfPatch.z < patchSizeHalf.z
				|| centerCoordinateOfPatch.x >= static_cast<int>(volume->getProperties().getVolumeResolution().x) - patchSizeHalf.x
				|| centerCoordinateOfPatch.y >= static_cast<int>(volume->getProperties().getVolumeResolution().y) - patchSizeHalf.y
				|| centerCoordinateOfPatch.z >= static_cast<int>(volume->getProperties().getVolumeResolution().z) - patchSizeHalf.z)
				throw std::out_of_range("coordinates out of patch");

			Vec3f gradient = Vec3f(0.0f, 0.0f, 0.0f);

			float tmpValue = 0.0f;
			int i = 0;
			for (int z = centerCoordinateOfPatch.z - patchSizeHalf.z; z <= centerCoordinateOfPatch.z + patchSizeHalf.z; z++)
			{
				for (int y = centerCoordinateOfPatch.y - patchSizeHalf.y; y <= centerCoordinateOfPatch.y + patchSizeHalf.y; y++)
				{
					for (int x = centerCoordinateOfPatch.x - patchSizeHalf.x; x <= centerCoordinateOfPatch.x + patchSizeHalf.x; x++)
					{
						size_t index = volume->calculateVoxelIndex(Vec3ui(x, y, z));
						tmpValue = volume->getVoxelValue(index);
						gradient += tmpValue * sobelStencil3D.value[i];
						i++;
					}
				}
			}

			return gradient;
		}

		float Gradient3D::isStatusTargetRegion(unsigned char statusValue)
		{
			if (statusValue == TARGET_REGION)
				return 0.0f;
			return 1.0f;
		}

		void Gradient3D::initSobelStencil()
		{
			sobelStencil3D = 
			{
				Vec3f(1.0f, 1.0f, 1.0f),
				Vec3f(0.0f, 2.0f, 2.0f),
				Vec3f(-1.0f, 1.0f, 1.0f),
				Vec3f(2.0f, 0.0f, 2.0f),
				Vec3f(0.0f, 0.0f, 4.0f), // 5
				Vec3f(-2.0f, 0.0f, 2.0f),
				Vec3f(1.0f, -1.0f, 1.0f),
				Vec3f(0.0f, -2.0f, 2.0f),
				Vec3f(-1.0f, -1.0f, 1.0f),
				Vec3f(2.0f, 2.0f, 0.0f), // 10
				Vec3f(0.0f, 4.0f, 0.0f),
				Vec3f(-2.0f, 2.0f, 0.0f),
				Vec3f(4.0f, 0.0f, 0.0f),
				Vec3f(0.0f, 0.0f, 0.0f),
				Vec3f(-4.0f, 0.0f, 0.0f), // 15
				Vec3f(2.0f, -2.0f, 0.0f),
				Vec3f(0.0f, -4.0f, 0.0f),
				Vec3f(-2.0f, -2.0f, 0.0f),
				Vec3f(1.0f, 1.0f, -1.0f),
				Vec3f(0.0f, 2.0f, -2.0f), // 20
				Vec3f(-1.0f, 1.0f, -1.0f),
				Vec3f(2.0f, 0.0f, -2.0f),
				Vec3f(0.0f, 0.0f, -4.0f),
				Vec3f(-2.0f, 0.0f, -2.0f),
				Vec3f(1.0f, -1.0f, -1.0f), // 25
				Vec3f(0.0f, -2.0f, -2.0f),
				Vec3f(-1.0f, -1.0f, -1.0f) 
			};
		};

		float Gradient3D::nanToValue(float oldValue, float newValue)
		{
			if (std::isnan(oldValue))
				return newValue;
			else
				return oldValue;
		}

		Vec3f Gradient3D::computeOrthogonalGradient(Volume* volume, Vec3i coordinate, bool fromMask)
		{
			Vec3f gradient = computeGradient(volume, coordinate, fromMask);

			if (gradient.x == 0.0f)
				return Vec3f(0.0f, -1.0f * gradient.z, gradient.y);

			return Vec3f(gradient.y, -1.0f * gradient.x, 0.0f);
		}

	} // namespace inpainting
} // namespace ettention