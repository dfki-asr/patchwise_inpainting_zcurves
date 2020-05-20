#pragma once

#include "math/Vec3.h"

namespace ettention
{
	namespace inpainting
	{

		class ComputeFrontEntry
		{
		public:
			ComputeFrontEntry(Vec3ui coordinate, float priority);
			~ComputeFrontEntry();

			Vec3ui coordinate;
			float priority;
		};

		class CompareByPriority
		{
		public:
			// implements less
			bool operator()(const ComputeFrontEntry& a, const ComputeFrontEntry& b) const
			{
				if (a.priority < b.priority)
					return true;
				if (a.priority > b.priority)
					return false;
				return a.coordinate < b.coordinate;
			}
		};

		class CompareByCoordinate
		{
		public:
			// implements less
			bool operator()(const ComputeFrontEntry& a, const ComputeFrontEntry& b) const
			{
				return a.coordinate < b.coordinate;
			}
		};

	} // namespace inpainting
} // namespace ettention
