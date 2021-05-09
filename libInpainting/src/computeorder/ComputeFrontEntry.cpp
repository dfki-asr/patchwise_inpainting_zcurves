#include "stdafx.h"

#include "ComputeFrontEntry.h"

namespace inpainting
{

	ComputeFrontEntry::ComputeFrontEntry(libmmv::Vec3ui coordinate, float priority)
		: priority(priority)
		, coordinate(coordinate)
	{

	}

	ComputeFrontEntry::~ComputeFrontEntry()
	{
	}

} // namespace inpainting