
set(graphics_root ${CMAKE_CURRENT_SOURCE_DIR})
set(graphics_opengl ${graphics_root}/platforms/opengl)
set(graphics_vulkan ${graphics_root}/platforms/vulkan)
set(graphics_directX ${graphics_Root}/platforms/directx)

set(shader_sources
    
)

# abstract graphics sources
set(graphics_sources
    ${graphics_root}/buffer.h
    #${graphics_root}/buffer.cpp
    ${graphics_root}/camera.h
    ${graphics_root}/camera.cpp
    ${graphics_root}/camera_controller.hpp
    ${graphics_root}/camera_controller.cpp
    ${graphics_root}/graphics_context.h
    ${graphics_root}/graphics_profiler.h
    ${graphics_root}/graphics_profiler.cpp
    ${graphics_root}/renderable_types.h
    # ${graphics_root}/renderer_2d.h
    # ${graphics_root}/renderer_2d.cpp
    ${graphics_root}/renderer_api.h
    ${graphics_root}/renderer_api.cpp
    ${graphics_root}/render_types.h
    ${graphics_root}/render_commands.h
    ${graphics_root}/render_commands.cpp
    ${graphics_root}/shader.h
    ${graphics_root}/shader.cpp
    ${graphics_root}/shader_cache.h
    ${graphics_root}/shader_cache.cpp
    ${graphics_root}/shader_util.h
    ${graphics_root}/shader_util.cpp
    ${graphics_root}/technique.h
    ${graphics_root}/technique.cpp
    ${graphics_root}/screen_quad.h    
    ${graphics_root}/screen_quad.cpp

    ${graphics_root}/texture.h
    ${graphics_root}/texture.cpp
    ${graphics_root}/framebuffer.h    
    #${graphics_root}/framebuffer.cpp   
    ${graphics_root}/mesh.h    
    ${graphics_root}/mesh.cpp   
    ${graphics_root}/material.h    
    ${graphics_root}/material.cpp
    ${graphics_root}/vertex.h
    ${graphics_root}/vertex.cpp  
    ${graphics_root}/swapchain.h   
    ${graphics_root}/swapchain.cpp   

    ${graphics_root}/graphics_events.hpp
    ${graphics_root}/render_layer.h
    ${graphics_root}/render_layer.cpp


)

set(directx_sources
    #Directx
	${graphics_root}/platform/directx/d3dx12.h
    ${graphics_root}/platform/directx/directx_buffer.h
    ${graphics_root}/platform/directx/directx_buffer.cpp
    ${graphics_root}/platform/directx/directx_copy_context.h
    ${graphics_root}/platform/directx/directx_copy_context.cpp
    ${graphics_root}/platform/directx/directx_command_buffer.h
    ${graphics_root}/platform/directx/directx_command_buffer.cpp
    ${graphics_root}/platform/directx/directx_fence.h
    ${graphics_root}/platform/directx/directx_fence.cpp
    ${graphics_root}/platform/directx/directx_framebuffer.h
    ${graphics_root}/platform/directx/directx_framebuffer.cpp
    ${graphics_root}/platform/directx/directx_frame_heap.h
    ${graphics_root}/platform/directx/directx_frame_heap.cpp
    ${graphics_root}/platform/directx/directx_graphics_context.h
    ${graphics_root}/platform/directx/directx_graphics_context.cpp
    ${graphics_root}/platform/directx/directx_graphics_profiler.h
    ${graphics_root}/platform/directx/directx_graphics_profiler.cpp
    ${graphics_root}/platform/directx/directx_heap.h
    ${graphics_root}/platform/directx/directx_heap.cpp
    ${graphics_root}/platform/directx/directx_indirect_signature.h
    ${graphics_root}/platform/directx/directx_indirect_signature.cpp
    ${graphics_root}/platform/directx/directx_pipeline.h
    ${graphics_root}/platform/directx/directx_pipeline.cpp
    ${graphics_root}/platform/directx/directx_pipeline_layout.h
    ${graphics_root}/platform/directx/directx_pipeline_layout.cpp
    ${graphics_root}/platform/directx/directx_render_system.h
    ${graphics_root}/platform/directx/directx_render_system.cpp
    ${graphics_root}/platform/directx/directx_renderer_api.h
    ${graphics_root}/platform/directx/directx_renderer_api.cpp
    ${graphics_root}/platform/directx/directx_shader.h
    ${graphics_root}/platform/directx/directx_shader.cpp
    ${graphics_root}/platform/directx/directx_swapchain.h
    ${graphics_root}/platform/directx/directx_swapchain.cpp
    ${graphics_root}/platform/directx/directx_system_manager.h
    ${graphics_root}/platform/directx/directx_system_manager.cpp
    ${graphics_root}/platform/directx/directx_technique.h
    ${graphics_root}/platform/directx/directx_technique.cpp
    ${graphics_root}/platform/directx/directx_texture.h
    ${graphics_root}/platform/directx/directx_texture.cpp


    ${graphics_root}/primitives/cube.h
    ${graphics_root}/primitives/cube.cpp
    ${graphics_root}/primitives/plane.h
    ${graphics_root}/primitives/plane.cpp
    ${graphics_root}/primitives/sphere.h
    ${graphics_root}/primitives/sphere.cpp


    ${graphics_root}/systems/bloom_system.h
    ${graphics_root}/systems/bloom_system.cpp
    ${graphics_root}/systems/deferred_lighting_system.h
    ${graphics_root}/systems/deferred_lighting_system.cpp
    ${graphics_root}/systems/depth_prepass_system.h 
    ${graphics_root}/systems/depth_prepass_system.cpp
    ${graphics_root}/systems/dof_system.h 
    ${graphics_root}/systems/dof_system.cpp 
    ${graphics_root}/systems/dlss_system.h 
    ${graphics_root}/systems/dlss_system.cpp
    ${graphics_root}/systems/ffx_fsr2_system.h 
    ${graphics_root}/systems/ffx_fsr2_system.cpp
    ${graphics_root}/systems/forward_plus_accumulation_system.h 
    ${graphics_root}/systems/forward_plus_accumulation_system.cpp
    ${graphics_root}/systems/forward_system.h 
    ${graphics_root}/systems/forward_system.cpp
    ${graphics_root}/systems/frustum_cull_compute_system.h 
    ${graphics_root}/systems/frustum_cull_compute_system.cpp
    ${graphics_root}/systems/fxaa_system.h 
    ${graphics_root}/systems/fxaa_system.cpp
    ${graphics_root}/systems/light_culling_compute_system.h 
    ${graphics_root}/systems/light_culling_compute_system.cpp
    ${graphics_root}/systems/shadow_system.h 
    ${graphics_root}/systems/shadow_system.cpp 
    ${graphics_root}/systems/ssao_system.h 
    ${graphics_root}/systems/ssao_system.cpp 
    ${graphics_root}/systems/taa_system.h 
    ${graphics_root}/systems/taa_system.cpp 


)

