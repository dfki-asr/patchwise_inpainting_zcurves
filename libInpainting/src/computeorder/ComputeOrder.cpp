#include "stdafx.h"

#include "ComputeOrder.h"

#include "libmmv/model/volume/ByteVolume.h"
#include "libmmv/model/volume/FloatVolume.h"

#include "libCFG/setup/parameterset/OutputParameterSet.h"
#include "setup/InpaintingDebugParameters.h"

#include "StatusFlags.h"
#include "Confidence.h"
#include "CriminisiDataTerm.h"
#include "ComputeFront.h"
#include "Gradient3D.h"
#include "ComputeFrontInitializer.h"
#include "libmmv/algorithm/Coordinates.h"

namespace inpainting
{

    ComputeOrder::ComputeOrder(libmmv::ByteVolume* mask)
		: front(nullptr)
        , mask(mask)
    {
        if (this->mask == nullptr)
            throw std::runtime_error("priority requires volume");
		volumeResolution = mask->getProperties().getVolumeResolution();
		initNeighborOffset();
    }

    ComputeOrder::~ComputeOrder()
    {
    }

    bool ComputeOrder::isEmpty()
    {
		ensureInitialized();

        return front->count() == 0;
    }

    void ComputeOrder::addCoordinateToFront(libmmv::Vec3ui coordinate)
    {
		ensureInitialized();

        if (front->contains(coordinate)) 
            return;
        float priorityValue = computePriorityForVoxel( coordinate );
        front->addEntry(coordinate, priorityValue);
    }

    void ComputeOrder::removeCoordinateFromFront(libmmv::Vec3ui coordinate)
    {
		ensureInitialized();

        if (!front->contains(coordinate))
            return;

        front->removeEntry(coordinate);
    }

	void ComputeOrder::rememberPatchForLater(libmmv::Vec3i coordinate)
	{
		if (front->contains(coordinate))
			front->removeEntry(coordinate);
		front->addEntry(coordinate, std::numeric_limits<float>::min() );
	}

	void ComputeOrder::outputDebugVolumes(std::string pathToDebugFolder, unsigned int iterationNumber, InpaintingDebugParameters* parameters)
    {
        libCFG::OutputParameterSet parameter;
        libmmv::VolumeSerializer serializer;

		if (parameters->shouldOutputFillFront())
		{
			std::string fileNameFront = pathToDebugFolder + "fillfront_iteration_" + std::to_string((int)iterationNumber) + ".mrc";
			serializer.write( plotFillFrontToVolume(), fileNameFront, parameter.getVoxelType(), parameter.getOrientation() );
		}
    }

    void ComputeOrder::progressFront( libmmv::BoundingBox3i region)
    {
        libmmv::BoundingBox3i border(libmmv::Vec3i(0, 0, 0), volumeResolution - libmmv::Vec3i(1,1,1) );

		auto updateRegion = region;
		updateRegion.setMin(updateRegion.getMin() - libmmv::Vec3i(1, 1, 1));
		updateRegion.setMax(updateRegion.getMax() + libmmv::Vec3i(1, 1, 1));
		updateRegion.cropToBorder(border);

		removeRegionFromFront(updateRegion);
        growFront(updateRegion);
    }

    void ComputeOrder::removeRegionFromFront(libmmv::BoundingBox3i region)
    {
        libmmv::Vec3i coord;
        for (coord.z = region.getMin().z; coord.z <= region.getMax().z; coord.z++)
        {
            for (coord.y = region.getMin().y; coord.y <= region.getMax().y; coord.y++)
            {
                for (coord.x = region.getMin().x; coord.x <= region.getMax().x; coord.x++)
                {
                    removeCoordinateFromFront(coord);
                }
            }
        }
    }

    int ComputeOrder::growFront(libmmv::BoundingBox3i region)
    {
        libmmv::Vec3i coord;
		int sizeOfTargetArea = 0;
        for (coord.z = region.getMin().z; coord.z <= region.getMax().z; coord.z++)
        {
            for (coord.y = region.getMin().y; coord.y <= region.getMax().y; coord.y++)
            {
                for (coord.x = region.getMin().x; coord.x <= region.getMax().x; coord.x++)
                {
					size_t index = mask->getVoxelIndex(coord);
					unsigned char status = mask->nativeVoxelValue(index);
						
					if (status == TARGET_REGION)
						sizeOfTargetArea++;

					if (status == TARGET_REGION || status == EMPTY_REGION)
						continue;

                    if ( isAnyNeighborInTargetRegion(coord) )
                    {
                        addCoordinateToFront(coord);
                    }
                }
            }
        }
		return sizeOfTargetArea;
    }

