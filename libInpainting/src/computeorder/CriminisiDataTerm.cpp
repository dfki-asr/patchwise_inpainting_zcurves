#include "stdafx.h"

#include "CriminisiDataTerm.h"

#include "libmmv/model/volume/Volume.h"

namespace inpainting
{

	CriminisiDataTerm::CriminisiDataTerm(libmmv::Volume* dataVolume, libmmv::Volume* maskVolume, libmmv::Vec3ui patchSize)
		: dataVolume(dataVolume)
		, maskVolume(maskVolume)
		, patchSize(patchSize)
	{
 		volumeResolution = maskVolume->getProperties().getVolumeResolution();
		patchRadius = libmmv::Vec3i( (int) floor( patchSize.x / 2 ), (int) floor( patchSize.y / 2 ), (int) floor( patchSize.z / 2 ) );
	}

	CriminisiDataTerm::~CriminisiDataTerm()
	{

	}

	float CriminisiDataTerm::computeDataTermForOneVoxel(libmmv::Vec3ui voxelCoord, float alpha)
	{
		libmmv::Vec3f imageGradient = libmmv::Vec3f(0.0f, 0.0f, 0.0f);
		libmmv::Vec3f maskNormal = libmmv::Vec3f(0.0f, 0.0f, 0.0f);

		if ( voxelCoord.x < 1 || voxelCoord.y < 1 || voxelCoord.z < 1)
			return 0.0f;
		if (voxelCoord.x >= volumeResolution.x - 1 || voxelCoord.y >= volumeResolution.y - 1 || voxelCoord.z >= volumeResolution.z - 1)
			return 0.0f;

		imageGradient = computeImageGradient(voxelCoord);
		maskNormal = computeMaskNormal(voxelCoord);

		libmmv::Vec3f product = libmmv::Vec3f( imageGradient.x * maskNormal.x, imageGradient.y * maskNormal.y, imageGradient.z * maskNormal.z );
		float absSum = std::fabs( product.x + product.y + product.z );
		return absSum / alpha;
	}

	libmmv::Vec3f CriminisiDataTerm::computeImageGradient(libmmv::Vec3ui voxelCoord)
	{
		return gradient.computeOrthogonalGradient(dataVolume, voxelCoord, false);
	}

	libmmv::Vec3f CriminisiDataTerm::computeMaskNormal(libmmv::Vec3ui voxelCoord)
	{
		libmmv::Vec3f maskNormal = gradient.computeGradient(maskVolume, voxelCoord, true );
		if (maskNormal.getLength() == 0.0f)
			return libmmv::Vec3f(0.0f, 0.0f, 0.0f);;
		return doNormalize(maskNormal);
	}

	void CriminisiDataTerm::outputDebugVolumes(std::string pathToDebugFolder, unsigned int iterationNumber, InpaintingDebugParameters* parameters)
	{

	}

} // namespace inpainting
