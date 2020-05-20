#include "stdafx.h"

#include "IndexOptions.h"
#include "setup/parametersource/ParameterSource.h"

namespace ettention
{
    namespace inpainting
    {

		IndexOptions::IndexOptions()
		{
			knnFilterSize = 50;
			indexDimensions = 8;
			indexType = IndexType::ZCURVE_RGB;
			fractionOfPixelsCoveredByEachIndex = 0.6f;
			minSizeForRecursion = 1024;
		}

		IndexOptions::IndexOptions(const ParameterSource* parameterSource)
			: IndexOptions()
		{
			if (parameterSource->parameterExists("index.knnFilterSize"))
			{
				knnFilterSize = parameterSource->getIntParameter("index.knnFilterSize");
			}

			if (parameterSource->parameterExists("index.dimensions"))
			{
				indexDimensions = parameterSource->getIntParameter("index.dimensions");
			}

			if (parameterSource->parameterExists("index.type"))
			{
				indexType = parseIndexType(parameterSource->getStringParameter("index.type"));
			}

			if (parameterSource->parameterExists("index.coverPixels"))
			{
				fractionOfPixelsCoveredByEachIndex = parameterSource->getFloatParameter("index.coverPixels");
			}

			if (parameterSource->parameterExists("index.minSizeForRecursion"))
			{
				minSizeForRecursion = parameterSource->getIntParameter("index.minSizeForRecursion");
			}

			minSizeForParallelization = -1;
			if ( parameterSource->parameterExists("optimization.parallelComputation")
			 &&  parameterSource->getBoolParameter("optimization.parallelComputation") == true )
			{
				if (parameterSource->parameterExists("index.minSizeForParallelization"))
				{
					minSizeForParallelization = parameterSource->getIntParameter("index.minSizeForParallelization");
				}
			}

			cacheFileName = "";
			if (parameterSource->parameterExists("index.cacheFileName"))
			{
				cacheFileName = parameterSource->getPathParameter("index.cacheFileName");
			}

		}

		IndexOptions::~IndexOptions()
		{

		}

		int IndexOptions::getKNNFilterSize()
		{
			return knnFilterSize;
		}

		int IndexOptions::getIndexDimensions()
		{
			return indexDimensions;
		}

		ettention::inpainting::IndexOptions::IndexType IndexOptions::getIndexType()
		{
			return indexType;
		}

		float IndexOptions::getFractionOfPixelsCoveredByEachIndex()
		{
			return fractionOfPixelsCoveredByEachIndex;
		}

		int IndexOptions::getMinSizeForRecursion()
		{
			return minSizeForRecursion;
		}

		int IndexOptions::getMinSizeForParallelization()
		{
			return minSizeForParallelization;
		}

		bool IndexOptions::shouldUseParallelization()
		{
			return ( minSizeForParallelization != -1 );
		}

		std::string IndexOptions::getCacheFileName()
		{
			return cacheFileName.string();
		}

		ettention::inpainting::IndexOptions::IndexType IndexOptions::parseIndexType(std::string value)
		{
			if (value == "none")
				return IndexType::NONE;
			if (value == "zcurve_rgb")
				return IndexType::ZCURVE_RGB;
			if (value == "zcurve_3d")
				return IndexType::ZCURVE_3D;
			if (value == "idistance_rgb")
				return IndexType::IDISTANCE_RGB;
			if (value == "patch_match")
				return IndexType::PATCH_MATCH;
			throw std::invalid_argument("illegal IndexType " + value );
		}

	} // namespace
} // namespace
