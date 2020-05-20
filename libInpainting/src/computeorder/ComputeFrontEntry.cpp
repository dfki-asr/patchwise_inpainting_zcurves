#include "stdafx.h"

#include "ComputeFrontEntry.h"

namespace ettention
{
	namespace inpainting
	{

		ComputeFrontEntry::ComputeFrontEntry(Vec3ui coordinate, float priority)
			: priority(priority)
			, coordinate(coordinate)
		{

		}

		ComputeFrontEntry::~ComputeFrontEntry()
		{
		}

	} // namespace inpainting
} // namespace ettention