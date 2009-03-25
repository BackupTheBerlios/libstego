import os
import glob

vars = Variables()
vars.Add(BoolVariable('debug', 'build with debug symbols', 0))
vars.Add(BoolVariable('withoutjpeg', 'build without jpeg support', 0))
vars.Add(BoolVariable('withoutpng', 'build without png support', 0))
vars.Add(BoolVariable('withoutwav', 'build without wav support', 0))
vars.Add(BoolVariable('withoutgif', 'build without gif support', 0))
vars.Add(BoolVariable('withoutsvg', 'build without svg support', 0))
#vars.Add(BoolVariable('libstego', 'build libstego library', True))
#vars.Add(BoolVariable('libstegofile', 'build libstegofile library', True))
vars.Add(PathVariable('PREFIX', 'directory to install under', '/usr', 
                      PathVariable.PathIsDir))
vars.Add(EnumVariable('PACKAGETYPE', 'type of package to build', 'tarbz2',
         allowed_values=['tarbz2', 'rpm']))



env = Environment(variables=vars, tools=['default', 'packaging'],
                  CPPPATH=['include', '#/src/include'],
                  CPPDEFINES={'WITH_JPEG' : '${not withoutjpeg}',
                              'WITH_WAV' : '${not withoutwav}',
                              'WITH_GIF' : '${not withoutgif}',
                              'WITH_PNG' : '${not withoutpng}',
                              'WITH_SVG' : '${not withoutsvg}'},
                  CCFLAGS='-std=c99')

Help(vars.GenerateHelpText(env))


env.Alias('libstego', 'src/libstego/')
env.Alias('libstegofile', 'src/libstegofile')
env.Alias('bindings', 'python_bindings')

env.Default('libstego', 'libstegofile')

#env.Alias('tarball', 

idir_prefix = '$PREFIX'
idir_lib    = '$PREFIX/lib'
idir_bin    = '$PREFIX/bin'
idir_inc    = '$PREFIX/include/libstego'
idir_data   = '$PREFIX/share'

lib_files = ['src/libstego/libstego.so', 'src/libstegofile/libstegofile.so']
inc_files = glob.glob('src/include/**/*') +                                \
            glob.glob('src/libstego/include/**/*') +                       \
            glob.glob('src/libstegofile/include/**/*')                     \

install_libs = env.Install(idir_lib, lib_files)
install_incs = env.Install(idir_inc, inc_files)

env.Alias('install', [install_libs, install_incs])

Export('env idir_prefix idir_lib idir_bin idir_inc idir_data')


SConscript(['src/libstegofile/SConscript'])

SConscript(['src/libstego/SConscript'])

if 'bindings' in BUILD_TARGETS:
    SConscript(['python_bindings/SConscript'])



#p = env.Package( NAME='libstego',
#                 VERSION='0.1.0',
#                 PACKAGEVERSION=0,
#                 PACKAGETYPE="$PACKAGETYPE",
#                 LICENSE='gpl',
#                 SUMMARY='a set of steganographic libraries',
#                 DESCRIPTION='blah',
#                 X_RPM_GROUP='Library/security',
#                 SOURCE_URL='http://here'
#                 )


#env.Alias('pkg', p)



