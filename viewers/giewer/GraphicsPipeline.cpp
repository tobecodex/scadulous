#include "GraphicsPipeline.h"

#include <vector>
#include <fstream>
#include <stdexcept>

#include "Vulkan.h"

#include "Mesh.h"
#include "Device.h"
#include "SwapChain.h"
#include "DescriptorSet.h"
#include "DescriptorPool.h"
#include "DescriptorSetLayout.h"


VkPipelineRasterizationStateCreateInfo createRasterizationState()
{
  VkPipelineRasterizationStateCreateInfo rasterizer{};

  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth = 1.0f;
  rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  rasterizer.depthBiasConstantFactor = 0.0f; // Optional
  rasterizer.depthBiasClamp = 0.0f; // Optional
  rasterizer.depthBiasSlopeFactor = 0.0f; // Optional
  rasterizer.depthBiasEnable = VK_FALSE;

  return rasterizer;
}

VkPipelineVertexInputStateCreateInfo createVertexInputInfo(
  std::vector<VkVertexInputBindingDescription> &bindings, 
  std::vector<VkVertexInputAttributeDescription> &attributes
)
{
  VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
  vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputInfo.vertexBindingDescriptionCount = (uint32_t)bindings.size();
  vertexInputInfo.pVertexBindingDescriptions = bindings.data();
  vertexInputInfo.vertexAttributeDescriptionCount = (uint32_t)attributes.size();
  vertexInputInfo.pVertexAttributeDescriptions = attributes.data();
  return vertexInputInfo;
}

VkViewport createViewport(float w, float h)
{
  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = w;
  viewport.height = h;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  return viewport;
}

VkPipelineMultisampleStateCreateInfo createMultisampleStateInfo()
{
  VkPipelineMultisampleStateCreateInfo multisampling{};
  multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  multisampling.minSampleShading = 1.0f; // Optional
  multisampling.pSampleMask = nullptr; // Optional
  multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
  multisampling.alphaToOneEnable = VK_FALSE; // Optional
  return multisampling;
}

VkPipelineColorBlendAttachmentState createColorBlendAttachmentState()
{
  VkPipelineColorBlendAttachmentState colorBlendAttachment{};
  colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  colorBlendAttachment.blendEnable = VK_FALSE;
  colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
  colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
  colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
  colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
  colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
  colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional
  return colorBlendAttachment;
}

VkPipelineColorBlendStateCreateInfo createColorBlendingStateInfo(
  std::vector<VkPipelineColorBlendAttachmentState> &attachments
)
{
  VkPipelineColorBlendStateCreateInfo colorBlending{};
  colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlending.logicOpEnable = VK_FALSE;
  colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
  colorBlending.attachmentCount = (uint32_t)attachments.size();
  colorBlending.pAttachments = attachments.data();
  colorBlending.blendConstants[0] = 0.0f; // Optional
  colorBlending.blendConstants[1] = 0.0f; // Optional
  colorBlending.blendConstants[2] = 0.0f; // Optional
  colorBlending.blendConstants[3] = 0.0f; // Optional
  return colorBlending;
}

VkPushConstantRange createPushConstantRange()
{
  VkPushConstantRange pushConstantRange = {};
  pushConstantRange.offset = 0;
  pushConstantRange.size = sizeof(glm::mat4);
  pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  return pushConstantRange;
}

VkPipelineDepthStencilStateCreateInfo createDepthStencilStateInfo()
{
  VkPipelineDepthStencilStateCreateInfo depthStencil{};
  depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depthStencil.depthTestEnable = VK_TRUE;
  depthStencil.depthWriteEnable = VK_TRUE;
  depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
  depthStencil.depthBoundsTestEnable = VK_FALSE;
  depthStencil.minDepthBounds = 0.0f; // Optional
  depthStencil.maxDepthBounds = 1.0f; // Optional
  depthStencil.stencilTestEnable = VK_FALSE;
  depthStencil.front = {}; // Optional
  depthStencil.back = {}; // Optional
  return depthStencil;
}

VkPipelineInputAssemblyStateCreateInfo createInputAssempblyStateInfo(VkPrimitiveTopology topology) 
{
  VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo{};
  inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssemblyStateCreateInfo.topology = topology;
  inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;
  return inputAssemblyStateCreateInfo;
}

