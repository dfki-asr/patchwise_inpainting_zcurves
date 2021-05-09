#include "stdafx.h"

#include "PatchMatchPatchSelection.h"

#include <math.h> 

#include "StatusFlags.h"
#include "dictionary/Dictionary.h"

#include "setup/IndexOptions.h"

#include "PatchSelectionStatistics.h"
#include "index/DimensionSelection.h"
#include "../zcurve/ZCurveKNNQuery.h"
#include "../zcurve/ZCurveIndex.h"
#include "index/MultiIndex.h"

#include "index/MaskedIndex.h"
#include "libmmv/io/serializer/VolumeSerializer.h"
#include "setup/parameterset/OutputParameterSet.h"

namespace inpainting
{

	PatchMatchPatchSelection::PatchMatchPatchSelection
	(
		DictionaryBasedCostFunctionKernel* costFunction,
		libmmv::ByteVolume* dataVolume,
		libmmv::ByteVolume* maskVolume,
		libmmv::ByteVolume* dictionaryVolume,
		std::vector<unsigned int>& dictionaryPatches,
		libmmv::Vec3ui patchSize,
		InpaintingDebugParameters* debugParameters,
		InpaintingOptimizationParameters* optimizationParameters,
		IndexOptions* indexOptions,
		CostFunctionOptions* costFunctionOptions,
		bool shouldUseBruteForce,
		ProgressReporter* progress
	)
		: PatchSelection(dataVolume, maskVolume, dictionaryVolume)
		, dictionaryPatches(dictionaryPatches)
        , patchSize(patchSize)
        , debugParameters(debugParameters)
        , optimizationParameters(optimizationParameters)
		, progress(progress)
    {
		verbose = false;
		log = new Log(debugParameters);
		srand((unsigned int)42);

		//if set to 1.0F no iteration will occur at unreliable pixels init, respectively at inpainting
		iterateIfBelowFactorInitUnreliable = 0.9F;// 0.9F;
		iterateIfBelowFactorInpaint = 1.0F;

		libmmv::Vec3ui resolution = maskVolume->getProperties().getVolumeResolution();
		resolutionInt = libmmv::Vec3i(resolution.x, resolution.y, resolution.z);
		unsigned int numberOfPixels = (resolution.x * resolution.y * resolution.z);

		std::vector<libmmv::Vec3i> dictPatchCoords;
		libmmv::Vec3i currentCoord;
		for (int i = 0; i < dictionaryPatches.size(); i++)
		{
			currentCoord = Unflatten3D(dictionaryPatches[i], resolutionInt);
			dictPatchCoords.push_back(currentCoord);
		}

		progress->reportTaskStart("compute reliable mask", 1.0F);
		reliableMaskStatus.resize(numberOfPixels);
		maskStatus.resize(numberOfPixels);
		initMaskStatus();
		computeReliableSourceRegion();
		//saveReliableMaskStatusToVolume("D:/forEBI/test/reliableMaskStatus.mrc");
		progress->reportTaskEnd();

		progress->reportTaskStart("initializing NNF", 1.0F);
		nearestNeighborField.resize(numberOfPixels);
		nearestNeighborFieldCosts.resize(numberOfPixels);
		initNearestNeighborField();
		progress->reportTaskEnd();
			
		progress->reportTaskStart("initializing unreliable pixels", 1.0F);
		initUnreliablePixelsNNF();
		progress->reportTaskEnd();
	}

	PatchMatchPatchSelection::~PatchMatchPatchSelection()
    {
		delete log;
    }

	libmmv::Vec3i PatchMatchPatchSelection::adjustTargetPatchPosition( libmmv::Vec3i targetPatchCenter)
	{
		return ensureTargetPatchPositionInsideSafeRegion( targetPatchCenter );
	}

