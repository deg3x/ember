internal void renderer_pipeline_init(renderer_pipeline_t* pipeline)
{
    renderer_pipeline_create_descriptor_pool();
    renderer_pipeline_create_descriptor_set_layouts(pipeline);
    renderer_pipeline_create_descriptor_sets(pipeline);
    renderer_pipeline_create_graphics_pipeline_layout(pipeline);
}

internal void renderer_pipeline_destroy(renderer_pipeline_t* pipeline)
{
    vkDestroyPipeline(g_renderer.device, pipeline->graphics_pipeline, NULL);
    vkDestroyPipelineLayout(g_renderer.device, pipeline->graphics_pipeline_layout, NULL);

    // NOTE(KB): No need to free descriptor sets individually
    //           If we want to do this we need to create the pool with VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT
    //
    //for (u32_t i = 0; i < RENDERER_FRAMES_IN_FLIGHT; i++)
    //{
    //    vkFreeDescriptorSets(g_renderer.device, g_renderer.descriptor_pool, RENDERER_FRAMES_IN_FLIGHT, pipeline->descriptor_sets);
    //}
    vkDestroyDescriptorSetLayout(g_renderer.device, pipeline->descriptor_set_layout_frame, NULL);
    vkDestroyDescriptorSetLayout(g_renderer.device, pipeline->descriptor_set_layout_global, NULL);
    vkDestroyDescriptorPool(g_renderer.device, g_renderer.descriptor_pool, NULL);
}

internal void renderer_pipeline_create_descriptor_pool()
{
    VkDescriptorPoolSize pool_sizes[3] = {0};
    pool_sizes[0].type                 = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    pool_sizes[0].descriptorCount      = RENDERER_FRAMES_IN_FLIGHT;
    pool_sizes[1].type                 = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    pool_sizes[1].descriptorCount      = 3 * RENDERER_FRAMES_IN_FLIGHT;
    pool_sizes[2].type                 = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    pool_sizes[2].descriptorCount      = RENDERER_TEX_COUNT_MAX;

    VkDescriptorPoolCreateInfo pool_info = {0};
    pool_info.sType                      = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags                      = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
    pool_info.poolSizeCount              = ARRAY_COUNT(pool_sizes);
    pool_info.pPoolSizes                 = pool_sizes;
    pool_info.maxSets                    = RENDERER_FRAMES_IN_FLIGHT + 1;

    VkResult vk_result = vkCreateDescriptorPool(g_renderer.device, &pool_info, NULL, &g_renderer.descriptor_pool);
    EMBER_ASSERT(vk_result == VK_SUCCESS);
}

