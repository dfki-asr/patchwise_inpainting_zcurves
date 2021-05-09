#pragma once

#include "CriminisiOrder.h"

namespace inpainting
{
    class BWCriminisiOrder : public CriminisiOrder
    {
    public:
		BWCriminisiOrder( Problem* problem, ProgressReporter* progress, bool regularizeConfidence = false);
        virtual ~BWCriminisiOrder();
    };

} // namespace inpainting