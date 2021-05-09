#include "stdafx.h"

#include "Index.h"

namespace inpainting
{

	IndexCannotHandlePositionError::IndexCannotHandlePositionError(libmmv::Vec3i targetPatchCenter, libmmv::Vec3i minPosition, libmmv::Vec3i maxPosition)
		: std::runtime_error("unable to adjust target patch position " + targetPatchCenter.to_string() + " to manageable position, tried shifting from " + minPosition.to_string() + " to " + maxPosition.to_string() )
		, targetPatchCenter(targetPatchCenter)
	{
	}

	Index::~Index()
	{
	}

} // namespace

