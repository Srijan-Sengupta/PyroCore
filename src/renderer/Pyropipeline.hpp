//
// Created by srijan on 2/12/25.
//

#ifndef PYROPIPELINE_HPP
#define PYROPIPELINE_HPP
#include "../core/VulkanDevice.hpp"

namespace pyro {

    class Pyropipeline {
    public:
        Pyropipeline(VulkanDevice *device);
        ~Pyropipeline();
        std::vector<VkDynamicState> get_dynamic_states() const { return dynamic_states; }
        VkPipelineLayout get_pipeline_layout() const { return pipeline_layout; }
        VulkanDevice *get_device() const { return device; }
        VkRenderPass get_render_pass() const { return render_pass; }
        VkPipeline get_pipeline() const { return pipeline; }
        std::vector<VkFramebuffer> get_swap_chain_framebuffers() const { return swap_chain_framebuffers; }

    private:
        const std::vector<VkDynamicState> dynamic_states = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };
        VkPipelineLayout pipeline_layout;
        VulkanDevice *device;
        VkRenderPass render_pass;
        VkPipeline pipeline;
        std::vector<VkFramebuffer> swap_chain_framebuffers;
    };

} // namespace pyro

#endif // PYROPIPELINE_HPP
