#pragma once

#include "ComputeFrontEntry.h"

namespace ettention
{
	class Volume;

	namespace inpainting
	{

		class ComputeFront
		{
		public:
			typedef std::set < ComputeFrontEntry, CompareByCoordinate >::iterator Iterator;

			ComputeFront( );
			~ComputeFront();

			size_t count();
			void updatePriority(Vec3ui coordinate, float newPriority);
			void addEntry(Vec3ui coordinate, float newPriority);
			void removeEntry(Vec3ui coordinate);
			ComputeFrontEntry popEntryWithHighestPriority();
			ComputeFrontEntry peekEntryWithHighestPriority();
			bool contains(Vec3ui coordinate);
			void clearFillfront();
			bool shouldUseBruteForce();

			Iterator begin();
			Iterator end();

			Volume* plotToVolume(Vec3ui volumeResolution);

		private:
			std::set < ComputeFrontEntry, CompareByCoordinate > entryByCoordinate;
			std::set < ComputeFrontEntry, CompareByPriority > entryByPriority;
			bool bruteForceTrigger = false;
		};

	} // namespace inpainting
} // namespace ettention