	libmmv::Vec3i PatchMatchPatchSelection::selectCenterOfBestPatch(libmmv::Vec3i targetPatchCenter)
    {
		log->setTargetPosition(targetPatchCenter);
		this->centerOfTargetPatch = targetPatchCenter;

		propagateNearestNeighborField(targetPatchCenter);
		float lastCostFunctionValue;
		lastCostFunctionValue = performRandomSearch(targetPatchCenter);

		size_t targetPatchIndex = Flatten3D(targetPatchCenter, resolutionInt);
		libmmv::Vec3i bestCoordinate = nearestNeighborField[targetPatchIndex];
		setNNFofNeighboringTargetPixels(targetPatchCenter);

		log->setPatchNorm(lastCostFunctionValue);
		log->setSourcePosition(bestCoordinate);

		if (debugParameters->shouldDoubleCheckUsingBruteForce())
		{
			doubleCheckUsingBruteForce(targetPatchCenter);
		}

		log->reportPatchSelection();

        return bestCoordinate;
    }

	float PatchMatchPatchSelection::computeSSD(libmmv::Vec3i targetPatchCenter, libmmv::Vec3i sourcePatchCenter)
	{
		libmmv::BoundingBox3i insideBox = libmmv::BoundingBox3i(libmmv::Vec3i(0, 0, 0), resolutionInt - libmmv::Vec3i(1, 1, 1));
		float result = 0.0F;
		libmmv::Vec3i patchRadius = patchSize / 2;
		size_t currentIndex = -1;
		unsigned char status = TARGET_REGION;
		libmmv::Vec3i targetCoord, sourceCoord;
		size_t targetIndex, sourceIndex;
		float targetValue, sourceValue, difference;
		int counter = 0;

		for (int x = -patchRadius.x; x <= patchRadius.x; x++)
			for (int y = -patchRadius.y; y <= patchRadius.y; y++)
				for(int z = -1; z <= 1; z++)
				{
					targetCoord = targetPatchCenter + libmmv::Vec3i(x, y, z);

					if (targetCoord.x >= 0 && targetCoord.y >= 0 && targetCoord.x < resolutionInt.x && targetCoord.y < resolutionInt.y)
					{
						targetIndex = Flatten3D(targetCoord, resolutionInt);
						status = maskVolume->nativeVoxelValue(targetIndex);

						if (status != TARGET_REGION && status != EMPTY_REGION)
						{
							sourceCoord = sourcePatchCenter + libmmv::Vec3i(x, y, z);

							if (insideBox.isInside(sourceCoord))
							{
								sourceIndex = Flatten3D(sourceCoord, resolutionInt);
								targetValue = dataVolume->getVoxelValue(targetIndex);
								sourceValue = dataVolume->getVoxelValue(sourceIndex);

								difference = (sourceValue - targetValue);
								result += (difference * difference);
								counter++;
							}
						}
					}
				}
		if (counter == 0)
		{
			return std::numeric_limits<float>::max();
		}

		return sqrtf(result/counter);
	}

	//duplicated SSD with no checking for sourceCoord in range, in test status if it helps for speed up when applied where appropriate
	float PatchMatchPatchSelection::computeSSDnoCheck(libmmv::Vec3i targetPatchCenter, libmmv::Vec3i sourcePatchCenter)
	{
		libmmv::BoundingBox3i insideBox = libmmv::BoundingBox3i( libmmv::Vec3i(0, 0, 0), resolutionInt - libmmv::Vec3i(1, 1, 1) );
		float result = 0.0F;
		libmmv::Vec3i patchRadius = patchSize / 2;
		size_t currentIndex = -1;
		unsigned char status = TARGET_REGION;
		libmmv::Vec3i targetCoord, sourceCoord;
		size_t targetIndex, sourceIndex;
		float targetValue, sourceValue, difference;
		int counter = 0;

		for (int x = -patchRadius.x; x <= patchRadius.x; x++)
			for (int y = -patchRadius.y; y <= patchRadius.y; y++)
				for (int z = -1; z <= 1; z++)
				{
					targetCoord = targetPatchCenter + libmmv::Vec3i(x, y, z);

					if (targetCoord.x >= 0 && targetCoord.y >= 0 && targetCoord.x < resolutionInt.x && targetCoord.y < resolutionInt.y)
					{
						targetIndex = Flatten3D(targetCoord, resolutionInt);
						status = maskVolume->nativeVoxelValue(targetIndex);

						if (status != TARGET_REGION && status != EMPTY_REGION)
						{
							sourceCoord = sourcePatchCenter + libmmv::Vec3i(x, y, z);
							sourceIndex = Flatten3D(sourceCoord, resolutionInt);
							targetValue = dataVolume->getVoxelValue(targetIndex);
							sourceValue = dataVolume->getVoxelValue(sourceIndex);

							difference = (sourceValue - targetValue);
							result += (difference * difference);
							counter++;
						}
					}
				}
		if (counter == 0)
		{
			return std::numeric_limits<float>::max();
		}

		return sqrtf(result / counter);
	}

