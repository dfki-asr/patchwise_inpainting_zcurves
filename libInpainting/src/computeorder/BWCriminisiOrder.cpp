#include "stdafx.h"

#include "BWCriminisiOrder.h"

#include "Problem.h"
#include "Confidence.h"
#include "CriminisiDataTerm.h"
#include "model/volume/ByteVolume.h"

namespace ettention
{
    namespace inpainting
    {

		BWCriminisiOrder::BWCriminisiOrder( Problem* problem, ProgressReporter* progress, bool regularizeConfidence /*= false*/)
			: CriminisiOrder( problem, progress, regularizeConfidence )
		{
			dataTerm = new CriminisiDataTerm(problem->data, problem->mask, problem->patchSize);
		}

		BWCriminisiOrder::~BWCriminisiOrder()
		{

		}

	} // namespace inpainting
} // namespace ettention