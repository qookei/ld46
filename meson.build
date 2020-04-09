project('ldeng', 'cpp', default_options:['cpp_std=c++17', 'warning_level=3'])

sources = files(
	'src/main.cpp',
	'src/shader_prog.cpp',
	'src/utils/mapped_file.cpp',
	'src/utils/errors.cpp',
	'src/mesh.cpp',
	'src/chunk_view.cpp',
	'src/texture.cpp',
	'src/window.cpp')

includes = include_directories('src')

sdl2_dep = dependency('SDL2', method: 'pkg-config')
epoxy_dep = dependency('epoxy')
threads_dep = dependency('threads')

glm_inc = include_directories('subprojects/glm/')

executable('ldeng', sources,
	include_directories: [includes, glm_inc],
	dependencies: [sdl2_dep, epoxy_dep, threads_dep])