	void PatchMatchPatchSelection::propagateNearestNeighborField(libmmv::Vec3i targetPatchCenter)
	{
		libmmv::Vec3i minCoord = libmmv::Vec3i(patchSize.x / 2, patchSize.y / 2, 1);
		size_t targetPatchIndex = Flatten3D(targetPatchCenter, resolutionInt);
		size_t propagatedPatchIndex;
		size_t currentIndex = -1;
		unsigned char status = TARGET_REGION;
		libmmv::Vec3i currentCoord, mappedCoordInNNF, propagatedCoord;

		// compute the current cost function value and update in NNF, as it may have changed due to inpainting
		currentCoord = nearestNeighborField[targetPatchIndex];
		float costValue = computeSSDnoCheck(targetPatchCenter, currentCoord);
		nearestNeighborFieldCosts[targetPatchIndex] = costValue;

		// propagate over all pixels inside window instead of left+upper
		for (int wx = -1; wx <= 1; wx++)
			for (int wy = -1; wy <= 1; wy++)
			{
				currentCoord = targetPatchCenter + libmmv::Vec3i(wx, wy, 0);
				if (currentCoord.x >= minCoord.x && currentCoord.y >= minCoord.y && currentCoord.x < resolutionInt.x - minCoord.x && currentCoord.y < resolutionInt.y - minCoord.y)
				{
					currentIndex = Flatten3D(currentCoord, resolutionInt);
					mappedCoordInNNF = nearestNeighborField[currentIndex];

					propagatedCoord = mappedCoordInNNF - libmmv::Vec3i(wx, wy, 0);

					propagatedPatchIndex = Flatten3D(propagatedCoord, resolutionInt);
					status = reliableMaskStatus[propagatedPatchIndex];
					if (status != SOURCE_REGION)
					{
						if (propagatedPatchIndex == currentIndex)
						{
							propagatedCoord = getRandomCoordinateInsideSource();
						}
						else
						{
							propagatedCoord = nearestNeighborField[propagatedPatchIndex];
						}
					}
					costValue = computeSSD(targetPatchCenter, propagatedCoord);
					if (costValue < nearestNeighborFieldCosts[targetPatchIndex])
					{
						nearestNeighborFieldCosts[targetPatchIndex] = costValue;
						nearestNeighborField[targetPatchIndex] = mappedCoordInNNF;
					}
				}
			}
	}

	float PatchMatchPatchSelection::performRandomSearch(libmmv::Vec3i targetPatchCenter)
	{
		libmmv::Vec3i boxSize = libmmv::Vec3i(resolutionInt.x, resolutionInt.y, 1);
		libmmv::Vec3i randomCoord;
		int randomXCoord, randomYCoord;
		float costValue, bestCostAtTheMoment;
		size_t targetPatchIndex = Flatten3D(targetPatchCenter, resolutionInt);
		size_t randomIndex;
		unsigned char status = TARGET_REGION;

		do 
		{
			randomXCoord = rand() % boxSize.x;
			randomYCoord = rand() % boxSize.y;
			randomCoord = libmmv::Vec3i(targetPatchCenter.x + randomXCoord, targetPatchCenter.y + randomYCoord, targetPatchCenter.z);
			randomCoord = ensureTargetPatchPositionInsideSafeRegion(randomCoord);
			if (targetPatchCenter != randomCoord)
			{
				randomIndex = Flatten3D(randomCoord, resolutionInt);
				status = reliableMaskStatus[randomIndex];
				if (status != SOURCE_REGION)
				{
					randomCoord = nearestNeighborField[randomIndex];
				}
				costValue = computeSSDnoCheck(targetPatchCenter, randomCoord);
				bestCostAtTheMoment = nearestNeighborFieldCosts[targetPatchIndex];
				if (bestCostAtTheMoment == 0.0F)
				{
					return 0.0F;
				}
				if (costValue < bestCostAtTheMoment)
				{
					nearestNeighborFieldCosts[targetPatchIndex] = costValue;
					nearestNeighborField[targetPatchIndex] = randomCoord;
				}
			}
			boxSize = libmmv::Vec3i(boxSize.x / 2, boxSize.y / 2, 1);
		} while (boxSize.x > 1 && boxSize.y > 1);

		return nearestNeighborFieldCosts[targetPatchIndex];
	}

