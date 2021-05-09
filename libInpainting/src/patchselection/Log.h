#pragma once

#include "libmmv/math/Vec3.h"
#include "setup/InpaintingDebugParameters.h" 
#include "zcurve/KNNCandidate.h"

namespace libmmv
{
	class FloatVolume;
	class ByteVolume;
}

namespace inpainting {

    class Log 
    {
    public:
		Log( InpaintingDebugParameters* debugParameters = nullptr );
        virtual ~Log();

		void setPatchNorm( float norm );
		void setPatchNormBruteForce( float norm );
		void setSourcePosition( libmmv::Vec3i position );
		void setTargetPosition( libmmv::Vec3i position );
		void setSelectivity( float selectivity );
		void setNJobs( int jobs );
		void setKnnCandidates(const std::list<KNNCandidate>& knnCandidates);
		void setTargetPointInPrincipalSpace( const NDPoint& target );
		void reportPatchSelection();
		void setIndexName(std::string name);
		void setBestBruteForcePosition(libmmv::Vec3i position);

	protected:
		void outputHeader();

	protected:
		InpaintingDebugParameters* debugParameters;
		std::ofstream ofs;

		float norm;
		float norm_bruteforce;
		float selectivity;
		int njobs;
		libmmv::Vec3i sourcePosition;
		libmmv::Vec3i targetPosition;
		std::list<KNNCandidate> knnCandidates;
		NDPoint targetPointInPrincipalSpace;
		std::string indexName;

		libmmv::Vec3i bestBruteForcePosition;
    };
}
