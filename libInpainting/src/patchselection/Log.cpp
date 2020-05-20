#include "stdafx.h"

#include "Log.h"

namespace ettention
{
    namespace inpainting
    {

		Log::Log(InpaintingDebugParameters* debugParameters)
			: debugParameters(debugParameters)
			, norm( 0.0f ), norm_bruteforce( 0.0f ), selectivity( 0.0f ), njobs( 0 )
		{
			if (debugParameters == nullptr)
				return;

			if (!debugParameters->shouldLogAcceleration())
				return;

			ofs = std::ofstream(debugParameters->getDebugInfoPath() + debugParameters->getLogFileName());
			outputHeader();
		}

		Log::~Log()
		{
			if (debugParameters == nullptr)
				return;

			if (!debugParameters->shouldLogAcceleration())
				return;
			
			ofs.close();
		}

		void Log::setPatchNorm(float norm)
		{
			this->norm = norm;
		}

		void Log::setPatchNormBruteForce(float norm)
		{
			this->norm_bruteforce = norm;
		}

		void Log::setSourcePosition(Vec3i position)
		{
			this->sourcePosition = position;
		}

		void Log::setTargetPosition(Vec3i position)
		{
			this->targetPosition = position;
		}

		void Log::setSelectivity(float selectivity)
		{
			this->selectivity = selectivity;
		}

		void Log::setNJobs(int jobs)
		{
			this->njobs = jobs;
		}

		void Log::reportPatchSelection()
		{
			ofs << sourcePosition << ";" << targetPosition << ";" << std::setprecision(4) << norm << ";" << std::setprecision(4) << norm_bruteforce << ";" << std::setprecision(2); // << selectivity << ";" << njobs << ";";
			for (auto it = knnCandidates.begin(); it != knnCandidates.end(); ++it)
				ofs << *it << ";";
			ofs << indexName << ";";
			ofs << targetPointInPrincipalSpace << ";";
			if (debugParameters->shouldDoubleCheckUsingBruteForce())
			{
				ofs << bestBruteForcePosition << ";";
			}
			ofs << std::endl;
		}

		void Log::setIndexName(std::string name)
		{
			indexName = name;
		}

		void Log::setBestBruteForcePosition(Vec3i position)
		{
			bestBruteForcePosition = position;
		}

		void Log::setKnnCandidates(const std::list<KNNCandidate>& candidates)
		{
			knnCandidates = candidates;
		}

		void Log::setTargetPointInPrincipalSpace(const NDPoint& target)
		{
			targetPointInPrincipalSpace = target;
		}

		void Log::outputHeader()
		{
			std::string headerString = "sourcePosition;targetPosition;norm;norm_bruteforce;candidates;index;target_in_principal_space";
			if (debugParameters->shouldDoubleCheckUsingBruteForce())
			{
				headerString = "sourcePosition;targetPosition;norm;norm_bruteforce;candidates;index;target_in_principal_space;bestBruteForceCoordinate";
			}
			ofs << headerString << std::endl;
		}

	} // namespace
} // namespace
