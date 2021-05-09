#pragma once

#include "ComputeFrontEntry.h"

namespace libmmv 
{
	class Volume;
}

namespace inpainting
{

	class ComputeFront
	{
	public:
		typedef std::set < ComputeFrontEntry, CompareByCoordinate >::iterator Iterator;

		ComputeFront( );
		~ComputeFront();

		size_t count();
		void updatePriority(libmmv::Vec3ui coordinate, float newPriority);
		void addEntry(libmmv::Vec3ui coordinate, float newPriority);
		void removeEntry(libmmv::Vec3ui coordinate);
		ComputeFrontEntry popEntryWithHighestPriority();
		ComputeFrontEntry peekEntryWithHighestPriority();
		bool contains(libmmv::Vec3ui coordinate);
		void clearFillfront();
		bool shouldUseBruteForce();

		Iterator begin();
		Iterator end();

		libmmv::Volume* plotToVolume(libmmv::Vec3ui volumeResolution);

	private:
		std::set < ComputeFrontEntry, CompareByCoordinate > entryByCoordinate;
		std::set < ComputeFrontEntry, CompareByPriority > entryByPriority;
		bool bruteForceTrigger = false;
	};

} // namespace inpainting