	libmmv::Vec3i PatchMatchPatchSelection::getRandomCoordinateInsideSource()
	{
		int randomIndex = rand() % reliableSourceCoordinates.size();
		return reliableSourceCoordinates[randomIndex];
	}

	void PatchMatchPatchSelection::initNearestNeighborField()
	{
		libmmv::Vec3i randomCoord;
		float currentCost = 0.0F;
		unsigned char status;

		libmmv::Vec3i reliableBorderDistance = libmmv::Vec3i(patchSize.x / 2, patchSize.y / 2, 0);

		for (int x = reliableBorderDistance.x; x < resolutionInt.x - reliableBorderDistance.x; x++)
			for (int y = reliableBorderDistance.y; y < resolutionInt.y - reliableBorderDistance.y; y++)
			{
				libmmv::Vec3ui currentCoord = libmmv::Vec3ui(x, y, 1);
				size_t currentIndex = Flatten3D(currentCoord, resolutionInt);
				status = reliableMaskStatus[currentIndex];
				if (status != SOURCE_REGION)
				{
					randomCoord = getRandomCoordinateInsideSource();
					nearestNeighborField[currentIndex] = randomCoord;
					currentCost = computeSSDnoCheck(currentCoord, randomCoord);
					nearestNeighborFieldCosts[currentIndex] = currentCost;
				}
				else
				{
					nearestNeighborField[currentIndex] = currentCoord;
					nearestNeighborFieldCosts[currentIndex] = 0.0F;
				}
			}
	}

	void PatchMatchPatchSelection::initUnreliablePixelsNNF()
	{
		float old_distance = 0.0F;
		float currentDistance = 0.0F;
		float currentCost = 0.0F;
		size_t currentIndex = 0;
		libmmv::Vec3i assignedCoord = libmmv::Vec3i(0,0,0);
			
		if (iterateIfBelowFactorInitUnreliable < 1.0F)
		{
			for (int i = 0; i < unreliableCoordinates.size(); i++)
			{
				currentIndex = Flatten3D(unreliableCoordinates[i], resolutionInt);
				assignedCoord = nearestNeighborField[currentIndex];

				//noCheck only possible if no TARGET_REGION at border, otherwise must be handled differently (slower)
				currentCost = computeSSDnoCheck(unreliableCoordinates[i], assignedCoord);
				if (currentCost != std::numeric_limits<float>::max())
				{
					currentDistance += currentCost;
				}
			}
		}

		do {
			old_distance = currentDistance;
			currentDistance = 0.0F;

			for (int i = 0; i < unreliableCoordinates.size(); i++)
			{
				propagateNearestNeighborField(unreliableCoordinates[i]);
				currentCost = performRandomSearch(unreliableCoordinates[i]);

				if (currentDistance != std::numeric_limits<float>::max())
				{
					currentDistance += currentCost;
				}
			}

			if (iterateIfBelowFactorInitUnreliable < 1.0F)
			{
				std::cout << currentDistance << std::endl;
			}
		} while (currentDistance < iterateIfBelowFactorInitUnreliable * old_distance && iterateIfBelowFactorInitUnreliable < 1.0F);
	}

