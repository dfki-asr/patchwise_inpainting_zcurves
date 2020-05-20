#include "stdafx.h"

#include "model/volume/ByteVolume.h"

#include "Gradient2D.h"
#include "../StatusFlags.h"

namespace ettention
{
	namespace inpainting
	{

		Gradient2D::Gradient2D()
		{
			initSobelStencil();
		}

		Gradient2D::~Gradient2D()
		{
		}

		Vec2f Gradient2D::computeGradient(Volume* volume, ByteVolume* mask, Vec3i coordinate, bool fromMask )
		{
			if (coordinate.x < 0 || coordinate.y < 0 || coordinate.z < 0 || coordinate.x >= (int) volume->getProperties().getVolumeResolution().x
				|| coordinate.y >= (int)volume->getProperties().getVolumeResolution().y || coordinate.z >= (int)volume->getProperties().getVolumeResolution().z)
				throw std::out_of_range("coordinates out of bounds");

			ByteVolume* byteVolume = nullptr;
			if (fromMask)
			{
				byteVolume = dynamic_cast<ByteVolume*>(volume);
				if (byteVolume == nullptr)
					throw std::runtime_error("gradient computation from mask requires byte volume");
			}

			Vec2f gradient = Vec2f(0.0f, 0.0f);

			float tmpValue = 0.0f;
			int i = 0;
			float centralValue = volume->getVoxelValue( coordinate );
			unsigned char status;

			for (int y = coordinate.y - 1; y <= coordinate.y + 1; y++)
			{
				for (int x = coordinate.x - 1; x <= coordinate.x + 1; x++)
				{
                    if ( x < 0 || y < 0 || x >= (int) volume->getProperties().getVolumeResolution().x || y >= (int) volume->getProperties().getVolumeResolution().y )
                        continue;

					size_t index = volume->calculateVoxelIndex(Vec3ui(x, y, coordinate.z));
					if (fromMask)
					{
						tmpValue = isStatusTargetRegion(byteVolume->nativeVoxelValue(index));
					}
					else
					{
						status = mask->nativeVoxelValue(index);
						if (status == TARGET_REGION || status == EMPTY_REGION)
							tmpValue = centralValue;
						else
							tmpValue = volume->getVoxelValue(index);
					}

					gradient += tmpValue * sobelStencil2D.value[i];
					i++;
				}
			}

			return gradient;
		}

