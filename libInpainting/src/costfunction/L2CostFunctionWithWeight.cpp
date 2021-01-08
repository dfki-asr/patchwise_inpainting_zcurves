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
												 std::stack<std::vector<std::pair<int, int>>>* totalJob,
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
				auto batchJob = totalJob->top();
				totalJob->pop();
				mutex->unlock();

				for (auto& job : batchJob) {
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

			std::stack<std::vector<std::pair<int, int>>> totalJob;
			std::vector<std::pair<int, int>> batchJob;
			for (int dictionaryIndex = interval.first; dictionaryIndex <= interval.last; dictionaryIndex++)
			{
				const int sourcePatchIndex = dictionary->getCompressedDictionary()[dictionaryIndex];
				batchJob.push_back(std::make_pair(sourcePatchIndex, dictionaryIndex - interval.first));

				if (batchJob.size() >= interval.length()/1000 || dictionaryIndex == interval.last) {
					totalJob.push(batchJob);
					batchJob.clear();
				}
			}

			resultCost = std::vector<float>(interval.length());
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

			for (auto& worker : allWorker) {
				worker.join();
			}
		}
	}
}