	// Each patch that is not completely contained in the source region is flagged unreliable_region, as it needs a special treatment.
	void PatchMatchPatchSelection::computeReliableSourceRegion()
	{
		libmmv::Vec3i reliableBorderDistance = libmmv::Vec3i(patchSize.x / 2, patchSize.y / 2, patchSize.z / 2);
		unsigned char status;

		for (int x = reliableBorderDistance.x; x < resolutionInt.x - reliableBorderDistance.x; x++)
			for (int y = reliableBorderDistance.y; y < resolutionInt.y - reliableBorderDistance.y; y++)
			{
				libmmv::Vec3ui currentCoord = libmmv::Vec3ui(x, y, 1);
				size_t currentIndex = Flatten3D(currentCoord, resolutionInt);
				status = maskStatus[currentIndex];
				if (status == SOURCE_REGION)
				{
					if (isSourceUnreliable(currentCoord))
					{
						reliableMaskStatus[currentIndex] = UNRELIABLE_REGION;
						unreliableCoordinates.push_back(currentCoord);
					}
					else
					{
						reliableMaskStatus[currentIndex] = SOURCE_REGION;
						reliableSourceCoordinates.push_back(currentCoord);
					}
				}
				else
					reliableMaskStatus[currentIndex] = status;
			}
	}

	void PatchMatchPatchSelection::initMaskStatus()
	{
		libmmv::Vec3i currentCoord;
		unsigned char status;

		for (int x = 0; x < resolutionInt.x; x++)
			for (int y = 0; y < resolutionInt.y; y++)
				for(int z = 0; z < resolutionInt.z; z++)
				{
					currentCoord = libmmv::Vec3i(x, y, z);
					size_t currentIndex = Flatten3D(currentCoord, resolutionInt);
					status = maskVolume->nativeVoxelValue(currentIndex);
					maskStatus[currentIndex] = status;
				}
	}

	bool PatchMatchPatchSelection::isSourceUnreliable(libmmv::Vec3ui coord)
	{
		libmmv::Vec3i coordInt = libmmv::Vec3i(coord.x, coord.y, coord.z);
		libmmv::Vec3i startCoord = componentWiseMax(libmmv::Vec3i(0, 0, 0), coordInt - (patchSize / 2));
		libmmv::Vec3i endCoord = componentWiseMin(coordInt + (patchSize / 2), resolutionInt - libmmv::Vec3i(1, 1, 1));
		unsigned char status;

		for (int x = startCoord.x; x <= endCoord.x; x++)
			for (int y = startCoord.y; y <= endCoord.y; y++)
				{
					libmmv::Vec3i currentCoord = libmmv::Vec3i(x, y, 1);
					size_t currentIndex = Flatten3D(currentCoord, resolutionInt);
					status = maskStatus[currentIndex];
					if (status != SOURCE_REGION)
						return true;
				}
		return false;
	}

	// used for testing, unreliable pixels are not changed in the original algorithm
	void PatchMatchPatchSelection::insertUnreliableRegionPixels()
	{
		size_t unreliableIndex, currentIndex;
		libmmv::Vec3i currentCoord, unreliableCoordinate, unreliableCoordinateTmp, tmpCoord;
		float newValue, oldValue;
		libmmv::ByteVolume* costVolume = new libmmv::ByteVolume(resolutionInt, 42.0F);

		for (int i = 0; i < unreliableCoordinates.size(); i++)
		{
			unreliableCoordinate = unreliableCoordinates[i];
			unreliableIndex = Flatten3D(unreliableCoordinate, resolutionInt);
			currentCoord = nearestNeighborField[unreliableIndex];
			for (int z = -1; z <= 1; z++)
			{
				unreliableCoordinateTmp = unreliableCoordinate + libmmv::Vec3i(0, 0, z);
				unreliableIndex = Flatten3D(unreliableCoordinateTmp, resolutionInt);
				tmpCoord = currentCoord + libmmv::Vec3i(0, 0, z);
				currentIndex = Flatten3D(tmpCoord, resolutionInt);
				oldValue = dataVolume->getVoxelValue(unreliableIndex);
				newValue = dataVolume->getVoxelValue(currentIndex);
				dataVolume->setVoxelToValue(unreliableIndex, newValue);

				if (z == 0)
				{
					costVolume->setVoxelToValue(unreliableIndex, nearestNeighborFieldCosts[unreliableIndex]);
				}
			}
		}

/*		cfg::OutputParameterSet parameter;
		libmmv::VolumeSerializer serializer;
		throw std::runtime_error("legacy code smell alarm")
		std::string filename = "D:/forEBI/test/unreliablePixelsInserted.mrc";
		serializer.write(dataVolume, filename, parameter.getVoxelType(), parameter.getOrientation() );
		std::string filenameCost = "D:/forEBI/test/unreliablePixelsInserted_cost.mrc";
		serializer.write(costVolume, filenameCost, parameter.getVoxelType(), parameter.getOrientation() ); */
		delete costVolume;
	}

