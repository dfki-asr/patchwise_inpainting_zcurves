#include "stdafx.h"

#include "DictionaryDebugOutput.h"

#include "algorithm/Coordinates.h"
#include "model/volume/ByteVolume.h"
#include "io/serializer/VolumeSerializer.h"
#include "setup/parameterset/OutputParameterSet.h"

namespace ettention
{
	namespace inpainting 
	{
        
		ByteVolume* DictionaryDebugOutput::extractDictionaryToVolume( ByteVolume* dictionaryVolume, Vec3ui patchSize, std::vector<unsigned int> patches )
        {
            Vec3ui volumeResolution = dictionaryVolume->getProperties().getVolumeResolution();
            Vec3ui targetVolumeSize = Vec3ui(patchSize.x, patchSize.y, patchSize.z * (unsigned int)patches.size());
			ByteVolume* result = new ByteVolume(targetVolumeSize, 0.0f);
            unsigned int zoffset = 0;
            for (auto patchIndex : patches)
            {
                Vec3ui sourceBasePosition = Unflatten3D(patchIndex, volumeResolution);
                sourceBasePosition -= patchSize / 2;
                Vec3ui position(0, 0, 0);
                for (position.z = 0; position.z < patchSize.z; position.z++)
                    for (position.y = 0; position.y < patchSize.y; position.y++)
                        for (position.x = 0; position.x < patchSize.x; position.x++)
                        {
                            Vec3ui sourcePosition = sourceBasePosition + position;
                            unsigned int sourceIndex = Flatten3D(sourcePosition, volumeResolution);
                            float value = dictionaryVolume->getVoxelValue(sourceIndex);

                            Vec3ui targetPosition = position;
                            targetPosition.z += zoffset * patchSize.z;
                            unsigned int targetIndex = Flatten3D(targetPosition, targetVolumeSize);
                            result->setVoxelToValue(targetIndex, value);
                        }
                zoffset++;
            }
            return result;
        }

        void DictionaryDebugOutput::writeDebugVolume( ByteVolume* dictionaryVolume, Vec3ui patchSize, std::string filename, std::vector<unsigned int> positions)
        {
            auto debugVolume = extractDictionaryToVolume(dictionaryVolume, patchSize, positions );
            OutputParameterSet parameter;
            VolumeSerializer serializer;
            serializer.write( debugVolume, filename, parameter.getVoxelType(), parameter.getOrientation() );
            delete debugVolume;
        }

    } // namespace inpainting
} // namespace ettention