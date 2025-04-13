project('PyroCore', 'cpp',
        default_options : ['cpp_std=c++20', 'default_library=static', 'b_staticpic=true']
)
subdir('src')
subdir('assets')

cmake = import('cmake')

deps = [dependency('vulkan', native : false)]

glm_proj = cmake.subproject('glm')
glm = glm_proj.dependency('glm')
deps += glm

cmake_subproj_opts = cmake.subproject_options()

if get_option('sdl')
    if (host_machine.system() == 'linux')
        cmake_subproj_opts.add_cmake_defines({ 'SDL_STATIC' : 'ON' })
        cmake_subproj_opts.add_cmake_defines({ 'SDL_SHARED' : 'OFF' })
        sdl_proj = cmake.subproject('SDL3-3.2.10', options : cmake_subproj_opts)
        sdl_dep = sdl_proj.dependency('SDL3-static')
    else
        sdl_proj = cmake.subproject('SDL3-3.2.10', options : cmake_subproj_opts)
        sdl_dep = sdl_proj.dependency('SDL3-shared')
    endif
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