internal void renderer_pipeline_create_descriptor_set_layouts(renderer_pipeline_t* pipeline)
{
    VkDescriptorSetLayoutBinding bindings_frame[4] = {0};
    bindings_frame[0].binding                      = 0;
    bindings_frame[0].descriptorCount              = 1;
    bindings_frame[0].descriptorType               = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    bindings_frame[0].stageFlags                   = VK_SHADER_STAGE_VERTEX_BIT;
    bindings_frame[0].pImmutableSamplers           = NULL;

    bindings_frame[1].binding                      = 1;
    bindings_frame[1].descriptorCount              = 1;
    bindings_frame[1].descriptorType               = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    bindings_frame[1].stageFlags                   = VK_SHADER_STAGE_VERTEX_BIT;
    bindings_frame[1].pImmutableSamplers           = NULL;

    bindings_frame[2].binding                      = 2;
    bindings_frame[2].descriptorCount              = 1;
    bindings_frame[2].descriptorType               = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    bindings_frame[2].stageFlags                   = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    bindings_frame[2].pImmutableSamplers           = NULL;

    bindings_frame[3].binding                      = 3;
    bindings_frame[3].descriptorCount              = 1;
    bindings_frame[3].descriptorType               = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    bindings_frame[3].stageFlags                   = VK_SHADER_STAGE_FRAGMENT_BIT;
    bindings_frame[3].pImmutableSamplers           = NULL;

    VkDescriptorSetLayoutCreateInfo binding_frame_info = {0};
    binding_frame_info.sType                           = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    binding_frame_info.bindingCount                    = ARRAY_COUNT(bindings_frame);
    binding_frame_info.pBindings                       = bindings_frame;

    VkResult create_result = vkCreateDescriptorSetLayout(
        g_renderer.device,
        &binding_frame_info,
        NULL,
        &pipeline->descriptor_set_layout_frame
    );
    EMBER_ASSERT(create_result == VK_SUCCESS);

    VkDescriptorSetLayoutBinding bindings_global[1] = {0};
    bindings_global[0].binding                      = 0;
    bindings_global[0].descriptorCount              = RENDERER_TEX_COUNT_MAX;
    bindings_global[0].descriptorType               = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bindings_global[0].stageFlags                   = VK_SHADER_STAGE_FRAGMENT_BIT;
    bindings_global[0].pImmutableSamplers           = NULL;

    VkDescriptorBindingFlags binding_flags[1] = {
        VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT
        | VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT
        | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT,
    };

    VkDescriptorSetLayoutBindingFlagsCreateInfo flags_info = {0};
    flags_info.sType                                       = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
    flags_info.bindingCount                                = ARRAY_COUNT(bindings_global);
    flags_info.pBindingFlags                               = binding_flags;

    VkDescriptorSetLayoutCreateInfo binding_global_info = {0};
    binding_global_info.sType                           = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    binding_global_info.pNext                           = &flags_info;
    binding_global_info.bindingCount                    = ARRAY_COUNT(bindings_global);
    binding_global_info.pBindings                       = bindings_global;
    binding_global_info.flags                           = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT;

    create_result = vkCreateDescriptorSetLayout(
        g_renderer.device,
        &binding_global_info,
        NULL,
        &pipeline->descriptor_set_layout_global
    );
    EMBER_ASSERT(create_result == VK_SUCCESS);
}

