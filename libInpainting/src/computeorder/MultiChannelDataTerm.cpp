#include "stdafx.h"

#include "MultiChannelDataTerm.h"
#include "model/volume/ByteVolume.h"

#include "setup/parameterset/OutputParameterSet.h"
#include "setup/InpaintingDebugParameters.h"

namespace ettention
{
	namespace inpainting
	{

		MultiChannelDataTerm::MultiChannelDataTerm(Volume* dataVolume, ByteVolume* maskVolume, Vec3ui patchSize)
			: dataVolume(dataVolume)
			, maskVolume(maskVolume)
			, patchSize(patchSize)
		{
			volumeResolution = maskVolume->getProperties().getVolumeResolution();
			patchRadius = Vec3i((int)floor(patchSize.x / 2), (int)floor(patchSize.y / 2), (int)floor(patchSize.z / 2));
		}

		MultiChannelDataTerm::~MultiChannelDataTerm()
		{
		}

		float MultiChannelDataTerm::computeDataTermForOneVoxel(Vec3ui voxelCoord, float alpha)
		{
			if (voxelCoord.x < 1 || voxelCoord.y < 1 )
				return 0.0f;
			if (voxelCoord.x >= volumeResolution.x - 1 || voxelCoord.y >= volumeResolution.y - 1 )
				return 0.0f;
			if (voxelCoord.z != patchSize.z / 2 )
				return 0.0f;

			Vec2f imageGradient = Vec2f(0.0f, 0.0f);
			Vec2f maskNormal = Vec2f(0.0f, 0.0f);
			maskNormal = computeMaskNormal(voxelCoord);

			Vec3ui voxelCoord2D = Vec3ui(0, 0, 0);
			for (int z = 0; z < patchSize.z; z++)
			{
				voxelCoord2D = Vec3ui(voxelCoord.x, voxelCoord.y, z);
				imageGradient += computeImageGradient(voxelCoord2D);
			}

			imageGradient /= ((float) patchSize.z * alpha);

			Vec2f product = Vec2f(imageGradient.x * maskNormal.x, imageGradient.y * maskNormal.y);

			return (std::fabs(product.x + product.y) + 0.001f);
		}

		ettention::Vec2f MultiChannelDataTerm::computeImageGradient(Vec3ui voxelCoord)
		{
			Vec2f gradValue = gradient.computeOrthogonalGradient(dataVolume, maskVolume, voxelCoord, false );
			return gradValue;
		}

		ettention::Vec2f MultiChannelDataTerm::computeMaskNormal(Vec3ui voxelCoord)
		{
			Vec2f maskNormal = gradient.computeGradient(maskVolume, maskVolume, voxelCoord, true);
			if (maskNormal.getLength() == 0.0f)
				return Vec2f(0.0f, 0.0f);;
			return doNormalize(maskNormal);
		}

		void MultiChannelDataTerm::outputDebugVolumes(std::string pathToDebugFolder, unsigned int iterationNumber, InpaintingDebugParameters* parameters)
		{
			OutputParameterSet parameter;
			VolumeSerializer serializer;

			if (parameters->shouldOutputGradient())
			{
				std::string fileNamePriority = pathToDebugFolder + "gradient_iteration_" + std::to_string((int)iterationNumber) + ".mrc";
				serializer.write(plotGradientToVolume(), fileNamePriority, parameter.getVoxelType(), parameter.getOrientation() );
			}
			if (parameters->shouldOutputMaskNormal())
			{
				std::string fileNamePriority = pathToDebugFolder + "masknormal_iteration_" + std::to_string((int)iterationNumber) + ".mrc";
				serializer.write(plotMaskNormalToVolume(), fileNamePriority, parameter.getVoxelType(), parameter.getOrientation() );
			}
		}

		Volume* MultiChannelDataTerm::plotGradientToVolume()
		{
			Vec3ui gradientDebugResolution = volumeResolution;
			gradientDebugResolution.z *= 2;

			Volume* volume = new FloatVolume(gradientDebugResolution, 0.0f);

			Vec3i coord(0, 0, 0);
			for (coord.z = 0; coord.z < (int)volumeResolution.z; coord.z++)
			{
				for (coord.y = 0; coord.y < (int)volumeResolution.y; coord.y++)
				{
					for (coord.x = 0; coord.x < (int)volumeResolution.x; coord.x++)
					{
						Vec2f gradient = computeImageGradient(coord);
						volume->setVoxelToValue(Vec3i(coord.x, coord.y, 2 * coord.z), gradient.x);
						volume->setVoxelToValue(Vec3i(coord.x, coord.y, 2 * coord.z + 1), gradient.y);
					}
				}
			}
			return volume;
		}

		Volume* MultiChannelDataTerm::plotMaskNormalToVolume()
		{
			Vec3ui gradientDebugResolution = volumeResolution;
			gradientDebugResolution.z *= 2;

			Volume* volume = new FloatVolume(gradientDebugResolution, 0.0f);

			Vec3i coord(0, 0, 0);
			for (coord.z = 0; coord.z < (int)volumeResolution.z; coord.z++)
			{
				for (coord.y = 0; coord.y < (int)volumeResolution.y; coord.y++)
				{
					for (coord.x = 0; coord.x < (int)volumeResolution.x; coord.x++)
					{
						Vec2f gradient = computeMaskNormal(coord);
						volume->setVoxelToValue(Vec3i(coord.x, coord.y, 2 * coord.z), gradient.x);
						volume->setVoxelToValue(Vec3i(coord.x, coord.y, 2 * coord.z + 1), gradient.y);
					}
				}
			}
			return volume;
		}


	} // namespace inpainting
} // namespace ettention