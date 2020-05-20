#include "stdafx.h"

#ifdef _DEBUG
#undef new 
#endif

#include "index/PatchAccess8Bit.h"
#include "algorithm/Coordinates.h"

#include "PCASubspace.h"

using namespace daal;
using namespace daal::data_management;
using namespace daal::algorithms;

namespace ettention
{
    namespace inpainting 
    { 

		template<class TDATA>
		PCASubspace<TDATA>::PCASubspace(ByteVolume* data, ByteVolume* dictionary, Vec3ui patchSize, std::vector<unsigned int>& permutation, std::vector<unsigned int>& patchKeys, unsigned int subspaceDimensions )
			: patchSize(patchSize), dictionaryAccess(dictionary, patchSize, permutation ), data(data), dataAccess(data, patchSize, permutation), permutation( permutation ), subspaceDimensions( subspaceDimensions )
		{
			daal::algorithms::pca::Online<float> pca_algorithm;
			size_t blockSize = 1024 * 8;

			for (size_t blockStart = 0; blockStart < patchKeys.size(); blockStart += blockSize )
			{
				SharedPtrHNT dataBlock = loadVariableBlock( patchKeys, blockStart, blockSize );
				pca_algorithm.input.set( daal::algorithms::pca::data, dataBlock );
				pca_algorithm.compute();
			}
			pca_algorithm.finalizeCompute();
			extractEigenvectors( pca_algorithm );
			determineDataRange( patchKeys );
		}


		template<class TDATA>
		PCASubspace<TDATA>::PCASubspace(std::istream& is, ByteVolume* data )
			: data(data)
		{
			loadFromStream( is );
			dataAccess = BytePatchAccess8Bit(data, patchSize, permutation);
		}

		template<class TDATA>
		PCASubspace<TDATA>::~PCASubspace()
		{
		}

		template<class TDATA>
		std::vector<TDATA> PCASubspace<TDATA>::getDataPointInPrincipalSpace(unsigned int patchID)
		{
			dataAccess.setPatchId(patchID);
			std::vector<float> pointInVectorSpace(permutation.size());
			for ( unsigned int dimension = 0; dimension < (unsigned int) permutation.size(); dimension++ )
			{
				pointInVectorSpace[dimension] = (float)dataAccess[dimension];
			}

			std::vector<float> resultAsFloat(subspaceDimensions);
			cblas_sgemm(
				CblasRowMajor,
				CblasNoTrans,
				CblasTrans,
				1,
				subspaceDimensions,
				(int) permutation.size(),
				1.0f,
				&pointInVectorSpace[0],
				(int) permutation.size(),
				&eigvecs[0],
				(int) permutation.size(),
				0.0f,
				&resultAsFloat[0],
				subspaceDimensions);

			normalizeDataRange( resultAsFloat );
			std::vector<TDATA> castResult( resultAsFloat.size() );
			for (size_t i = 0; i < resultAsFloat.size(); i++)
				castResult[i] = (TDATA) resultAsFloat[i];
			return castResult;
		}

		template<class TDATA>
		std::vector<TDATA> ettention::inpainting::PCASubspace<TDATA>::getDataPointInPrincipalSpace(Vec3ui patchCenter)
		{
			unsigned int patchId = Flatten3D( patchCenter, data->getProperties().getVolumeResolution() );
			return getDataPointInPrincipalSpace(patchId);
		}

		template<class TDATA>
		std::vector<TDATA> PCASubspace<TDATA>::getDictionaryPointInPrincipalSpace( unsigned int patchKey )
		{
			std::vector<float> resultAsFloat( subspaceDimensions );
			transferDictionaryPointToUnscaledPrincipalSpace( patchKey, resultAsFloat );
			normalizeDataRange( resultAsFloat );

			std::vector<TDATA> castResult(resultAsFloat.size());
			for (size_t i = 0; i < resultAsFloat.size(); i++)
				castResult[i] = (TDATA)resultAsFloat[i];
			return castResult;
		}

