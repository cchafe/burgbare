project('burgbare', ['cpp','c'],
		default_options: ['cpp_std=c++17','warning_level=2'])

if get_option('profile') == 'development'
  application_id = 'org.burgbare.burgbare.Devel'
  name_suffix = ' (Development Snapshot)'
else
  application_id = 'org.burgbare.burgbare'
  name_suffix = ''
endif

qt5 = import('qt5')
cmake = import('cmake')

compiler = meson.get_compiler('cpp')

defines = ['-DWAIRTOHUB']
c_defines = []
incdirs = []

src = [	'burgalgorithm.cpp',
	'main.cpp',
	'wavfileio.cpp']

ui_h = []
qres = []
deps = dependency('qt5', modules: ['Core', 'Gui', 'Network', 'Widgets'], include_type: 'system')
#deps += dependency('stk', include_type: 'system')
cc = meson.get_compiler('c')
dep += declare_dependency(
  dependencies : cc.find_library('stk', dirs : [meson.current_source_dir()]),
  include_directories : include_directories('.'),
)
burgbare = executable('burgbare', src, include_directories: incdirs, c_args: c_defines, dependencies: deps, cpp_args: defines )