set(opengl_sources
    ${graphics_root}/platform/opengl/opengl_buffer.h
    ${graphics_root}/platform/opengl/opengl_buffer.cpp
    ${graphics_root}/platform/opengl/opengl_uniform_buffer.h
    ${graphics_root}/platform/opengl/opengl_uniform_buffer.cpp
    ${graphics_root}/platform/opengl/opengl_graphics_context.h
    ${graphics_root}/platform/opengl/opengl_graphics_context.cpp
    ${graphics_root}/platform/opengl/opengl_renderer_api.h
    ${graphics_root}/platform/opengl/opengl_renderer_api.cpp
    ${graphics_root}/platform/opengl/opengl_shader.h
    ${graphics_root}/platform/opengl/opengl_shader.cpp
    ${graphics_root}/platform/opengl/opengl_texture.h
    ${graphics_root}/platform/opengl/opengl_texture.cpp
    ${graphics_root}/platform/opengl/opengl_framebuffer.h
    ${graphics_root}/platform/opengl/opengl_framebuffer.cpp
)

set(vulkan_sources
	${graphics_root}/platform/vulkan/vulkan_buffer.h
	${graphics_root}/platform/vulkan/vulkan_buffer.cpp
	${graphics_root}/platform/vulkan/vulkan_device.h
    ${graphics_root}/platform/vulkan/vulkan_device.cpp
	${graphics_root}/platform/vulkan/vulkan_framebuffer.h
	${graphics_root}/platform/vulkan/vulkan_framebuffer.cpp
	${graphics_root}/platform/vulkan/vulkan_graphics_pipeline.h
	${graphics_root}/platform/vulkan/vulkan_graphics_pipeline.cpp
	${graphics_root}/platform/vulkan/vulkan_renderer_api.h
	${graphics_root}/platform/vulkan/vulkan_renderer_api.cpp
	${graphics_root}/platform/vulkan/vulkan_swapchain.h
	${graphics_root}/platform/vulkan/vulkan_swapchain.cpp
	${graphics_root}/platform/vulkan/vulkan_texture.h
	${graphics_root}/platform/vulkan/vulkan_texture.cpp
	${graphics_root}/platform/vulkan/vulkan_util.h
	${graphics_root}/platform/vulkan/vulkan_util.cpp
)



message(STATUS "\nGraphics platform found: ${GRAPHICS_PLATFORM}")

if(GRAPHICS_PLATFORM STREQUAL opengl)
    add_definitions(
        -DOPENGL
    )
    list(APPEND platform_sources ${opengl_sources})

elseif(GRAPHICS_PLATFORM STREQUAL vulkan)
    add_definitions(
        -DVULKAN
		-DGLFW_NO_API
    )
    list(APPEND platform_sources ${vulkan_sources})

elseif(GRAPHICS_PLATFORM STREQUAL directx)
    add_definitions(
        -DDIRECTX
		-DGLFW_NO_API
    )
    list(APPEND platform_sources ${directx_sources})

 endif()


 message(STATUS "\nInitialising graphics platform with sources\n")
 foreach(item IN LISTS platform_sources)
     message(STATUS  "\t-${item}")
 endforeach()
 
 # display sources being added
 message(STATUS "\nInitialising graphics library with sources:\n")
 foreach(item IN LISTS graphics_sources)
     message(STATUS  "\t-${item}")
 endforeach()
 
# add as static library
add_library(graphics STATIC 
    ${graphics_sources}
    ${platform_sources}
)

set(destinations 
    ${CMAKE_BINARY_DIR}/sandbox
    ${CMAKE_BINARY_DIR}/game
    ${CMAKE_BINARY_DIR}/editor
)

message(STATUS ${GRAPHICS_PLATFORM})

source_group(TREE ${graphics_root} FILES ${graphics_sources})
source_group(TREE ${graphics_root} FILES ${platform_sources})