		template<class TDATA>
		SharedPtrHNT ettention::inpainting::PCASubspace<TDATA>::loadVariableBlock(const std::vector<unsigned int>& patchKeys, size_t blockStart, size_t blockSize )
		{
			if ( blockStart + blockSize > patchKeys.size() )
				blockSize = patchKeys.size() - blockStart;

			auto dataTable = new daal::data_management::HomogenNumericTable<float>(permutation.size(), blockSize, daal::data_management::NumericTable::doAllocate, 0.0f);
			unsigned int nColumns = (unsigned int)permutation.size();
			for (unsigned int row = 0; row < blockSize; row++)
			{
				float* dataPtr = dataTable->operator[]( row );
				dictionaryAccess.setPatchId( patchKeys[blockStart + row] );
				for (unsigned int column = 0; column < nColumns; column++)
				{
					dataPtr[column] = (float)dictionaryAccess[column];
				}
			}
			return SharedPtrHNT(dataTable);
		}


		template<class TDATA>
		void ettention::inpainting::PCASubspace<TDATA>::transferDictionaryPointToUnscaledPrincipalSpace( unsigned int patchKey, std::vector<float>& buffer)
		{
			dictionaryAccess.setPatchId( patchKey );
			std::vector<float> pointInVectorSpace(permutation.size());
			for (unsigned int dimension = 0; dimension < (unsigned int)permutation.size(); dimension++)
			{
				pointInVectorSpace[dimension] = (float)dictionaryAccess[dimension];
			}

			cblas_sgemm(
				CblasRowMajor,
				CblasNoTrans,
				CblasTrans,
				1,
				subspaceDimensions,
				(int) permutation.size(),
				1.0f,
				&pointInVectorSpace[0],
				(int) permutation.size(),
				&eigvecs[0],
				(int) permutation.size(),
				0.0f,
				&buffer[0],
				subspaceDimensions);
		}

		template<class TDATA>
		void PCASubspace<TDATA>::extractEigenvectors(daal::algorithms::pca::Online<float>& pca_algorithm )
		{
			daal::services::SharedPtr<daal::algorithms::pca::Result> result = pca_algorithm.getResult();
			auto pca_loadings = daal::services::staticPointerCast<HNT, daal::data_management::NumericTable>(result->get(daal::algorithms::pca::eigenvectors));
			float* eigvecsf = pca_loadings->getArray();

			size_t sizeOfEigvecs = pca_loadings->getNumberOfRows() * pca_loadings->getNumberOfColumns();
			eigvecs.assign(eigvecsf, eigvecsf + sizeOfEigvecs);
		}

		template<class TDATA>
		void PCASubspace<TDATA>::determineDataRange( const std::vector<unsigned int>& patchKeys )
		{
			minValue = std::numeric_limits<float>::max();
			maxValue = std::numeric_limits<float>::min();

			std::vector<float> dataPoint(subspaceDimensions);

			for (size_t i = 0; i < patchKeys.size(); i++)
			{
				transferDictionaryPointToUnscaledPrincipalSpace( patchKeys[i], dataPoint );
				for (size_t i = 0; i < subspaceDimensions; i++)
				{
					minValue = (dataPoint[i] < minValue) ? dataPoint[i] : minValue;
					maxValue = (dataPoint[i] > maxValue) ? dataPoint[i] : maxValue;
				}
			}
		}

		template<class TDATA>
		void PCASubspace<TDATA>::normalizeDataRange( std::vector<float>& data )
		{
			const float dataRange = maxValue - minValue;
			for (size_t i = 0; i < data.size(); i++)
			{
				data[i] = ((data[i] - minValue) / dataRange) * 255.0f;
			}
		}

		template<class TDATA>
		void PCASubspace<TDATA>::loadFromStream(std::istream& is)
		{
			ensure_binary_read(is, "PCASubspace");

			binary_read(is, patchSize);
			binary_read(is, subspaceDimensions);

			binary_read(is, permutation);

			binary_read(is, minValue);
			binary_read(is, maxValue);

			binary_read(is, eigvecs);
		}

		template<class TDATA>
		void PCASubspace<TDATA>::writeToStream(std::ostream& os)
		{
			binary_write( os, "PCASubspace" );

			binary_write(os, patchSize );
			binary_write(os, subspaceDimensions);

			binary_write(os, permutation);

			binary_write(os, minValue);
			binary_write(os, maxValue);

			binary_write(os, eigvecs);
		}

		template class PCASubspace<float>;
		template class PCASubspace<unsigned char>;

	} // namespace inpainting
} // namespace ettention