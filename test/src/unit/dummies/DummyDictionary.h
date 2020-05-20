#pragma once

#include "dictionary/Dictionary.h"

namespace ettention
{
	class Volume;

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
} // namespace ettention
