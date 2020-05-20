#include "stdafx.h"

#include "Index.h"

namespace ettention
{
    namespace inpainting
    {

		IndexCannotHandlePositionError::IndexCannotHandlePositionError(Vec3i targetPatchCenter, Vec3i minPosition, Vec3i maxPosition)
			: std::runtime_error("unable to adjust target patch position " + targetPatchCenter.to_string() + " to manageable position, tried shifting from " + minPosition.to_string() + " to " + maxPosition.to_string() )
			, targetPatchCenter(targetPatchCenter)
		{
		}

		Index::~Index()
		{
		}

	} // namespace
} // namespace
