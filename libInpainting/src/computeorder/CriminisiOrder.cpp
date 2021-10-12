#include "stdafx.h"

#include "CriminisiOrder.h"
#include "Problem.h"

#include "libmmv/io/serializer/VolumeSerializer.h"
#include "libCFG/setup/parameterset/OutputParameterSet.h"
#include "setup/InpaintingDebugParameters.h"

#include "Problem.h"
#include "Confidence.h"
#include "CriminisiDataTerm.h"
#include "ComputeFront.h"
#include "Gradient3D.h"
#include "ComputeFrontInitializer.h"
#include "StatusFlags.h"

#include "libmmv/model/volume/ByteVolume.h"
#include "libmmv/model/volume/FloatVolume.h"

namespace inpainting
{

    CriminisiOrder::CriminisiOrder( Problem* problem, ProgressReporter* progress, bool regularizeConfidence)
        : ComputeOrder(problem->mask)
        , progress(progress)
		, problem(problem)
		, dataTerm ( nullptr )
	{
        volumeResolution = problem->mask->getProperties().getVolumeResolution();
        confidence = new Confidence(problem->mask, problem->patchSize, regularizeConfidence);
		dataTerm = new CriminisiDataTerm(problem->data, problem->mask, problem->patchSize);
    }

    CriminisiOrder::~CriminisiOrder()
    {
        delete confidence;
        delete dataTerm;
        delete front;
    }

	libmmv::Vec3ui CriminisiOrder::selectCenterOfPatchToProcess( bool shouldPopPatch )
    {
		ensureInitialized();
        if (shouldPopPatch)
            return front->popEntryWithHighestPriority().coordinate;
        return front->peekEntryWithHighestPriority().coordinate;
    }

    float CriminisiOrder::computePriorityForVoxel( libmmv::Vec3ui coordinate)
    {
        float dataValue = dataTerm->computeDataTermForOneVoxel(coordinate, 255.0f);
        float confidenceValue = confidence->computeConfidenceOfOneVoxel(coordinate, inpaintingIterationNumber);

        if( dataValue == 0.0f )
            return confidenceValue;
        else
            return dataValue * confidenceValue;
    }

	void CriminisiOrder::outputDebugVolumes(std::string pathToDebugFolder, unsigned int iterationNumber, InpaintingDebugParameters* parameters)
	{
		ComputeOrder::outputDebugVolumes(pathToDebugFolder, iterationNumber, parameters);

		libCFG::OutputParameterSet parameter;
		libmmv::VolumeSerializer serializer;

		if (parameters->shouldOutputPriority())
		{
			std::string fileNamePriority = pathToDebugFolder + "priority_iteration_" + std::to_string((int)iterationNumber) + ".mrc";
			serializer.write(plotPriorityToVolume(), fileNamePriority, parameter.getVoxelType(), parameter.getOrientation() );
		}

		if (parameters->shouldOutputConfidenceTerm())
		{
			std::string fileNameConfidence = pathToDebugFolder + "confidence_iteration_" + std::to_string((int)iterationNumber) + ".mrc";
			serializer.write(plotConfidenceToVolume(), fileNameConfidence, parameter.getVoxelType(), parameter.getOrientation() );
		}

		if (parameters->shouldOutputDataTerm())
		{
			std::string fileNameDataTerm = pathToDebugFolder + "dataterm_iteration_" + std::to_string((int)iterationNumber) + ".mrc";
			serializer.write(plotDataTermToVolume(), fileNameDataTerm, parameter.getVoxelType(), parameter.getOrientation() );
		}

		dataTerm->outputDebugVolumes( pathToDebugFolder, iterationNumber, parameters );
    }

	void CriminisiOrder::init()
	{
		ComputeFrontInitializer initializer( this, problem->patchSize, progress);
		front = initializer.generateComputeFront();
		sizeOfTargetArea = initializer.getSizeOfTargetArea();
	}

	libmmv::Volume* CriminisiOrder::plotConfidenceToVolume()
    {
		ensureInitialized();

		libmmv::Volume* volume = new libmmv::FloatVolume(volumeResolution, 0.0f);
		for (auto it = front->begin(); it != front->end(); ++it)
		{
			libmmv::Vec3i coord = it->coordinate;
			size_t voxelIndex = problem->mask->getVoxelIndex(coord);
			float value = confidence->computeConfidenceOfOneVoxel(coord, inpaintingIterationNumber);
			volume->setVoxelToValue(coord, value);
		}
        return volume;
    }

	libmmv::Volume* CriminisiOrder::plotDataTermToVolume()
    {
		ensureInitialized();

		libmmv::Volume* volume = new libmmv::FloatVolume(volumeResolution, 0.0f);
		for (auto it = front->begin(); it != front->end(); ++it)
		{
			libmmv::Vec3i coord = it->coordinate;
			size_t voxelIndex = problem->mask->getVoxelIndex(coord);
			float value = dataTerm->computeDataTermForOneVoxel(coord, 255);
			volume->setVoxelToValue(coord, value);
		}
		return volume;
	}

	libmmv::Volume* CriminisiOrder::plotComputeFrontToVolume()
    {
		ensureInitialized();

        return front->plotToVolume( volumeResolution );
    }

} // namespace inpainting
