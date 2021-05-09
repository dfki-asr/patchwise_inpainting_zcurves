#pragma once

#include "index/NDPointPair.h"
#include "io/Serializable.h"

namespace inpainting 
{

	class ZCurveEntry : public Serializable
	{
	public:
		ZCurveEntry();
		ZCurveEntry(unsigned int key, NDPoint value);
		ZCurveEntry(const ZCurveEntry& other);
		ZCurveEntry& operator=(const ZCurveEntry& other);

		virtual void writeToStream(std::ostream& os) override;
		virtual void loadFromStream(std::istream& is) override;

		void performLoadFromStream(std::istream& is);

		unsigned int key;
		NDPoint value;
	};
 
} // namespace inpainting
