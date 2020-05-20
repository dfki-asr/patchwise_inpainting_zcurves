#include "stdafx.h"

#include "ComputeFront.h"

#include "model/volume/FloatVolume.h"

namespace ettention
{
	namespace inpainting
	{

		ComputeFront::ComputeFront()
		{

		}

		ComputeFront::~ComputeFront()
		{

		}

		size_t ComputeFront::count()
		{
			if (entryByCoordinate.size() != entryByPriority.size())
				throw std::runtime_error("compute front sanity check failed!");
			return entryByCoordinate.size();
		}

		void ComputeFront::updatePriority(Vec3ui coordinate, float newPriority)
		{
			removeEntry( coordinate );
			addEntry( coordinate, newPriority );
		}

		void ComputeFront::addEntry(Vec3ui coordinate, float priority)
		{
			count();
			ComputeFrontEntry entry = ComputeFrontEntry(coordinate, priority);
			entryByCoordinate.insert(entry);
			entryByPriority.insert(entry);
			count();
		}

		void ComputeFront::removeEntry(Vec3ui coordinate)
		{
			auto it = entryByCoordinate.find(ComputeFrontEntry(coordinate, 0.0));
			if (it == entryByCoordinate.end())
				throw std::runtime_error("coordinate not found in compute front");
			float priority = it->priority;
			entryByCoordinate.erase(it);
			entryByPriority.erase(ComputeFrontEntry(coordinate, priority));
		}

		ComputeFrontEntry ComputeFront::popEntryWithHighestPriority()
		{
			if (count() == 0)
				throw std::runtime_error("ComputeFront is empty, cannot pop");
			auto it = entryByPriority.rbegin();
			ComputeFrontEntry entry = *it;

			if (it->priority == std::numeric_limits<float>::min())
				clearFillfront();
			else
				removeEntry(entry.coordinate);

			return entry;
		}

		ettention::inpainting::ComputeFrontEntry ComputeFront::peekEntryWithHighestPriority()
		{
			if (count() == 0)
				throw std::runtime_error("ComputeFront is empty, cannot pop");
			auto it = entryByPriority.rbegin();
			if(it->priority == std::numeric_limits<float>::min())
				clearFillfront();
			return *it;
		}

		bool  ComputeFront::contains(Vec3ui coordinate)
		{
			return entryByCoordinate.find(ComputeFrontEntry(coordinate, 0.0)) != entryByCoordinate.end();
		}

		void ComputeFront::clearFillfront()
		{
			bruteForceTrigger = true;

			std::cout << "Ignored " << entryByCoordinate.size() << " entries!" << std::endl;
			entryByCoordinate.clear();
			entryByPriority.clear();
		}

		bool ComputeFront::shouldUseBruteForce()
		{
			return bruteForceTrigger;
		}

		ComputeFront::Iterator ComputeFront::begin()
		{
			return entryByCoordinate.begin();
		}

		ComputeFront::Iterator ComputeFront::end()
		{
			return entryByCoordinate.end();
		}

		Volume* ComputeFront::plotToVolume(Vec3ui volumeResolution)
		{
			Volume* volume = new FloatVolume( volumeResolution, 0.0f );
			for (auto it = entryByCoordinate.begin(); it != entryByCoordinate.end(); ++it)
			{
				const Vec3ui coord = it->coordinate;
				volume->setVoxelToValue(coord, 255.0f);
			}
			return volume;
		}

	} // namespace inpainting
} // namespace ettention