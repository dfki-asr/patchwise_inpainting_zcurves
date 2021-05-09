#include "stdafx.h"

#include "ComputeFrontInitializer.h"
#include "libmmv/model/volume/ByteVolume.h"
#include "computeorder/ComputeFront.h"
#include "computeorder/ComputeOrder.h"
#include "StatusFlags.h"
#include "ProgressReporter.h"

#include <thread>

    namespace inpainting
    {

        ComputeFrontInitializer::ComputeFrontInitializer( ComputeOrder* priority, libmmv::Vec3ui patchSize, ProgressReporter* progress)
            : progress(progress)
            , priority(priority)
            , patchSize(patchSize)
        {
        }

        ComputeFrontInitializer::~ComputeFrontInitializer()
        {
        }

        ComputeFront* ComputeFrontInitializer::generateComputeFront( )
        {
			progress->reportTaskStart("initializing compute front", 1.0f);
			ComputeFront* front = new ComputeFront();
			priority->front = front;
			sizeOfTargetArea = priority->growFront( priority->getMask()->getProperties().getMemoryBoundingBox() );
			progress->reportTaskEnd();
			return front;
        }

        size_t ComputeFrontInitializer::getSizeOfTargetArea()
        {
            return sizeOfTargetArea;
        }

    } // namespace inpainting