	bool ComputeOrder::isAnyNeighborInTargetRegion(libmmv::Vec3i coordinate)
	{
        libmmv::BoundingBox3i safeBoundingBox = mask->getProperties().getMemoryBoundingBox();
		safeBoundingBox.shrink(1);
		if (safeBoundingBox.isInside(coordinate))
			return isAnyNeighborInTargetRegion_FastCheck( coordinate );

		return isAnyNeighborInTargetRegion_BorderCheck( coordinate );
	}

	bool ComputeOrder::isAnyNeighborInTargetRegion_FastCheck(libmmv::Vec3i coordinate)
	{
		size_t baseIndex = mask->getVoxelIndex( coordinate );
		for (size_t i = 0; i < neighborOffset.size(); i++)
		{
			size_t index = baseIndex + neighborOffset[i];
			if (mask->nativeVoxelValue(index) == TARGET_REGION)
				return true;
		}
		return false;
	}

	bool ComputeOrder::isAnyNeighborInTargetRegion_BorderCheck(libmmv::Vec3i coordinate)
	{
        libmmv::BoundingBox3i boundingBox = mask->getProperties().getMemoryBoundingBox();
        libmmv::Vec3i offset;
		for (offset.z = -1; offset.z <= 1; offset.z++)
		{
			for (offset.y = -1; offset.y <= 1; offset.y++)
			{
				for (offset.x = -1; offset.x <= 1; offset.x++)
				{
                    libmmv::Vec3i pixelCoord = coordinate + offset;
					if (!boundingBox.isInside(pixelCoord))
						continue;

					size_t index = mask->getVoxelIndex(pixelCoord);
					if (mask->nativeVoxelValue(index) == TARGET_REGION)
						return true;
				}
			}
		}
		return false;
	}

	void ComputeOrder::updatePriorityInRegion(libmmv::Vec3ui from, libmmv::Vec3ui to)
    {
        libmmv::Vec3ui coord;
        for (coord.z = from.z; coord.z <= to.z; coord.z++)
        {
            for (coord.y = from.y; coord.y <= to.y; coord.y++)
            {
                for (coord.x = from.x; coord.x <= to.x; coord.x++)
                {
                    updatePriority(coord);
                }
            }
        }
    }

    libmmv::ByteVolume* ComputeOrder::getMask()
    {
        return mask;
    }

    void ComputeOrder::updatePriority(libmmv::Vec3ui coordinate)
    {
		ensureInitialized();

        if (!front->contains(coordinate))
            return;

        float priorityValue = computePriorityForVoxel(coordinate);
        front->updatePriority(coordinate, priorityValue);
    }

	bool ComputeOrder::shouldUseBruteForce()
	{
		return front->shouldUseBruteForce();
	}

	size_t ComputeOrder::getSizeOfTargetArea()
    {
        return sizeOfTargetArea;
    }

    libmmv::Volume* ComputeOrder::plotPriorityToVolume()
    {
        libmmv::Vec3ui volumeResolution = mask->getProperties().getVolumeResolution();
        libmmv::Volume* volume = new libmmv::FloatVolume(volumeResolution, 0.0f);

        libmmv::Vec3ui coord(0, 0, 0);
        for (coord.z = 0; coord.z < volumeResolution.z; coord.z++)
        {
            for (coord.y = 0; coord.y < volumeResolution.y; coord.y++)
            {
                for (coord.x = 0; coord.x < volumeResolution.x; coord.x++)
                {
                    float value = computePriorityForVoxel(coord);
                    volume->setVoxelToValue(coord, value);
                }
            }
        }
        return volume;
    }

    libmmv::Volume* ComputeOrder::plotFillFrontToVolume()
    {
		ensureInitialized();

        libmmv::Volume* volume = new libmmv::ByteVolume(volumeResolution, 0.0f);
        for (auto it : *front)
        {
            volume->setVoxelToValue( it.coordinate, 255.0f );
        }
        return volume;
    }

	void ComputeOrder::initNeighborOffset()
	{
		int indexToUpperLeft = Flatten3D(libmmv::Vec3i(1, 1, 1), volumeResolution );
        libmmv::Vec3i voxelOffset;
		for (voxelOffset.z = 0; voxelOffset.z <= 2; voxelOffset.z++)
		{
			for (voxelOffset.y = 0; voxelOffset.y <= 2; voxelOffset.y++)
			{
				for (voxelOffset.x = 0; voxelOffset.x <= 2; voxelOffset.x++)
				{
					int indexFromUpperLeft = Flatten3D(voxelOffset, volumeResolution);
					if (voxelOffset.x != 1 || voxelOffset.y != 1 || voxelOffset.z != 1)
						neighborOffset.push_back( indexFromUpperLeft - indexToUpperLeft );
				}
			}
		}
	}

	void ComputeOrder::ensureInitialized()
	{
		if (front == nullptr)
			throw std::runtime_error("fillfront not initialized, use init function after constructor");
	}

} // namespace inpainting
