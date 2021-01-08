#include "stdafx.h"

#include <mutex>
#include "L2CostFunctionWithWeight.h"
#include "dictionary/Dictionary.h"
#include "StatusFlags.h"
#include "../../libInpainting/src/Problem.h"

namespace ettention
{
	namespace inpainting
	{
		L2CostFunctionWithWeight::L2CostFunctionWithWeight(Problem* problem, Dictionary* dictionary)
			: DictionaryBasedCostFunctionKernel( problem, dictionary )
		{
			if (problem->patchSize.z != problem->costWeight.size()) {
				std::ostringstream stringStream;
				stringStream << "size of costWeight and patchSize.z don't match: ";
				stringStream << "size of costWeight is " << problem->costWeight.size() << " while patchSize.z is " << problem->patchSize.z << std::endl;
				std::string errorStr = stringStream.str();

				throw std::runtime_error(errorStr);
			}
		}

		L2CostFunctionWithWeight::~L2CostFunctionWithWeight()
		{
		}

		void L2CostFunctionWithWeight::singleJob(std::mutex *mutex,
												 std::stack<std::pair<int, int>>* totalJob,
												 BytePatchAccess8Bit* ptrDictionaryAccess,
												 BytePatchAccess8Bit* ptrDataAccess,
												 BytePatchAccess8Bit* ptrMaskAccess)
		{
			while (true) {
				mutex->lock();
				if (totalJob->size() == 0) {
					mutex->unlock();
					return;
				}
				auto job = totalJob->top();
				totalJob->pop();
				mutex->unlock();

				int sourcePatchIndex = job.first;
				int vecPos = job.second;

				ptrDictionaryAccess->setPatchId(sourcePatchIndex);
				ptrDataAccess->setPatchId(indexOfTargetPatch);
				ptrMaskAccess->setPatchId(indexOfTargetPatch);

				float distance = 0.0f;
				for (unsigned int i = 0; i < ptrDataAccess->size(); i++)
				{
					const unsigned char pixelStatus = (*ptrMaskAccess)[i];
					if (pixelStatus == EMPTY_REGION || pixelStatus == TARGET_REGION)
						continue;

					Vec3ui vIndex = ptrDataAccess->getPositionInVolume(i);

					unsigned char pA = (*ptrDictionaryAccess)[i];
					unsigned char pB = (*ptrDataAccess)[i];
					const float distanceInDimension = (float)(pB - pA);
					distance += distanceInDimension * distanceInDimension * problem->costWeight[vIndex.z];
				}

				resultCost[vecPos] = distance;
			}
		}
		
		void L2CostFunctionWithWeight::computeCostForInterval(IndexInterval interval)
		{
			int numWorker = std::thread::hardware_concurrency() / 2;

			std::vector<BytePatchAccess8Bit> multiWorkerDictionaryAccess;
			std::vector<BytePatchAccess8Bit> multiWorkerDataAccess;
			std::vector<BytePatchAccess8Bit> multiWorkerMaskAccess;

			for (int idx = 0; idx < numWorker; idx++) {
				multiWorkerDictionaryAccess.push_back(dictionaryAccess);
				multiWorkerDataAccess.push_back(dataAccess);
				multiWorkerMaskAccess.push_back(maskAccess);
			}

			std::stack<std::pair<int, int>> totalJob;
			for (int dictionaryIndex = interval.first; dictionaryIndex <= interval.last; dictionaryIndex++)
			{
				const int sourcePatchIndex = dictionary->getCompressedDictionary()[dictionaryIndex];
				totalJob.push(std::make_pair(sourcePatchIndex, dictionaryIndex - interval.first));
			}

			resultCost = std::vector<float>(totalJob.size());
			std::vector<std::thread> allWorker;
			std::mutex mutex;
			for (int idx = 0; idx < numWorker; idx++)
			{
				allWorker.push_back(std::thread(&L2CostFunctionWithWeight::singleJob,
												this, &mutex, &totalJob,
												&(multiWorkerDictionaryAccess[idx]),
												&(multiWorkerDataAccess[idx]),
												&(multiWorkerMaskAccess[idx])));
			}

			for (auto& w : allWorker) {
				w.join();
			}

			return;

			// original single thread implementation
			for (int dictionaryIndex = interval.first; dictionaryIndex <= interval.last; dictionaryIndex++)
			{
				const int patchIndex = dictionary->getCompressedDictionary()[ dictionaryIndex ];

				dictionaryAccess.setPatchId(patchIndex);
				dataAccess.setPatchId(indexOfTargetPatch);
				maskAccess.setPatchId(indexOfTargetPatch);

				const float cost = computeCostFunction( patchIndex, &dictionaryAccess, &dataAccess, &maskAccess);

				resultCost.push_back(cost);
			}
		}

		float L2CostFunctionWithWeight::computeCostFunction( unsigned int indexOfSourcePatch,
															 BytePatchAccess8Bit *ptrDictionaryAccess,
															 BytePatchAccess8Bit *ptrDataAccess,
															 BytePatchAccess8Bit *ptrMaskAccess)
		{
			ptrDictionaryAccess->setPatchId(indexOfSourcePatch);
			ptrDataAccess->setPatchId(indexOfTargetPatch);
			ptrMaskAccess->setPatchId(indexOfTargetPatch);

			float distance = 0.0f;
			for (unsigned int i = 0; i < ptrDataAccess->size(); i++)
			{
				const unsigned char pixelStatus = (*ptrMaskAccess)[i];
				if (pixelStatus == EMPTY_REGION || pixelStatus == TARGET_REGION)
					continue;
			
				Vec3ui vIndex = ptrDataAccess->getPositionInVolume(i);
				
				unsigned char pA = (*ptrDictionaryAccess)[i];
				unsigned char pB = (*ptrDataAccess)[i];
				const float distanceInDimension = (float)(pB - pA);
				distance += distanceInDimension * distanceInDimension * problem->costWeight[vIndex.z];
			}

			return std::sqrtf(distance);
		}
	}
}