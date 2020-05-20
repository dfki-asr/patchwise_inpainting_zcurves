#pragma once

#include "math/Vec3.h"
#include <chrono>

namespace ettention
{

    class FloatVolume;
    class ByteVolume; 

    /*
	    This base class determines the way a patch is selected for inpainting to be able to use
	    acceleration structures. Inheriting from this class is used to define accelerated  
    */

    namespace inpainting {

		class Index;

        typedef std::pair<unsigned int, unsigned int> Interval;
        typedef std::vector<Interval> IntervalList;

        class PatchSelection
        {
        public:
            PatchSelection( ByteVolume* dataVolume, ByteVolume* maskVolume, ByteVolume* dictionaryVolume );
            virtual ~PatchSelection();

			virtual Vec3i adjustTargetPatchPosition(Vec3i targetPatchCenter) = 0;
            virtual Vec3i selectCenterOfBestPatch(Vec3i sourcePosition) = 0;
			virtual void activateBruteForceFallback();

			//used for several iterations inside patch match, may need reworking in terms of proper software design
			virtual void computeCostAfterIteration();
			virtual bool initializeNewIteration();

            float lastCostFunctionValue;

            // remove after runtime optimization
            std::chrono::nanoseconds timeArgumentSettingTook = std::chrono::nanoseconds(0);
			unsigned int iterationNumber;

        protected:
			ByteVolume* dictionaryVolume;
			ByteVolume* dataVolume;
            ByteVolume* maskVolume;
        };
    }
}