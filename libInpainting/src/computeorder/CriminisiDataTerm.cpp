#include "stdafx.h"

#include "CriminisiDataTerm.h"

#include "model/volume/Volume.h"

namespace ettention
{
	namespace inpainting
	{

		CriminisiDataTerm::CriminisiDataTerm(Volume* dataVolume, Volume* maskVolume, Vec3ui patchSize)
			: dataVolume(dataVolume)
			, maskVolume(maskVolume)
			, patchSize(patchSize)
		{
 			volumeResolution = maskVolume->getProperties().getVolumeResolution();
			patchRadius = Vec3i( (int) floor( patchSize.x / 2 ), (int) floor( patchSize.y / 2 ), (int) floor( patchSize.z / 2 ) );
		}

		CriminisiDataTerm::~CriminisiDataTerm()
		{

		}

		float CriminisiDataTerm::computeDataTermForOneVoxel(Vec3ui voxelCoord, float alpha)
		{
			Vec3f imageGradient = Vec3f(0.0f, 0.0f, 0.0f);
			Vec3f maskNormal = Vec3f(0.0f, 0.0f, 0.0f);

			if ( voxelCoord.x < 1 || voxelCoord.y < 1 || voxelCoord.z < 1)
				return 0.0f;
			if (voxelCoord.x >= volumeResolution.x - 1 || voxelCoord.y >= volumeResolution.y - 1 || voxelCoord.z >= volumeResolution.z - 1)
				return 0.0f;

			imageGradient = computeImageGradient(voxelCoord);
			maskNormal = computeMaskNormal(voxelCoord);

			Vec3f product = Vec3f( imageGradient.x * maskNormal.x, imageGradient.y * maskNormal.y, imageGradient.z * maskNormal.z );
			float absSum = std::fabs( product.x + product.y + product.z );
			return absSum / alpha;
		}

		ettention::Vec3f CriminisiDataTerm::computeImageGradient(Vec3ui voxelCoord)
		{
			return gradient.computeOrthogonalGradient(dataVolume, voxelCoord, false);
		}

		ettention::Vec3f CriminisiDataTerm::computeMaskNormal(Vec3ui voxelCoord)
		{
			Vec3f maskNormal = gradient.computeGradient(maskVolume, voxelCoord, true );
			if (maskNormal.getLength() == 0.0f)
				return Vec3f(0.0f, 0.0f, 0.0f);;
			return doNormalize(maskNormal);
		}

		void CriminisiDataTerm::outputDebugVolumes(std::string pathToDebugFolder, unsigned int iterationNumber, InpaintingDebugParameters* parameters)
		{

		}

	} // namespace inpainting
} // namespace ettention