#pragma once

#include "dictionary/Dictionary.h"

namespace libmmv
{
	class Volume;
}

namespace inpainting
{
	class ComputeFront;
	class Confidence;
	class DataTerm;

	class DummyDictionary : public Dictionary
	{
	public:
		DummyDictionary();
	};

} // namespace inpainting
