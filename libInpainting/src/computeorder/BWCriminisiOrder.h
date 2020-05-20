#pragma once

#include "CriminisiOrder.h"

namespace ettention
{
    namespace inpainting
    {
        class BWCriminisiOrder : public CriminisiOrder
        {
        public:
			BWCriminisiOrder( Problem* problem, ProgressReporter* progress, bool regularizeConfidence = false);
            virtual ~BWCriminisiOrder();
        };

    } // namespace inpainting
} // namespace ettention