internal void renderer_pipeline_create_descriptor_sets(renderer_pipeline_t* pipeline)
{
    VkDescriptorSetLayout layouts[RENDERER_FRAMES_IN_FLIGHT] = {
        pipeline->descriptor_set_layout_frame,
        pipeline->descriptor_set_layout_frame
    };

    VkDescriptorSetAllocateInfo alloc_info_frame = {0};
    alloc_info_frame.sType                       = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info_frame.descriptorPool              = g_renderer.descriptor_pool;
    alloc_info_frame.descriptorSetCount          = RENDERER_FRAMES_IN_FLIGHT;
    alloc_info_frame.pSetLayouts                 = layouts;

    VkResult vk_result = vkAllocateDescriptorSets(g_renderer.device, &alloc_info_frame, pipeline->descriptor_set_frame);
    EMBER_ASSERT(vk_result == VK_SUCCESS);

    u32 descriptor_counts = RENDERER_TEX_COUNT_MAX;

    VkDescriptorSetVariableDescriptorCountAllocateInfo desc_count_alloc_info = {0};
    desc_count_alloc_info.sType                                              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO;
    desc_count_alloc_info.descriptorSetCount                                 = 1;
    desc_count_alloc_info.pDescriptorCounts                                  = &descriptor_counts;

    VkDescriptorSetAllocateInfo alloc_info_global = {0};
    alloc_info_global.sType                       = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info_global.pNext                       = &desc_count_alloc_info;
    alloc_info_global.descriptorPool              = g_renderer.descriptor_pool;
    alloc_info_global.descriptorSetCount          = 1;
    alloc_info_global.pSetLayouts                 = &pipeline->descriptor_set_layout_global;

    vk_result = vkAllocateDescriptorSets(g_renderer.device, &alloc_info_global, &pipeline->descriptor_set_global);
    EMBER_ASSERT(vk_result == VK_SUCCESS);

    for (u32 i = 0; i < RENDERER_FRAMES_IN_FLIGHT; i++)
    {
        u64 size_ubo  = FRAME_SIZE(GPU_MEM_SIZE_UBO);
        u64 size_ssbo = FRAME_SIZE(GPU_MEM_SIZE_SSBO);
        u64 size_draw = FRAME_SIZE(GPU_MEM_SIZE_DRAW);
        u64 size_mats = FRAME_SIZE(GPU_MEM_SIZE_MATS);

        VkDescriptorBufferInfo ubo_info = {0};
        ubo_info.buffer                 = g_renderer.resources.ubo_buf;
        ubo_info.offset                 = i * size_ubo;
        ubo_info.range                  = size_ubo;

        VkDescriptorBufferInfo ssbo_info = {0};
        ssbo_info.buffer                 = g_renderer.resources.ssbo_buf;
        ssbo_info.offset                 = i * size_ssbo;
        ssbo_info.range                  = size_ssbo;

        VkDescriptorBufferInfo draw_info = {0};
        draw_info.buffer                 = g_renderer.resources.draw_buf;
        draw_info.offset                 = i * size_draw;
        draw_info.range                  = size_draw;

        VkDescriptorBufferInfo mats_info = {0};
        mats_info.buffer                 = g_renderer.resources.mats_buf;
        mats_info.offset                 = i * size_mats;
        mats_info.range                  = size_mats;

        VkWriteDescriptorSet write_set[4] = {0};

        write_set[0].sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write_set[0].dstSet           = pipeline->descriptor_set_frame[i];
        write_set[0].dstBinding       = 0;
        write_set[0].dstArrayElement  = 0;
        write_set[0].descriptorCount  = 1;
        write_set[0].descriptorType   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        write_set[0].pImageInfo       = NULL;
        write_set[0].pBufferInfo      = &ubo_info;
        write_set[0].pTexelBufferView = NULL;

        write_set[1].sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write_set[1].dstSet           = pipeline->descriptor_set_frame[i];
        write_set[1].dstBinding       = 1;
        write_set[1].dstArrayElement  = 0;
        write_set[1].descriptorCount  = 1;
        write_set[1].descriptorType   = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        write_set[1].pImageInfo       = NULL;
        write_set[1].pBufferInfo      = &ssbo_info;
        write_set[1].pTexelBufferView = NULL;

        write_set[2].sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write_set[2].dstSet           = pipeline->descriptor_set_frame[i];
        write_set[2].dstBinding       = 2;
        write_set[2].dstArrayElement  = 0;
        write_set[2].descriptorCount  = 1;
        write_set[2].descriptorType   = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        write_set[2].pImageInfo       = NULL;
        write_set[2].pBufferInfo      = &draw_info;
        write_set[2].pTexelBufferView = NULL;

        write_set[3].sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write_set[3].dstSet           = pipeline->descriptor_set_frame[i];
        write_set[3].dstBinding       = 3;
        write_set[3].dstArrayElement  = 0;
        write_set[3].descriptorCount  = 1;
        write_set[3].descriptorType   = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        write_set[3].pImageInfo       = NULL;
        write_set[3].pBufferInfo      = &mats_info;
        write_set[3].pTexelBufferView = NULL;

        vkUpdateDescriptorSets(g_renderer.device, ARRAY_COUNT(write_set), write_set, 0, NULL);
    }
}

internal void renderer_pipeline_update_texture_bindings(renderer_pipeline_t* pipeline, i32 tex_id, i32 img_id, i32 smpl_id, i32 count)
{
    VkDescriptorImageInfo image_info = {0};
    image_info.imageLayout           = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    image_info.imageView             = g_renderer.resources.image_views[img_id];
    image_info.sampler               = g_renderer.resources.samplers[smpl_id];

    VkWriteDescriptorSet write_set = {0};
    write_set.sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write_set.dstSet           = pipeline->descriptor_set_global;
    write_set.dstBinding       = 0;
    write_set.dstArrayElement  = tex_id;
    write_set.descriptorCount  = count;
    write_set.descriptorType   = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    write_set.pImageInfo       = &image_info;
    write_set.pBufferInfo      = NULL;
    write_set.pTexelBufferView = NULL;

    vkUpdateDescriptorSets(g_renderer.device, 1, &write_set, 0, NULL);
}

internal void renderer_pipeline_create_graphics_pipeline_layout(renderer_pipeline_t* pipeline)
{
    VkDescriptorSetLayout layouts[2] = {
        pipeline->descriptor_set_layout_frame,
        pipeline->descriptor_set_layout_global
    };
    VkPipelineLayoutCreateInfo layout_info = {0};
    layout_info.sType                      = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layout_info.setLayoutCount             = 2;
    layout_info.pSetLayouts                = layouts;
    layout_info.pushConstantRangeCount     = 0;
    layout_info.pPushConstantRanges        = NULL;

    VkResult create_result = vkCreatePipelineLayout(g_renderer.device, &layout_info, NULL, &pipeline->graphics_pipeline_layout);
    EMBER_ASSERT(create_result == VK_SUCCESS);
}

