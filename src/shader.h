#ifndef __SHADER__
#define __SHADER__

#include "base.h"
#include "mesh.h"

VkShaderModule loadShader(VkDevice device, const char* path);

VkPipelineLayout createPipelineLayout(VkDevice device);

VkPipeline createPipeline(
    VkDevice device, VkPipelineLayout layout, VkShaderModule vs, VkShaderModule fs, VkRenderPass renderPass
);

#endif /* __SHADER__ */
