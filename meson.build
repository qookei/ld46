project('ldeng', 'cpp', default_options:['cpp_std=c++17', 'warning_level=3'])

sources = files(
	'src/main.cpp',
	'src/shader_prog.cpp',
	'src/utils/mapped_file.cpp',
	'src/utils/errors.cpp',
	'src/mesh.cpp',
	'src/chunk_view.cpp',
	'src/imgui_drawer.cpp',
	'src/texture.cpp',
	'src/window.cpp')

includes = include_directories('src')

sdl2_dep = dependency('SDL2', method: 'pkg-config')
epoxy_dep = dependency('epoxy')
threads_dep = dependency('threads')

glm_inc = include_directories('subprojects/glm/')

imgui_lib = static_library('imgui',
		files(
			'subprojects/imgui/imgui_demo.cpp',
			'subprojects/imgui/imgui_draw.cpp',
			'subprojects/imgui/imgui_widgets.cpp',
			'subprojects/imgui/imgui.cpp'
		))

imgui_inc = include_directories('subprojects/imgui')

imgui_dep = declare_dependency(link_with: imgui_lib, include_directories: imgui_inc)

executable('ldeng', sources,
	include_directories: [includes, glm_inc],
	dependencies: [sdl2_dep, epoxy_dep, threads_dep, imgui_dep])
