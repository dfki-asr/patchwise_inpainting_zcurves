#include "stdafx.h"

#include "DictionaryDebugOutput.h"

#include "libmmv/algorithm/Coordinates.h"
#include "libmmv/model/volume/ByteVolume.h"
#include "libmmv/io/serializer/VolumeSerializer.h"
#include "setup/parameterset/OutputParameterSet.h"

namespace inpainting 
{
        
    libmmv::ByteVolume* DictionaryDebugOutput::extractDictionaryToVolume( libmmv::ByteVolume* dictionaryVolume, libmmv::Vec3ui patchSize, std::vector<unsigned int> patches )
    {
        libmmv::Vec3ui volumeResolution = dictionaryVolume->getProperties().getVolumeResolution();
        libmmv::Vec3ui targetVolumeSize = libmmv::Vec3ui(patchSize.x, patchSize.y, patchSize.z * (unsigned int)patches.size());
        libmmv::ByteVolume* result = new libmmv::ByteVolume(targetVolumeSize, 0.0f);
        unsigned int zoffset = 0;
        for (auto patchIndex : patches)
        {
            libmmv::Vec3ui sourceBasePosition = Unflatten3D(patchIndex, volumeResolution);
            sourceBasePosition -= patchSize / 2;
            libmmv::Vec3ui position(0, 0, 0);
            for (position.z = 0; position.z < patchSize.z; position.z++)
                for (position.y = 0; position.y < patchSize.y; position.y++)
                    for (position.x = 0; position.x < patchSize.x; position.x++)
                    {
                        libmmv::Vec3ui sourcePosition = sourceBasePosition + position;
                        unsigned int sourceIndex = Flatten3D(sourcePosition, volumeResolution);
                        float value = dictionaryVolume->getVoxelValue(sourceIndex);

                        libmmv::Vec3ui targetPosition = position;
                        targetPosition.z += zoffset * patchSize.z;
                        unsigned int targetIndex = Flatten3D(targetPosition, targetVolumeSize);
                        result->setVoxelToValue(targetIndex, value);
                    }
            zoffset++;
        }
        return result;
    }

    void DictionaryDebugOutput::writeDebugVolume( libmmv::ByteVolume* dictionaryVolume, libmmv::Vec3ui patchSize, std::string filename, std::vector<unsigned int> positions)
    {
        auto debugVolume = extractDictionaryToVolume(dictionaryVolume, patchSize, positions );
        cfg::OutputParameterSet parameter;
        libmmv::VolumeSerializer serializer;
        serializer.write( debugVolume, filename, parameter.getVoxelType(), parameter.getOrientation() );
        delete debugVolume;
    }

} // namespace inpainting
