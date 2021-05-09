#pragma once

#include "libmmv/math/Vec3.h"
#include "libmmv/model/volume/BoundingBox.h"

namespace libmmv
{
    class Volume;
    class ByteVolume;
}

namespace inpainting
{
	class Index;
    class ComputeFront;
	class InpaintingDebugParameters;

    class ComputeOrder
    {
    public:
        ComputeOrder(libmmv::ByteVolume* mask );
        virtual ~ComputeOrder();

        virtual bool isEmpty();
        virtual libmmv::Vec3ui selectCenterOfPatchToProcess( bool pop ) = 0;
        virtual float computePriorityForVoxel(libmmv::Vec3ui coordinate) = 0;
		virtual void rememberPatchForLater(libmmv::Vec3i coordinate);

        virtual void outputDebugVolumes( std::string pathToDebugFolder, unsigned int iterationNumber, InpaintingDebugParameters* parameters );

        virtual void progressFront(libmmv::BoundingBox3i region);
        virtual void updatePriorityInRegion(libmmv::Vec3ui from, libmmv::Vec3ui to);

        libmmv::ByteVolume* getMask();

        // public for testing only
        virtual void addCoordinateToFront(libmmv::Vec3ui coordinate);
        void removeCoordinateFromFront(libmmv::Vec3ui coordinate);
        void removeRegionFromFront(libmmv::BoundingBox3i region);
		virtual int growFront(libmmv::BoundingBox3i region);
		bool isAnyNeighborInTargetRegion(libmmv::Vec3i coordinate);
		bool isAnyNeighborInTargetRegion_FastCheck(libmmv::Vec3i coordinate);
		bool isAnyNeighborInTargetRegion_BorderCheck(libmmv::Vec3i coordinate);
		void updatePriority(libmmv::Vec3ui coordinate);
		bool shouldUseBruteForce();

        virtual size_t getSizeOfTargetArea();

        // for testing
        libmmv::Volume* plotPriorityToVolume();
        libmmv::Volume* plotFillFrontToVolume();

    protected:
		void initNeighborOffset();
		void ensureInitialized();

        libmmv::Vec3ui volumeResolution;
        libmmv::ByteVolume* mask;
		std::vector<int> neighborOffset;

    public: 
        ComputeFront* front; // temporarily
        size_t sizeOfTargetArea;
        size_t inpaintingIterationNumber = 0;
    };

} // namespace inpainting