	libmmv::Vec3i PatchMatchPatchSelection::ensureTargetPatchPositionInsideSafeRegion(libmmv::Vec3i targetPatchCenter)
	{
		libmmv::Vec3i borderDistance = libmmv::Vec3i(patchSize.x / 2, patchSize.y / 2, 0);
		libmmv::Vec3i insideCoord = componentWiseMax(borderDistance, targetPatchCenter);
		insideCoord = componentWiseMin(insideCoord, resolutionInt - borderDistance - libmmv::Vec3i(1, 1, 0));
		insideCoord.z = 1;
		return insideCoord;
	}

	// needed for stopping criterion if iterative runs of inpainting are used
	void PatchMatchPatchSelection::computeCostAfterIteration()
	{
		totalCostOfCurrentNNF = 0.0F;
		libmmv::Vec3ui currentCoord, assignedCoord;
		size_t currentIndex;
		float currentCost;

		for (int x = 0; x < resolutionInt.x ; x++)
			for (int y = 0; y < resolutionInt.y; y++)
				for (int z = 0; z < resolutionInt.z; z++)
				{
					currentCoord = libmmv::Vec3ui(x, y, z);
					currentIndex = Flatten3D(currentCoord, resolutionInt);
					unsigned char status = reliableMaskStatus[currentIndex];

					if (status == UNRELIABLE_REGION || status == INPAINTED_REGION)
					{
						assignedCoord = nearestNeighborField[currentIndex];
						//noCheck only possible if no TARGET_REGION at border, otherwise must be handled differently (slower)
						currentCost = computeSSDnoCheck(currentCoord, assignedCoord);
						totalCostOfCurrentNNF += currentCost;
					}
				}
		std::cout << "Current iteration cost: " << totalCostOfCurrentNNF << std::endl;
	}

	// used if patch match is applied iteratively, that means several runs of inpainting to enhance NNF
	bool PatchMatchPatchSelection::initializeNewIteration()
	{
		bool shouldDoFurtherIteration = false;
		float oldCost = totalCostOfCurrentNNF;
		unsigned char status;

		if (iterateIfBelowFactorInpaint < 1.0F)
		{
			computeCostAfterIteration();

			if (totalCostOfCurrentNNF < iterateIfBelowFactorInpaint * oldCost)
			{
				shouldDoFurtherIteration = true;

				libmmv::Vec3ui currentCoord;
				size_t currentIndex;

				for (int x = 0; x < resolutionInt.x; x++)
					for (int y = 0; y < resolutionInt.y; y++)
					{
						currentCoord = libmmv::Vec3ui(x, y, 1);
						currentIndex = Flatten3D(currentCoord, resolutionInt);
						status = maskVolume->nativeVoxelValue(currentIndex);
						if (status == INPAINTED_REGION)
						{
							maskVolume->setVoxelToByteValue(currentIndex, TARGET_REGION);
						}
					}
			}
		}
			
		return shouldDoFurtherIteration;
	}

