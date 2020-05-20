#pragma once

#include "math/Vec3.h"
#include "model/volume/BoundingBox.h"

namespace ettention
{
    class Volume;
    class ByteVolume;

    namespace inpainting
    {
		class Index;
        class ComputeFront;
		class InpaintingDebugParameters;

        class ComputeOrder
        {
        public:
            ComputeOrder( ByteVolume* mask );
            virtual ~ComputeOrder();

            virtual bool isEmpty();
            virtual Vec3ui selectCenterOfPatchToProcess( bool pop ) = 0;
            virtual float computePriorityForVoxel(Vec3ui coordinate) = 0;
			virtual void rememberPatchForLater(Vec3i coordinate);

            virtual void outputDebugVolumes( std::string pathToDebugFolder, unsigned int iterationNumber, InpaintingDebugParameters* parameters );

            virtual void progressFront(BoundingBox3i region);
            virtual void updatePriorityInRegion(Vec3ui from, Vec3ui to);

            ByteVolume* getMask();

            // public for testing only
            virtual void addCoordinateToFront(Vec3ui coordinate);
            void removeCoordinateFromFront(Vec3ui coordinate);
            void removeRegionFromFront(BoundingBox3i region);
			virtual int growFront(BoundingBox3i region);
			bool isAnyNeighborInTargetRegion(Vec3i coordinate);
			bool isAnyNeighborInTargetRegion_FastCheck(Vec3i coordinate);
			bool isAnyNeighborInTargetRegion_BorderCheck(Vec3i coordinate);
			void updatePriority(Vec3ui coordinate);
			bool shouldUseBruteForce();

            virtual size_t getSizeOfTargetArea();

            // for testing
            Volume* plotPriorityToVolume();
            Volume* plotFillFrontToVolume();

        protected:
			void initNeighborOffset();
			void ensureInitialized();

            Vec3ui volumeResolution;
            ByteVolume* mask;
			std::vector<int> neighborOffset;

        public: 
            ComputeFront* front; // temporarily
            size_t sizeOfTargetArea;
            size_t inpaintingIterationNumber = 0;
        };

    } // namespace inpainting
} // namespace ettention
