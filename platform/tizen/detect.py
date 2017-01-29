
import os
import sys
import platform


def is_active():
    return True


def get_name():
    return "Tizen"


def can_build():
    
    if (os.getenv("TIZEN_ROOT")):
        return True
    
    return False


def get_opts():

    return [
        ('use_static_cpp', 'link stdc++ statically', 'no'),
        ('debug_release', 'Add debug symbols to release version', 'no'),
	('tizen_arch', 'select compiler architecture: (armv7/x86)', "armv7"),
    ]


def get_flags():

    return [
        ('tools', 'no'),
        ('builtin_freetype', 'yes'),
        ('builtin_libpng', 'yes'),
        ('builtin_openssl', 'yes'),
        ('builtin_zlib', 'yes'),
    ]


def configure(env):

    tizen_root = os.getenv("TIZEN_ROOT")

    if env['tizen_arch'] not in ['armv7', 'x86']:
	env['tizen_arch'] = 'armv7'
    arch = env['tizen_arch']
    if (arch == 'armv7'):
	env.extra_suffix = '.armv7' + env.extra_suffix
	march = 'armv7-a'
	target = 'arm'
    elif (arch == 'x86'):
	env.extra_suffix = '.x86' + env.extra_suffix
	march = 'i386'
	target = 'i386'
    abi_name = target+"-linux-gnueabi-"
    tools_path = tizen_root + "/tools/"+abi_name+"gcc-4.9"
    env["CC"] = tizen_root + "/tools/llvm-3.6/bin/clang"
    env["CXX"] = tizen_root + "/tools/llvm-3.6/bin/clang++"
    env["LD"] = tizen_root + "/tools/llvm-3.6/bin/clang++"
    env["AR"] = tools_path + "/bin/"+abi_name+"ar"
    env["RANLIB"] = tools_path + "/bin/"+abi_name+"ranlib"
    env["AS"] = tools_path + "/bin/"+abi_name+"as"
    if (env["target"] == "release"):

        if (env["debug_release"] == "yes"):
            env.Append(CCFLAGS=['-g2'])
        else:
            env.Append(CCFLAGS=['-O3', '-ffast-math'])

    elif (env["target"] == "release_debug"):

        env.Append(CCFLAGS=['-O2', '-ffast-math', '-DDEBUG_ENABLED'])
        if (env["debug_release"] == "yes"):
            env.Append(CCFLAGS=['-g2'])

    elif (env["target"] == "debug"):
        env.Append(CCFLAGS=['-g2', '-Wall', '-DDEBUG_ENABLED', '-DDEBUG_MEMORY_ENABLED'])
    if (env["tizen_arch"] == 'x86'):
	sysroot = tizen_root + '/platforms/tizen-2.4/mobile/rootstraps/mobile-2.4-emulator.core/'
    else:
	sysroot = tizen_root + '/platforms/tizen-2.4/mobile/rootstraps/mobile-2.4-device.core/'
    env.Append(CPPFLAGS=["-isystem", sysroot, '-I' + sysroot + 'usr/lib/dbus-1.0/include/'])
    inc_libs = ['appfw', 'dlog', 'system', 'elementary-1', 'efl-1', 'efl-extension', 'eina-1', 'eina-1/eina', 'eet-1', 'evas-1', 'eo-1', 'ecore-1', 'ecore-evas-1', 'ecore-file-1', 'ecore-input-1', 'ecore-imf-1', 'ecore-con-1', 'edje-1', 'eldbus-1', 'efreet-1', 'ethumb-client-1', 'ethumb-1', 'e_dbus-1', 'dbus-1.0']
    for lib in inc_libs:
	env.Append(CPPFLAGS=['-I' + sysroot + 'usr/include/' + lib])
    env.Append(CPPPATH=['#platform/tizen'])

    env.Append(LINKFLAGS=['-shared'])
    env.Append(LINKPATH=[sysroot + 'usr/lib'])

    env.Append(CPPFLAGS=['--sysroot="'+sysroot+'"'])
    env.Append(LINKFLAGS=['--sysroot="'+sysroot+'"'])

    env.Append(CPPFLAGS=['-gcc-toolchain', tools_path, '-fPIE', '-Wno-gnu'])
    env.Append(CPPFLAGS=['-target', target+'-tizen-linux-gnueabi'])
    env.Append(LINKFLAGS=['--gcc-toolchain="' + tizen_root + '/tools/'+target+'-linux-gnueabi-gcc-4.9"'])
    env.Append(LINKFLAGS=['-target', target+'-tizen-linux-gnueabi', '-ccc-gcc-name', target+'-linux-gnueabi-g++'])
    env.Append(CPPFLAGS=['-march='+march])
    if (arch == 'armv7'):
	env.Append(CPPFLAGS=['-mfloat-abi=softfp', '-mfpu=vfpv3-d16','-mtune=cortex-a8'])
    env.Append(CPPFLAGS=['-DTIZEN_ENABLED']) #'-DGLES2_ENABLED',
    env.Append(LIBS=['pthread', 'z'])#'GL',
    #if (platform.system() == "Linux"):
    #    env.Append(LIBS='dl')
    #env.Append(CPPFLAGS=['-DMPC_FIXED_POINT'])

# host compiler is default..

#    if (is64 and env["bits"] == "32"):
#        env.Append(CPPFLAGS=['-m32'])
#        env.Append(LINKFLAGS=['-m32', '-L/usr/lib/i386-linux-gnu'])
#    elif (not is64 and env["bits"] == "64"):
#        env.Append(CPPFLAGS=['-m64'])
#        env.Append(LINKFLAGS=['-m64', '-L/usr/lib/i686-linux-gnu'])

    import methods

    #env.Append(BUILDERS={'GLSL120': env.Builder(action=methods.build_legacygl_headers, suffix='glsl.h', src_suffix='.glsl')})
    #env.Append(BUILDERS={'GLSL': env.Builder(action=methods.build_glsl_headers, suffix='glsl.h', src_suffix='.glsl')})
    #env.Append(BUILDERS={'GLSL120GLES': env.Builder(action=methods.build_gles2_headers, suffix='glsl.h', src_suffix='.glsl')})
    #env.Append( BUILDERS = { 'HLSL9' : env.Builder(action = methods.build_hlsl_dx9_headers, suffix = 'hlsl.h',src_suffix = '.hlsl') } )

    if (env["use_static_cpp"] == "yes"):
        env.Append(LINKFLAGS=['-static-libstdc++'])

    #list_of_x86 = ['x86_64', 'x86', 'i386', 'i586']
    #if any(platform.machine() in s for s in list_of_x86):
     #   env["x86_libtheora_opt_gcc"] = True
