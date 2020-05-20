#include "stdafx.h"

#include <iomanip>

#include "KNNCandidate.h"

namespace ettention
{
	namespace inpainting 
	{  

		KNNCandidate::KNNCandidate()
		{
		}

		KNNCandidate::KNNCandidate(float distance, unsigned int key)
			: distance(distance), key(key)
		{

		}

		KNNCandidate::KNNCandidate(const KNNCandidate& other)
			: distance(other.distance), key(other.key)
		{
		}

		KNNCandidate& KNNCandidate::operator=(const KNNCandidate& other)
		{
			distance = other.distance;
			key = other.key;
			return *this;
		}

		bool KNNCandidateNearer::operator()(KNNCandidate a, KNNCandidate b)
		{
			if (a.distance == b.distance)
				return a.key < b.key;
			return a.distance < b.distance;
		}

		std::ofstream& operator<<(std::ofstream& ofs, const KNNCandidate& knn)
		{
			ofs << knn.key << "(=" << std::fixed << std::setprecision(4) << knn.distance << ")";
			return ofs;
		}

	} // namespace inpainting
} // namespace ettention