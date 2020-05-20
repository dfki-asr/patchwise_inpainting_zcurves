#include "stdafx.h"

#include "ZCurveEntry.h"

namespace ettention
{
	namespace inpainting 
	{  
		ZCurveEntry::ZCurveEntry()
		{
		}

		ZCurveEntry::ZCurveEntry(unsigned int key, NDPoint value)
			: key(key), value(value)
		{

		}

		ZCurveEntry::ZCurveEntry(const ZCurveEntry& other)
			: key(other.key), value(other.value)
		{
		}

		ZCurveEntry& ZCurveEntry::operator=(const ZCurveEntry& other)
		{
			key = other.key;
			value = other.value;
			return *this;
		}

		void ZCurveEntry::writeToStream(std::ostream& os)
		{
			binary_write(os, key);
			os.write(reinterpret_cast<const char*>(&value[0]), value.size() * sizeof(unsigned char) );
		}

		void ZCurveEntry::loadFromStream(std::istream& is)
		{
			performLoadFromStream( is );
		}

		void ZCurveEntry::performLoadFromStream(std::istream& is)
		{
			binary_read(is, key);
			is.read(reinterpret_cast<char*>(&value[0]), value.size() * sizeof(unsigned char));
		}

	} // namespace inpainting
} // namespace ettention