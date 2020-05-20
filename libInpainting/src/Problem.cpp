#include "stdafx.h"

#include "Problem.h"

#include "model/volume/ByteVolume.h"

namespace ettention
{
    namespace inpainting 
    {

        Problem::Problem()
            : data(nullptr)
			, mask(nullptr)
			, dictionaryMask(nullptr)
			, denseScan(nullptr)
            , dictionaryVolume(nullptr)
        {
        }

        Problem::~Problem()
        {
            if (data != nullptr)
                delete data;

			if (mask != nullptr)
				delete mask;

			if (dictionaryMask != nullptr && dictionaryMask != mask )
				delete dictionaryMask;

            if (denseScan != nullptr)
                delete denseScan;

            if( dictionaryVolume != nullptr && dictionaryVolume != data )
                delete dictionaryVolume;
        }

    }
}
