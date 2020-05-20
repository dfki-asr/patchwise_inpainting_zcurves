#pragma once

#include "math/Vec3.h"
#include "setup/InpaintingDebugParameters.h" 
#include "zcurve/KNNCandidate.h"

namespace ettention
{
    class FloatVolume;
    class ByteVolume; 

    namespace inpainting {

        class Log 
        {
        public:
			Log( InpaintingDebugParameters* debugParameters = nullptr );
            virtual ~Log();

			void setPatchNorm( float norm );
			void setPatchNormBruteForce( float norm );
			void setSourcePosition( Vec3i position );
			void setTargetPosition( Vec3i position );
			void setSelectivity( float selectivity );
			void setNJobs( int jobs );
			void setKnnCandidates(const std::list<KNNCandidate>& knnCandidates);
			void setTargetPointInPrincipalSpace( const NDPoint& target );
			void reportPatchSelection();
			void setIndexName(std::string name);
			void setBestBruteForcePosition(Vec3i position);

		protected:
			void outputHeader();

		protected:
			InpaintingDebugParameters* debugParameters;
			std::ofstream ofs;

			float norm;
			float norm_bruteforce;
			float selectivity;
			int njobs;
			Vec3i sourcePosition;
			Vec3i targetPosition;
			std::list<KNNCandidate> knnCandidates;
			NDPoint targetPointInPrincipalSpace;
			std::string indexName;

			Vec3i bestBruteForcePosition;
        };
    }
}