	// used for computing Acceleration Error for paper
	void PatchMatchPatchSelection::doubleCheckUsingBruteForce(libmmv::Vec3i targetPatchCenter)
	{
		float bestBruteForceCost = std::numeric_limits<float>::max();
		float currentCost = std::numeric_limits<float>::max();
		libmmv::Vec3i currentSourceCoord, bestBruteForceCoord;

		size_t currentIndex = 0;
		unsigned char status = TARGET_REGION;
	
		for(int i = 0; i < dictionaryPatches.size(); i++)
		{
			currentIndex = dictionaryPatches[i];
			currentSourceCoord = Unflatten3D(currentIndex, resolutionInt);
			status = reliableMaskStatus[currentIndex];
			if (targetPatchCenter != currentSourceCoord && status != TARGET_REGION && status != EMPTY_REGION)
			{
				currentCost = computeSSD(targetPatchCenter, currentSourceCoord);
				if (currentCost < bestBruteForceCost)
				{
					bestBruteForceCost = currentCost;
					bestBruteForceCoord = currentSourceCoord;
				}
			}
		}	

		log->setPatchNormBruteForce(bestBruteForceCost);
		log->setBestBruteForcePosition(bestBruteForceCoord);
	}

	void PatchMatchPatchSelection::saveReliableMaskStatusToVolume(std::string filename)
	{
		libmmv::Vec3i currentCoord;
		size_t currentIndex;
		unsigned char status;

		libmmv::ByteVolume reliableVolume(resolutionInt, 25.0F);
		for (int x = 0; x < resolutionInt.x; x++)
			for (int y = 0; y < resolutionInt.y; y++)
				for (int z = 0; z < resolutionInt.z; z++)
				{
					currentCoord = libmmv::Vec3i(x, y, 1);
					currentIndex = Flatten3D(currentCoord, resolutionInt);
					status = reliableMaskStatus[currentIndex];
					reliableVolume.setVoxelToByteValue(currentIndex, status);
				}

		cfg::OutputParameterSet parameter;
		libmmv::VolumeSerializer serializer;
		serializer.write(&reliableVolume, filename, parameter.getVoxelType(), parameter.getOrientation() );
	}

	void PatchMatchPatchSelection::setNNFofNeighboringTargetPixels(libmmv::Vec3i targetCoord)
	{
		libmmv::Vec3i currentTargetCoord;
		size_t targetIndex = Flatten3D(targetCoord, resolutionInt);
		size_t sourceIndex;
		unsigned char status, unreliableStatus;

		libmmv::Vec3i sourceCoord = nearestNeighborField[targetIndex];
		libmmv::Vec3i currentSourceCoord;
		libmmv::Vec3i borderDistance = libmmv::Vec3i(patchSize.x / 2, patchSize.y / 2, 0);
		libmmv::BoundingBox3i insideBox = libmmv::BoundingBox3i(borderDistance, resolutionInt - borderDistance - libmmv::Vec3i(1, 1, 0));

		for (int x = -patchSize.x / 2; x <= patchSize.x / 2; x++)
			for (int y = -patchSize.y / 2; y <= patchSize.y / 2; y++)
			{
				if (x != 0 && y != 0)
				{
					currentTargetCoord = targetCoord + libmmv::Vec3i(x, y, 0);

					if (currentTargetCoord.x >= 0 && currentTargetCoord.y >= 0 && currentTargetCoord.x < resolutionInt.x && currentTargetCoord.y < resolutionInt.y)
					{
						targetIndex = Flatten3D(currentTargetCoord, resolutionInt);
						status = maskVolume->nativeVoxelValue(targetIndex);
						if (status == TARGET_REGION)
						{
							currentSourceCoord = sourceCoord + libmmv::Vec3i(x, y, 0);
							sourceIndex = Flatten3D(currentSourceCoord, resolutionInt);
							unreliableStatus = reliableMaskStatus[sourceIndex];
							if (unreliableStatus == SOURCE_REGION)
							{
								nearestNeighborField[targetIndex] = currentSourceCoord;
							}
							else
							{
								nearestNeighborField[targetIndex] = sourceCoord;
							}
						}
					}
				}
			}
	}

} // namespace
