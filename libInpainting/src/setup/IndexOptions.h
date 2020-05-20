#pragma once

#include "setup/parameterset/ParameterSet.h"

namespace ettention
{
	class ParameterSource;

	namespace inpainting 
	{
		class IndexOptions : public ParameterSet 
		{
		public:
			enum class IndexType
			{
				NONE,
				ZCURVE_RGB,
				ZCURVE_3D,
				IDISTANCE_RGB,
				PATCH_MATCH
			};

			IndexOptions();
			IndexOptions(const ParameterSource* parameterSource);
			virtual ~IndexOptions();

			int getKNNFilterSize();
			int getIndexDimensions();
			IndexType getIndexType();
			float getFractionOfPixelsCoveredByEachIndex();
			int getMinSizeForRecursion();
			int getMinSizeForParallelization();
			bool shouldUseParallelization();
			std::string getCacheFileName();

		protected:
			IndexType parseIndexType( std::string value );

		public:
			int knnFilterSize;
			int indexDimensions;
			float fractionOfPixelsCoveredByEachIndex;
			IndexType indexType;
			int minSizeForRecursion;
			int minSizeForParallelization;
			std::filesystem::path cacheFileName;
		};
    }
}