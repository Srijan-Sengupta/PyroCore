shaders_src = ['basic.frag','basic.vert']
shaders = files(shaders_src)
##========== GLSL SHADERS COMPILE =======##
vulkan_glslc = find_program('glslc', required: true)
spv_targets = []
foreach shader: shaders_src
    outfile = shader + '.spv'
    spv_targets += custom_target(shader + '_spv',
                  output: outfile,
                    input: shader,
    command: [vulkan_glslc, '@INPUT@', '-o', '@OUTPUT@'],
    install: true,
    install_dir: 'PyroCore'/'assets/shaders'
    )
    message('Built: ' + shader)
endforeach
##=======================================##