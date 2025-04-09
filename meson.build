project('PyroCore', 'cpp',
        default_options : ['cpp_std=c++20']
)
subdir('src')
subdir('assets')

deps = [dependency('vulkan'), dependency('glm')]
if get_option('sdl')
    sdl_dep = dependency('sdl3', required : true)
    deps += sdl_dep
    add_project_arguments('-DUSE_SDL', language : 'cpp')
endif

if get_option('pyro_debug')
    add_project_arguments('-DPYRO_DEBUG', language : 'cpp')
endif

if get_option('logging_enabled')
    add_project_arguments('-DENABLE_LOGGING', language : 'cpp')
endif

executable('PyroCore',
           src + spv_targets,
           dependencies : deps,
           install : true,
           install_dir : 'PyroCore'
)