internal void renderer_pipeline_create_graphics_pipeline(renderer_pipeline_t* pipeline, renderer_pipeline_create_info_t* info)
{
    EMBER_ASSERT(info != NULL);
    EMBER_ASSERT(info->shader_vert != NULL);
    EMBER_ASSERT(info->shader_frag != NULL);

    cpu_scratch_t scratch = cpu_scratch_begin(g_renderer.host_arena);
    u8* vert              = MEMORY_PUSH(scratch.arena, u8, KB(16));
    u8* frag              = MEMORY_PUSH(scratch.arena, u8, KB(16));

    u64 vert_size = platform_file_data(info->shader_vert, vert);
    u64 frag_size = platform_file_data(info->shader_frag, frag);

    VkShaderModule vert_module = renderer_pipeline_create_shader_module(vert, vert_size);
    VkShaderModule frag_module = renderer_pipeline_create_shader_module(frag, frag_size);

    VkPipelineShaderStageCreateInfo shader_info[2] = {0};
    shader_info[0].sType                           = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_info[0].stage                           = VK_SHADER_STAGE_VERTEX_BIT;
    shader_info[0].pName                           = "main";
    shader_info[0].module                          = vert_module;
    shader_info[0].pSpecializationInfo             = NULL;

    shader_info[1].sType                           = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_info[1].stage                           = VK_SHADER_STAGE_FRAGMENT_BIT;
    shader_info[1].pName                           = "main";
    shader_info[1].module                          = frag_module;
    shader_info[1].pSpecializationInfo             = NULL;

    VkVertexInputBindingDescription vertex_bindings = {0};
    vertex_bindings.binding                         = 0;
    vertex_bindings.stride                          = RENDERER_SIZE_VERTEX;
    vertex_bindings.inputRate                       = VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputAttributeDescription vertex_attr[RENDERER_VATTR_TYPE_count] = {0};
    vertex_attr[RENDERER_VATTR_TYPE_position].binding                        = 0;
    vertex_attr[RENDERER_VATTR_TYPE_position].location                       = 0;
    vertex_attr[RENDERER_VATTR_TYPE_position].offset                         = offsetof(vertex_t, position);
    vertex_attr[RENDERER_VATTR_TYPE_position].format                         = VK_FORMAT_R32G32B32_SFLOAT;

    vertex_attr[RENDERER_VATTR_TYPE_normal].binding                          = 0;
    vertex_attr[RENDERER_VATTR_TYPE_normal].location                         = 1;
    vertex_attr[RENDERER_VATTR_TYPE_normal].offset                           = offsetof(vertex_t, normal);
    vertex_attr[RENDERER_VATTR_TYPE_normal].format                           = VK_FORMAT_R32G32B32_SFLOAT;

    vertex_attr[RENDERER_VATTR_TYPE_tangent].binding                         = 0;
    vertex_attr[RENDERER_VATTR_TYPE_tangent].location                        = 2;
    vertex_attr[RENDERER_VATTR_TYPE_tangent].offset                          = offsetof(vertex_t, tangent);
    vertex_attr[RENDERER_VATTR_TYPE_tangent].format                          = VK_FORMAT_R32G32B32A32_SFLOAT;

    vertex_attr[RENDERER_VATTR_TYPE_color].binding                           = 0;
    vertex_attr[RENDERER_VATTR_TYPE_color].location                          = 3;
    vertex_attr[RENDERER_VATTR_TYPE_color].offset                            = offsetof(vertex_t, color);
    vertex_attr[RENDERER_VATTR_TYPE_color].format                            = VK_FORMAT_R32G32B32_SFLOAT;

    vertex_attr[RENDERER_VATTR_TYPE_uv0].binding                             = 0;
    vertex_attr[RENDERER_VATTR_TYPE_uv0].location                            = 4;
    vertex_attr[RENDERER_VATTR_TYPE_uv0].offset                              = offsetof(vertex_t, uv0);
    vertex_attr[RENDERER_VATTR_TYPE_uv0].format                              = VK_FORMAT_R32G32_SFLOAT;

    vertex_attr[RENDERER_VATTR_TYPE_uv1].binding                             = 0;
    vertex_attr[RENDERER_VATTR_TYPE_uv1].location                            = 5;
    vertex_attr[RENDERER_VATTR_TYPE_uv1].offset                              = offsetof(vertex_t, uv1);
    vertex_attr[RENDERER_VATTR_TYPE_uv1].format                              = VK_FORMAT_R32G32_SFLOAT;

    VkPipelineVertexInputStateCreateInfo vertex_input_info = {0};
    vertex_input_info.sType                                = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_info.vertexBindingDescriptionCount        = 1;
    vertex_input_info.pVertexBindingDescriptions           = &vertex_bindings;
    vertex_input_info.vertexAttributeDescriptionCount      = RENDERER_VATTR_TYPE_count;
    vertex_input_info.pVertexAttributeDescriptions         = vertex_attr;

    VkPipelineInputAssemblyStateCreateInfo input_assembly_info = {0};
    input_assembly_info.sType                                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly_info.topology                               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_assembly_info.primitiveRestartEnable                 = VK_FALSE;

    // NOTE(KB): We will use dynamic state so we dont need to specify a VkViewport and VkRect2D (viewport and scissor)

    VkDynamicState dynamic_states[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamic_info = {0};
    dynamic_info.sType                            = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_info.dynamicStateCount                = ARRAY_COUNT(dynamic_states);
    dynamic_info.pDynamicStates                   = dynamic_states;

    VkPipelineViewportStateCreateInfo viewport_info = {0};
    viewport_info.sType                             = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_info.viewportCount                     = 1;
    viewport_info.scissorCount                      = 1;

    // NOTE(KB): Even though meshes use CCW we use CW in the rasterizer, because we flipped the viewport Y
    VkPipelineRasterizationStateCreateInfo rasterizer_info = {0};
    rasterizer_info.sType                                  = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer_info.depthClampEnable                       = VK_FALSE;
    rasterizer_info.rasterizerDiscardEnable                = VK_FALSE;
    rasterizer_info.polygonMode                            = VK_POLYGON_MODE_FILL;
    rasterizer_info.lineWidth                              = 1.0f;
    rasterizer_info.cullMode                               = VK_CULL_MODE_BACK_BIT;
    rasterizer_info.frontFace                              = VK_FRONT_FACE_CLOCKWISE;
    rasterizer_info.depthBiasEnable                        = VK_FALSE;
    rasterizer_info.depthBiasConstantFactor                = 0.0f;
    rasterizer_info.depthBiasClamp                         = 0.0f;
    rasterizer_info.depthBiasSlopeFactor                   = 0.0f;

    VkPipelineMultisampleStateCreateInfo multisample_info = {0};
    multisample_info.sType                                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisample_info.sampleShadingEnable                  = VK_FALSE;
    multisample_info.rasterizationSamples                 = VK_SAMPLE_COUNT_1_BIT;
    multisample_info.minSampleShading                     = 1.0f;
    multisample_info.pSampleMask                          = NULL;
    multisample_info.alphaToCoverageEnable                = VK_FALSE;
    multisample_info.alphaToOneEnable                     = VK_FALSE;

    VkStencilOpState stencil_state_front = {0};
    VkStencilOpState stencil_state_back  = {0};

    VkPipelineDepthStencilStateCreateInfo depth_stencil_info = {0};
    depth_stencil_info.sType                                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth_stencil_info.depthTestEnable                       = VK_TRUE;
    depth_stencil_info.depthWriteEnable                      = VK_TRUE;
    depth_stencil_info.depthCompareOp                        = VK_COMPARE_OP_LESS;
    depth_stencil_info.depthBoundsTestEnable                 = VK_FALSE;
    depth_stencil_info.stencilTestEnable                     = VK_FALSE;
    depth_stencil_info.front                                 = stencil_state_front;
    depth_stencil_info.back                                  = stencil_state_back;
    depth_stencil_info.minDepthBounds                        = 0.0f;
    depth_stencil_info.maxDepthBounds                        = 1.0f;

    VkPipelineColorBlendAttachmentState color_blend_attachment = {0};
    color_blend_attachment.blendEnable                         = VK_FALSE;
    color_blend_attachment.srcColorBlendFactor                 = VK_BLEND_FACTOR_ONE;
    color_blend_attachment.dstColorBlendFactor                 = VK_BLEND_FACTOR_ZERO;
    color_blend_attachment.colorBlendOp                        = VK_BLEND_OP_ADD;
    color_blend_attachment.srcAlphaBlendFactor                 = VK_BLEND_FACTOR_ONE;
    color_blend_attachment.dstAlphaBlendFactor                 = VK_BLEND_FACTOR_ZERO;
    color_blend_attachment.alphaBlendOp                        = VK_BLEND_OP_ADD;
    color_blend_attachment.colorWriteMask                      = VK_COLOR_COMPONENT_R_BIT |
                                                                 VK_COLOR_COMPONENT_G_BIT |
                                                                 VK_COLOR_COMPONENT_B_BIT |
                                                                 VK_COLOR_COMPONENT_A_BIT;

    VkPipelineColorBlendStateCreateInfo color_blend_info = {0};
    color_blend_info.sType                               = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blend_info.logicOpEnable                       = VK_FALSE;
    color_blend_info.logicOp                             = VK_LOGIC_OP_COPY;
    color_blend_info.attachmentCount                     = 1;
    color_blend_info.pAttachments                        = &color_blend_attachment;
    color_blend_info.blendConstants[0]                   = 0.0f;
    color_blend_info.blendConstants[1]                   = 0.0f;
    color_blend_info.blendConstants[2]                   = 0.0f;
    color_blend_info.blendConstants[3]                   = 0.0f;

    VkPipelineRenderingCreateInfo rendering_info = {0};
    rendering_info.sType                         = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    rendering_info.viewMask                      = 0;
    rendering_info.colorAttachmentCount          = 1;
    rendering_info.pColorAttachmentFormats       = &g_renderer.swapchain_img_fmt;
    rendering_info.depthAttachmentFormat         = VK_FORMAT_D32_SFLOAT; // TODO(KB): g_renderer.depth_img_fmt
    rendering_info.stencilAttachmentFormat       = VK_FORMAT_UNDEFINED;

    VkGraphicsPipelineCreateInfo graphics_pipeline_info = {0};
    graphics_pipeline_info.sType                        = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphics_pipeline_info.pNext                        = &rendering_info;
    graphics_pipeline_info.stageCount                   = 2;
    graphics_pipeline_info.pStages                      = shader_info;
    graphics_pipeline_info.pVertexInputState            = &vertex_input_info;
    graphics_pipeline_info.pInputAssemblyState          = &input_assembly_info;
    graphics_pipeline_info.pViewportState               = &viewport_info;
    graphics_pipeline_info.pRasterizationState          = &rasterizer_info;
    graphics_pipeline_info.pMultisampleState            = &multisample_info;
    graphics_pipeline_info.pDepthStencilState           = &depth_stencil_info;
    graphics_pipeline_info.pColorBlendState             = &color_blend_info;
    graphics_pipeline_info.pDynamicState                = &dynamic_info;
    graphics_pipeline_info.layout                       = pipeline->graphics_pipeline_layout;
    graphics_pipeline_info.renderPass                   = NULL;
    graphics_pipeline_info.subpass                      = 0;
    graphics_pipeline_info.basePipelineHandle           = VK_NULL_HANDLE;
    graphics_pipeline_info.basePipelineIndex            = -1;

    VkResult create_result = vkCreateGraphicsPipelines(g_renderer.device, VK_NULL_HANDLE, 1, &graphics_pipeline_info, NULL, &pipeline->graphics_pipeline);
    EMBER_ASSERT(create_result == VK_SUCCESS);

    vkDestroyShaderModule(g_renderer.device, vert_module, NULL);
    vkDestroyShaderModule(g_renderer.device, frag_module, NULL);

    cpu_scratch_end(scratch);
}

internal VkShaderModule renderer_pipeline_create_shader_module(const u8* code, u64 code_size)
{
    VkShaderModuleCreateInfo module_info = {0};
    module_info.sType                    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    module_info.codeSize                 = code_size;
    module_info.pCode                    = (u32 *)code;

    VkShaderModule result;

    VkResult create_result = vkCreateShaderModule(g_renderer.device, &module_info, NULL, &result);
    EMBER_ASSERT(create_result == VK_SUCCESS);

    return result;
}
