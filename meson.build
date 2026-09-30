project('khdays', ['c', 'cpp'],
    version: '1.0',
    meson_version: '>=1.7.0',
    default_options : [
        'buildtype=plain',
        'warning_level=0'
    ]
)

fs = import('fs')
build_target = get_option('build_target')

############################################################
###                    INCLUDE PATHS                     ###
############################################################
public_includes = include_directories('include')


############################################################
###                    COMPILER FLAGS                    ###
############################################################
shared_args = []
c_args = []
cpp_args = []
if (build_target == 'arm')
    c_args = shared_args + ['-lang', 'c99']
    cpp_args = shared_args + ['-RTTI', 'off']
else
    c_args = shared_args
    cpp_args = shared_args
endif

add_global_arguments(c_args,
    language: 'c',
    native: (build_target == 'linux' or build_target == 'win64')
)

add_global_arguments(cpp_args,
    language: 'cpp',
    native: (build_target == 'linux' or build_target == 'win64')
)

khdays_args = [
]

if(build_target != 'arm')
    khdays_args += [
        '-std=c99'
    ]
endif

khdays_link_args = []

############################################################
###                     DEPENDENCIES                     ###
############################################################
libntr_prj = subproject('libntr',
    default_options: [
        'card_game_code=YKGP',
        'card_maker_code=GD'
    ])

libntr_dep = dependency('NitroSDK')
libntrsystem_dep = dependency('NitroSystem')

############################################################
###                  INTERNAL LIBRARIES                  ###
############################################################
subdir('libs')

############################################################
###                     ARM9 BINARY                      ###
############################################################
subdir('src')

khdays_dependencies = [
    libntr_dep,
    libntrsystem_dep
]

if (build_target == 'win64')
ws2_32_lib = meson.get_compiler('c').find_library('ws2_32')

ws2_32_dep = declare_dependency(dependencies: [ws2_32_lib])

khdays_dependencies += ws2_32_dep
endif

khdays = executable('khdays',
    sources: [
        khdays_srcs,
    ],
    c_args: [
        khdays_args,
    ],
    include_directories: [
        public_includes,
    ],
    dependencies: khdays_dependencies,
    link_args: [
        khdays_link_args,
    ],
    native: (build_target == 'linux' or build_target == 'win64')
)