GraphicsPipeline::GraphicsPipeline(
  const SwapChain &swapChain, std::vector<VkDescriptorSetLayout> &descriptorSetLayouts
) : _layoutInfo{}, _pipelineInfo{}
{
  _descriptorSetLayouts = descriptorSetLayouts;

  _vertexInputStateInfo = createVertexInputInfo(
    LitVertex::getVertexBindingDescriptions(),
    LitVertex::getVertexAttributeDescriptions()
  );

  setPrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

  _viewports.push_back(createViewport(
    (float)swapChain.extent().width, (float)swapChain.extent().height
  ));

  _scissors.push_back(VkRect2D{{0, 0}, swapChain.extent()});

  _viewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  _viewportStateInfo.viewportCount = (uint32_t)_viewports.size();
  _viewportStateInfo.pViewports = _viewports.data();
  _viewportStateInfo.scissorCount = (uint32_t)_scissors.size();
  _viewportStateInfo.pScissors = _scissors.data();

  _rasterizationStateInfo = createRasterizationState();
  _multisampleStateInfo = createMultisampleStateInfo();

  _colorBlendAttachments.push_back(createColorBlendAttachmentState());
  _colorBlendStateInfo = createColorBlendingStateInfo(_colorBlendAttachments);

  _depthStencilStateInfo = createDepthStencilStateInfo();
}

GraphicsPipeline::~GraphicsPipeline()
{
}

void GraphicsPipeline::setPrimitiveTopology(VkPrimitiveTopology topology)
{
  _inputAssemblyStateInfo = createInputAssempblyStateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
}

void GraphicsPipeline::addShaderStage(const char *path, VkShaderStageFlagBits flags) 
{
  std::ifstream file(path, std::ios::ate | std::ios::binary);
  if (!file.is_open()) {
   throw std::runtime_error("failed to open file!");
  }
  size_t fileSize = (size_t) file.tellg();
  std::vector<char> buffer(fileSize);
  file.seekg(0);
  file.read(buffer.data(), fileSize);
  file.close();

  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = buffer.size();
  createInfo.pCode = (const uint32_t*)(buffer.data());

  VkShaderModule shaderModule;
  if (vkCreateShaderModule(Vulkan::ctx().device(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
    throw std::runtime_error("failed to create shader module!");
  }

  VkPipelineShaderStageCreateInfo shaderStageInfo{};

  shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  shaderStageInfo.stage = flags;
  shaderStageInfo.module = shaderModule;
  shaderStageInfo.pName = "main";

  _shaderStages.push_back(shaderStageInfo);
}

void GraphicsPipeline::addPushConstantRange()
{
  VkPushConstantRange pushConstant = createPushConstantRange();
  _pushConstantRanges.push_back(pushConstant);
}

void GraphicsPipeline::createLayout()
{
  _layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  _layoutInfo.setLayoutCount = (uint32_t)_descriptorSetLayouts.size();
  _layoutInfo.pSetLayouts = _descriptorSetLayouts.data();
  _layoutInfo.pushConstantRangeCount = (uint32_t)_pushConstantRanges.size();
  _layoutInfo.pPushConstantRanges = _pushConstantRanges.data();

  if (vkCreatePipelineLayout(Vulkan::ctx().device(), &_layoutInfo, nullptr, &_pipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout!");
  }
}

void GraphicsPipeline::createPipeline(VkRenderPass renderPass)
{
  _pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  _pipelineInfo.stageCount = (uint32_t)_shaderStages.size();
  _pipelineInfo.pStages = _shaderStages.data();

  _pipelineInfo.pVertexInputState = &_vertexInputStateInfo;
  _pipelineInfo.pInputAssemblyState = &_inputAssemblyStateInfo;
  _pipelineInfo.pViewportState = &_viewportStateInfo;
  _pipelineInfo.pRasterizationState = &_rasterizationStateInfo;
  _pipelineInfo.pMultisampleState = &_multisampleStateInfo;
  _pipelineInfo.pDepthStencilState = &_depthStencilStateInfo;
  _pipelineInfo.pColorBlendState = &_colorBlendStateInfo;
  _pipelineInfo.pDynamicState = nullptr; // Optional

  _pipelineInfo.layout = _pipelineLayout;

  _pipelineInfo.renderPass = renderPass;
  _pipelineInfo.subpass = 0;

  _pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
  _pipelineInfo.basePipelineIndex = -1; // Optional
  
  if (vkCreateGraphicsPipelines(
    Vulkan::ctx().device(), VK_NULL_HANDLE, 1, &_pipelineInfo, nullptr, &_graphicsPipeline) != VK_SUCCESS
  ) {
    throw std::runtime_error("failed to create graphics pipeline!");
  }
}