#include "stdafx.h"

#include "IDistance.h"
#include "IDistanceIndex.h"
#include "pca/PCASubspace.h"
#include "StatusFlags.h"

namespace inpainting
{

	IDistanceIndex::IDistanceIndex(libmmv::ByteVolume* data, libmmv::ByteVolume* mask, libmmv::ByteVolume* dictionary, std::vector<unsigned int>& patchIds, libmmv::Vec3ui patchSize, DimensionSelection::PreferredDirection preferredDirection, int unprojectedSize, int subspaceDimensions, int filterSize )
		: MaskedIndex(mask, patchSize, preferredDirection, unprojectedSize )
		, data(data), dictionary(dictionary), subspaceDimensions(subspaceDimensions), filterSize(filterSize), preferredDirection(preferredDirection)
	{
		subspace = new PCASubspace<float>( data, dictionary, patchSize, permutation, patchIds, subspaceDimensions );
		auto dataPoints = getDictionaryPointSetInPrincipalSpace( subspace, patchIds );
		iDistance = new IDistance( patchIds, dataPoints, (int) patchIds.size(), subspaceDimensions );
		filterAndRefine = new FilterAndRefineImpl< L2Distance< BytePatchAccess8Bit > >(data, mask, dictionary, patchSize);
	}

	IDistanceIndex::~IDistanceIndex()
	{
		delete filterAndRefine;
		delete iDistance;
		delete subspace;
	}

	std::tuple<libmmv::Vec3i, float> IDistanceIndex::query(libmmv::Vec3i targetPatchCenter )
	{
		auto queryPointInPrincipalSpace = subspace->getDataPointInPrincipalSpace( targetPatchCenter );

		iDistance->resetNodeCount();
		iDistance->resetCandidateCount();
		iDistance->resetPartitionCount();

		auto knn = iDistance->QueryKNN(queryPointInPrincipalSpace, 10);

		int numPartitions = iDistance->getPartitionCount();
		int numNodes = iDistance->getNodeCount();
		int numCandidates = iDistance->getCandidateCount();

		std::cout << "query touched partitions " << numPartitions << std::endl;
		std::cout << "              nodes      " << numNodes << std::endl;
		std::cout << "              candidates " << numCandidates << std::endl;

		return filterAndRefine->selectBestPatch( targetPatchCenter, knn );
	}

	std::string IDistanceIndex::to_string()
	{
		return "idistance_index" + DimensionSelection::to_string(preferredDirection);;
	}

	std::vector<float> IDistanceIndex::getDictionaryPointSetInPrincipalSpace(PCASubspace<float>* subspace, std::vector<unsigned int>& patchIds)
	{
		std::vector<float> result(patchIds.size() * subspaceDimensions);
		for (size_t i = 0; i < patchIds.size(); i++)
		{
			auto projectedPoint = subspace->getDictionaryPointInPrincipalSpace( patchIds[i] );
			for (size_t j = 0; j < subspaceDimensions; j++)
			{
				const float point = projectedPoint[j];
				result[i*subspaceDimensions+j] = point;
			}
		}
		return result;
	}

	void IDistanceIndex::writeToStream(std::ostream& ofs)
	{
	}

	void  IDistanceIndex::loadFromStream(std::istream& is)
	{
	}

} // namespace

