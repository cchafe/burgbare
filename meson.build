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
	'AudioInterface.cpp',
	'Regulator.cpp',
	'main.cpp',
	'wavfileio.cpp']

ui_h = []
qres = []
deps = dependency('qt5', modules: ['Core', 'Gui', 'Network', 'Widgets'], include_type: 'system')
cc = meson.get_compiler('c')
lib_stk = cc.find_library('stk',
               dirs : ['/usr/lib64/stk'])
inc_stk = include_directories('/usr/include/stk')
incdirs += [inc_stk]
burgbare = executable('burgbare', src, include_directories: incdirs, c_args: c_defines, dependencies: [deps,lib_stk], cpp_args: defines )

