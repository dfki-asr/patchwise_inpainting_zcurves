#include "stdafx.h"

#include <mutex>
#include "L2CostFunctionWithWeight.h"
#include "dictionary/Dictionary.h"
#include "StatusFlags.h"
#include "../../libInpainting/src/Problem.h"

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

	void L2CostFunctionWithWeight::computeCostSingleThread(std::mutex *mutex,
															std::stack<std::vector<std::pair<int, int>>>* totalJob,
															BytePatchAccess8Bit* ptrDictionaryAccess)
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

				float distance = 0.0f;
				for (unsigned int i = 0; i < dataAccess.size(); i++)
				{
					const unsigned char pixelStatus = maskAccess[i];
					if (pixelStatus == EMPTY_REGION || pixelStatus == TARGET_REGION)
						continue;

					libmmv::Vec3ui vIndex = dataAccess.getPositionInVolume(i);

					unsigned char pA = (*ptrDictionaryAccess)[i];
					unsigned char pB = dataAccess[i];
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
		for (int idx = 0; idx < numWorker; idx++) {
			multiWorkerDictionaryAccess.push_back(dictionaryAccess);
		}

		std::stack<std::vector<std::pair<int, int>>> totalJob;
		std::vector<std::pair<int, int>> batchJob;
		int sizePerBatch = std::max(interval.length() / 1000, (unsigned int)200);
		for (int dictionaryIndex = interval.first; dictionaryIndex <= interval.last; dictionaryIndex++)
		{
			const int sourcePatchIndex = dictionary->getCompressedDictionary()[dictionaryIndex];
			batchJob.push_back(std::make_pair(sourcePatchIndex, dictionaryIndex - interval.first));

			if (batchJob.size() >= sizePerBatch || dictionaryIndex == interval.last) {
				totalJob.push(batchJob);
				batchJob.clear();
			}
		}

		resultCost = std::vector<float>(interval.length());
		std::vector<std::thread> allWorker;
		std::mutex mutex;

		dataAccess.setPatchId(indexOfTargetPatch);
		maskAccess.setPatchId(indexOfTargetPatch);
		for (int idx = 0; idx < numWorker; idx++)
		{
			allWorker.push_back(std::thread(&L2CostFunctionWithWeight::computeCostSingleThread,
											this, &mutex, &totalJob,
											&(multiWorkerDictionaryAccess[idx])));
		}

		for (auto& worker : allWorker) {
			worker.join();
		}
	}
}
