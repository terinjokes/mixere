project('audiere', 'cpp', 'c', license: 'LGPL-2.1-or-later')

cxx = meson.get_compiler('cpp')

flac = dependency('flac')
ogg = dependency('ogg')
vorbisfile = dependency('vorbisfile')

winmm = cxx.find_library('winmm')
rpcrt4 = cxx.find_library('rpcrt4')

windows = import('windows')
audiere_rc = windows.compile_resources('src/audiere.rc')

subdir('src/mpaudec')

audiere_lib = library(
    'audiere',
    [
        'src/basic_source.cpp',
        'src/cd_win32.cpp',
        'src/debug.cpp',
        'src/device.cpp',
        'src/device_ds.cpp',
        'src/device_ds_buffer.cpp',
        'src/device_ds_stream.cpp',
        'src/device_mixer.cpp',
        'src/device_mm.cpp',
        'src/device_null.cpp',
        'src/dumb_resample.cpp',
        'src/dxguid.cpp',
        'src/file_ansi.cpp',
        'src/input.cpp',
        'src/input_aiff.cpp',
        'src/input_flac.cpp',
        'src/input_mp3.cpp',
        'src/input_mpg123.cpp',
        'src/input_ogg.cpp',
        'src/input_wav.cpp',
        'src/loop_point_source.cpp',
        'src/memory_file.cpp',
        'src/midi_mci.cpp',
        'src/noise.cpp',
        'src/resampler.cpp',
        'src/sample_buffer.cpp',
        'src/sound.cpp',
        'src/sound_effect.cpp',
        'src/square_wave.cpp',
        'src/threads_win32.cpp',
        'src/timer_win32.cpp',
        'src/tone.cpp',
        'src/utility.cpp',
        'src/version.cpp',
    ],
    audiere_rc,
    cpp_args: [
        '-DAUDIERE_EXPORTS',
        '-DNO_DUMB',
        '-DNO_SPEEX',
        '-DHAVE_DSOUND',
        '-DHAVE_WINMM', '/source-charset:windows-1250',
    ],
    dependencies: [flac, ogg, vorbisfile, winmm, rpcrt4],
    link_with: [libmpaudec],
    install: true,
    include_directories: 'src',
)

audiere_dep = declare_dependency(
    include_directories: 'src',
    link_with: audiere_lib,
)

install_headers('src/audiere.h')
