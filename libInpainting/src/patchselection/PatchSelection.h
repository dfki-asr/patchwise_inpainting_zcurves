#pragma once

#include "libmmv/math/Vec3.h"
#include <chrono>

namespace libmmv
{
    class FloatVolume;
    class ByteVolume;
}

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
        PatchSelection(libmmv::ByteVolume* dataVolume, libmmv::ByteVolume* maskVolume, libmmv::ByteVolume* dictionaryVolume );
        virtual ~PatchSelection();

		virtual libmmv::Vec3i adjustTargetPatchPosition(libmmv::Vec3i targetPatchCenter) = 0;
        virtual libmmv::Vec3i selectCenterOfBestPatch(libmmv::Vec3i sourcePosition) = 0;
		virtual void activateBruteForceFallback();

		//used for several iterations inside patch match, may need reworking in terms of proper software design
		virtual void computeCostAfterIteration();
		virtual bool initializeNewIteration();

        float lastCostFunctionValue;

        // remove after runtime optimization
        std::chrono::nanoseconds timeArgumentSettingTook = std::chrono::nanoseconds(0);
		unsigned int iterationNumber;

    protected:
        libmmv::ByteVolume* dictionaryVolume;
        libmmv::ByteVolume* dataVolume;
        libmmv::ByteVolume* maskVolume;
    };
}
