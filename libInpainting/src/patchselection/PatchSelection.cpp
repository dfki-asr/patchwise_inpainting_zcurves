#include "stdafx.h"

#include "PatchSelection.h"

namespace inpainting
{

    PatchSelection::PatchSelection(libmmv::ByteVolume* dataVolume, libmmv::ByteVolume* maskVolume, libmmv::ByteVolume* dictionaryVolume)
        : dataVolume(dataVolume), maskVolume(maskVolume), dictionaryVolume(dictionaryVolume)
    {
    }

    PatchSelection::~PatchSelection()
    {
    }

	void PatchSelection::activateBruteForceFallback()
	{
		/* to be implemented if needed */
	}

	void PatchSelection::computeCostAfterIteration()
	{
		/* to be implemented if needed */
	}

	bool PatchSelection::initializeNewIteration()
	{
		/* to be implemented if needed */
		return false;
	}

} // namespace
