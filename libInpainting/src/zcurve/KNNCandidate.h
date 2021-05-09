#pragma once

#include "ZCurveEntry.h"

namespace inpainting 
{

	class KNNCandidate
	{
	public:
		KNNCandidate();
		KNNCandidate(float distance, unsigned int key );
		KNNCandidate(const KNNCandidate& other);
		KNNCandidate& operator=(const KNNCandidate& other);

		float distance;
		unsigned int key;
	};

	struct KNNCandidateNearer
	{
		bool operator()(KNNCandidate a, KNNCandidate  b);
	};

	std::ofstream& operator<<(std::ofstream& ofs, const KNNCandidate& knn);

} // namespace inpainting
