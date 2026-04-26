#ifndef RENDERER_PIPELINE_H
#define RENDERER_PIPELINE_H

typedef struct renderer_pipeline_t renderer_pipeline_t;
struct renderer_pipeline_t
{
    VkDescriptorSetLayout descriptor_set_layout;
    VkDescriptorSet       descriptor_sets[RENDERER_FRAMES_IN_FLIGHT];
    VkPipelineLayout      graphics_pipeline_layout;
    VkPipeline            graphics_pipeline;
};

internal void renderer_pipeline_init(renderer_pipeline_t* state);
internal void renderer_pipeline_destroy(renderer_pipeline_t* state);

internal void           renderer_pipeline_create_descriptor_set_layout(renderer_pipeline_t* pipeline);
internal void           renderer_pipeline_create_descriptor_sets(renderer_pipeline_t* pipeline);
internal void           renderer_pipeline_create_graphics_pipeline_layout(renderer_pipeline_t* pipeline);
internal void           renderer_pipeline_create_graphics_pipeline(renderer_pipeline_t* pipeline);
internal VkShaderModule renderer_pipeline_create_shader_module(const u8* code, u64 code_size);

#endif // RENDERER_PIPELINE_H