		Vec2f Gradient2D::computeSimpleGradient(Volume* volume, ByteVolume* mask, Vec3i coordinate, bool fromMask)
		{
			if (coordinate.x < 0 || coordinate.y < 0 || coordinate.z < 0 || coordinate.x >= (int)volume->getProperties().getVolumeResolution().x
				|| coordinate.y >= (int)volume->getProperties().getVolumeResolution().y || coordinate.z >= (int)volume->getProperties().getVolumeResolution().z)
				throw std::out_of_range("coordinates out of bounds");

			ByteVolume* byteVolume = nullptr;
			if (fromMask)
			{
				byteVolume = dynamic_cast<ByteVolume*>(volume);
				if (byteVolume == nullptr)
					throw std::runtime_error("gradient computation from mask requires byte volume");
			}

			Vec2f gradient = Vec2f(0.0f, 0.0f);
			float centralValue = volume->getVoxelValue(coordinate);
			unsigned char status;
			size_t index;

			float value_minus_1 = centralValue;
			float value_plus_1 = centralValue;
			
			if (coordinate.y - 1 > 0)
			{
				index = volume->calculateVoxelIndex(Vec3ui(coordinate.x, coordinate.y - 1, coordinate.z));
				if (fromMask)
				{
					value_minus_1 = isStatusTargetRegion(byteVolume->nativeVoxelValue(index));
				}
				else
				{
					status = mask->nativeVoxelValue(index);
					if (status != TARGET_REGION && status != EMPTY_REGION)
						value_minus_1 = volume->getVoxelValue(index);
				}
			}

			if (coordinate.y + 1 < (int)volume->getProperties().getVolumeResolution().y)
			{
				index = volume->calculateVoxelIndex(Vec3ui(coordinate.x, coordinate.y + 1, coordinate.z));
				if (fromMask)
				{
					value_plus_1 = isStatusTargetRegion(byteVolume->nativeVoxelValue(index));
				}
				else
				{
					status = mask->nativeVoxelValue(index);
					if (status != TARGET_REGION && status != EMPTY_REGION)
						value_plus_1 = volume->getVoxelValue(index);
				}
			}

			gradient.y = 0.5f * (value_plus_1 - value_minus_1);

			if (coordinate.x - 1 > 0)
			{
				index = volume->calculateVoxelIndex(Vec3ui(coordinate.x - 1, coordinate.y, coordinate.z));
				if (fromMask)
				{
					value_minus_1 = isStatusTargetRegion(byteVolume->nativeVoxelValue(index));
				}
				else
				{
					status = mask->nativeVoxelValue(index);
					if (status != TARGET_REGION && status != EMPTY_REGION)
						value_minus_1 = volume->getVoxelValue(index);
				}
			}

			if (coordinate.x + 1 < (int)volume->getProperties().getVolumeResolution().x)
			{
				index = volume->calculateVoxelIndex(Vec3ui(coordinate.x + 1, coordinate.y, coordinate.z));
				if (fromMask)
				{
					value_plus_1 = isStatusTargetRegion(byteVolume->nativeVoxelValue(index));
				}
				else
				{
					status = mask->nativeVoxelValue(index);
					if (status != TARGET_REGION && status != EMPTY_REGION)
						value_plus_1 = volume->getVoxelValue(index);
				}
			}

			gradient.x = 0.5f * (value_plus_1 - value_minus_1);

			return gradient;
		}

/* NOT NEEDED?
		Vec2f Gradient2D::computeGradientOfPatch(Volume* volume, ByteVolume* mask, Vec3i centerCoordinateOfPatch, bool fromMask)
		{
			Vec3i sobelSize = Vec3i(3, 3, 3);
			Vec3i patchSizeHalf = (sobelSize / 2);

			if (centerCoordinateOfPatch.x < patchSizeHalf.x 
				|| centerCoordinateOfPatch.y < patchSizeHalf.y 
				|| centerCoordinateOfPatch.z < patchSizeHalf.z
				|| centerCoordinateOfPatch.x >= static_cast<int>(volume->getProperties().getVolumeResolution().x) - patchSizeHalf.x
				|| centerCoordinateOfPatch.y >= static_cast<int>(volume->getProperties().getVolumeResolution().y) - patchSizeHalf.y
				|| centerCoordinateOfPatch.z >= static_cast<int>(volume->getProperties().getVolumeResolution().z) - patchSizeHalf.z)
				throw std::runtime_error("coordinates out of patch");

			Vec2f gradient = Vec2f(0.0f, 0.0f);

			float tmpValue = 0.0f;
			int i = 0;
			for (int z = centerCoordinateOfPatch.z - patchSizeHalf.z; z <= centerCoordinateOfPatch.z + patchSizeHalf.z; z++)
			{
				for (int y = centerCoordinateOfPatch.y - patchSizeHalf.y; y <= centerCoordinateOfPatch.y + patchSizeHalf.y; y++)
				{
					for (int x = centerCoordinateOfPatch.x - patchSizeHalf.x; x <= centerCoordinateOfPatch.x + patchSizeHalf.x; x++)
					{
						size_t index = volume->calculateVoxelIndex(Vec3ui(x, y, z));

						if (!fromMask)
						{
							unsigned char status = mask->nativeVoxelValue(index);
							if (status == TARGET_REGION || status == EMPTY_REGION)
								continue;
						}

						tmpValue = volume->getVoxelValue(index);
						gradient += tmpValue * sobelStencil2D.value[i];
						i++;
					}
				}
			}

			return gradient;
		}
*/

		Vec2f Gradient2D::computeOrthogonalGradient(Volume* volume, ByteVolume* mask, Vec3i coordinate, bool fromMask)
		{
			Vec2f gradient = computeGradient(volume, mask, coordinate, fromMask);
			return Vec2f(gradient.y, -1.0f * gradient.x);
		}

		float Gradient2D::isStatusTargetRegion(unsigned char statusValue)
		{
			if (statusValue == TARGET_REGION)
				return 0.0f;
			return 1.0f;
		}

		void Gradient2D::initSobelStencil()
		{
			sobelStencil2D = 
			{
				Vec2f( 3.0f,  3.0f), // x == -1, y == -1
				Vec2f( 0.0f, 10.0f), // x == 0
				Vec2f(-3.0f,  3.0f), // x == 1
				Vec2f(10.0f,  0.0f), // x == -1, y == 0
				Vec2f( 0.0f,  0.0f), // x == 0
				Vec2f(-10.0f,  0.0f), // x == 1
				Vec2f( 3.0f, -3.0f), // x == -1, y == 1
				Vec2f( 0.0f, -10.0f), // x == 0
				Vec2f(-3.0f, -3.0f)  // x == 1
			};
		};

	} // namespace inpainting
} // namespace ettention