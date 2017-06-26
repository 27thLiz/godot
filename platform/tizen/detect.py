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
	('builtin_freetype', 'no'),
        ('builtin_libpng', 'yes'),
        ('builtin_openssl', 'yes'),
	('builtin_zlib', 'no'),
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
    env["CC"] = tizen_root + "/tools/llvm-3.7/bin/clang"
    env["CXX"] = tizen_root + "/tools/llvm-3.7/bin/clang++"
    env["LD"] = tizen_root + "/tools/llvm-3.7/bin/clang++"
    env["AR"] = tools_path + "/bin/"+abi_name+"ar"
    env["RANLIB"] = tools_path + "/bin/"+abi_name+"ranlib"
    env["AS"] = tools_path + "/bin/"+abi_name+"as"
    env.Append(LINKFLAGS=['-pie', '-rdynamic'])
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
    #inc_libs = ['appfw', 'dlog', 'system', 'elementary-1', 'efl-1', 'efl-extension', 'eina-1', 'eina-1/eina', 'eet-1', 'evas-1', 'eo-1', 'ecore-1', 'ecore-evas-1', 'ecore-file-1', 'ecore-input-1', 'ecore-imf-1', 'ecore-con-1', 'edje-1', 'eldbus-1', 'efreet-1', 'ethumb-client-1', 'ethumb-1', 'e_dbus-1', 'dbus-1.0']
    #for lib in inc_libs:
	#env.Append(CPPFLAGS=['-I' + sysroot + 'usr/include/' + lib])
    env.Append(CPPPATH=['#platform/tizen'])

    #env.Append(LINKFLAGS=['-shared'])
    #env.Append(LINKPATH=[sysroot + 'usr/lib'])

    env.Append(CPPFLAGS=['--sysroot="'+sysroot+'"'])
    env.Append(LINKFLAGS=['--sysroot="'+sysroot+'"'])

    #env.Append(CPPFLAGS=['-gcc-toolchain', tools_path, '-fPIE', '-Wno-gnu'])
    #env.Append(CPPFLAGS=['-target', target+'-tizen-linux-gnueabi'])
    #env.Append(LINKFLAGS=['--gcc-toolchain="' + tizen_root + '/tools/'+target+'-linux-gnueabi-gcc-4.9"'])
    #env.Append(LINKFLAGS=['-target', target+'-tizen-linux-gnueabi', '-ccc-gcc-name', target+'-linux-gnueabi-g++'])
    #env.Append(CPPFLAGS=['-march='+march])
    #if (arch == 'armv7'):
	#env.Append(CPPFLAGS=['-mfloat-abi=softfp', '-mfpu=vfpv3-d16','-mtune=cortex-a8'])

    env.Append(CPPFLAGS=['--sysroot='+sysroot,'-target', 'arm-tizen-linux-gnueabi', '-gcc-toolchain', tizen_root+'/tools/arm-linux-gnueabi-gcc-4.9/', '-ccc-gcc-name', 'arm-linux-gnueabi'])
    cflags = ('-O0 -g3 -Wall -c -fmessage-length=0 -march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3-d16 -mtune=cortex-a8 -Wno-gnu -fPIE  -mthumb -I"'+sysroot+'/usr/include/libxml2" -I"'+sysroot+'/usr/include" -I"'+sysroot+'/usr/include/appcore-agent" -I"'+sysroot+'/usr/include/appfw" -I"'+sysroot+'/usr/include/attach-panel" -I"'+sysroot+'/usr/include/badge" -I"'+sysroot+'/usr/include/base" -I"'+sysroot+'/usr/include/cairo" -I"'+sysroot+'/usr/include/calendar-service2" -I"'+sysroot+'/usr/include/ckm" -I"'+sysroot+'/usr/include/contacts-svc" -I"'+sysroot+'/usr/include/content" -I"'+sysroot+'/usr/include/context-service" -I"'+sysroot+'/usr/include/dali" -I"'+sysroot+'/usr/include/dali-toolkit" -I"'+sysroot+'/usr/include/dbus-1.0" -I"'+sysroot+'/usr/include/device" -I"'+sysroot+'/usr/include/dlog" -I"'+sysroot+'/usr/include/ecore-1" -I"'+sysroot+'/usr/include/ecore-buffer-1" -I"'+sysroot+'/usr/include/ecore-con-1" -I"'+sysroot+'/usr/include/ecore-evas-1" -I"'+sysroot+'/usr/include/ecore-file-1" -I"'+sysroot+'/usr/include/ecore-imf-1" -I"'+sysroot+'/usr/include/ecore-imf-evas-1" -I"'+sysroot+'/usr/include/ecore-input-1" -I"'+sysroot+'/usr/include/ecore-input-evas-1" -I"'+sysroot+'/usr/include/ecore-ipc-1" -I"'+sysroot+'/usr/include/ecore-x-1" -I"'+sysroot+'/usr/include/e_dbus-1" -I"'+sysroot+'/usr/include/edje-1" -I"'+sysroot+'/usr/include/eet-1" -I"'+sysroot+'/usr/include/efl-1" -I"'+sysroot+'/usr/include/efl-extension" -I"'+sysroot+'/usr/include/efreet-1" -I"'+sysroot+'/usr/include/eina-1" -I"'+sysroot+'/usr/include/eina-1/eina" -I"'+sysroot+'/usr/include/eio-1" -I"'+sysroot+'/usr/include/eldbus-1" -I"'+sysroot+'/usr/include/elementary-1" -I"'+sysroot+'/usr/include/embryo-1" -I"'+sysroot+'/usr/include/eo-1" -I"'+sysroot+'/usr/include/eom" -I"'+sysroot+'/usr/include/ethumb-1" -I"'+sysroot+'/usr/include/ethumb-client-1" -I"'+sysroot+'/usr/include/evas-1" -I"'+sysroot+'/usr/include/ewebkit2-0" -I"'+sysroot+'/usr/include/feedback" -I"'+sysroot+'/usr/include/fontconfig" -I"'+sysroot+'/usr/include/freetype2" -I"'+sysroot+'/usr/include/geofence" -I"'+sysroot+'/usr/include/gio-unix-2.0" -I"'+sysroot+'/usr/include/glib-2.0" -I"'+sysroot+'/usr/include/harfbuzz" -I"'+sysroot+'/usr/include/json-glib-1.0" -I"'+sysroot+'/usr/include/location" -I"'+sysroot+'/usr/include/maps" -I"'+sysroot+'/usr/include/media" -I"'+sysroot+'/usr/include/media-content" -I"'+sysroot+'/usr/include/messaging" -I"'+sysroot+'/usr/include/metadata-editor" -I"'+sysroot+'/usr/include/minicontrol" -I"'+sysroot+'/usr/include/minizip" -I"'+sysroot+'/usr/include/network" -I"'+sysroot+'/usr/include/notification" -I"'+sysroot+'/usr/include/phonenumber-utils" -I"'+sysroot+'/usr/include/sensor" -I"'+sysroot+'/usr/include/shortcut" -I"'+sysroot+'/usr/include/storage" -I"'+sysroot+'/usr/include/system" -I"'+sysroot+'/usr/include/telephony" -I"'+sysroot+'/usr/include/ui" -I"'+sysroot+'/usr/include/web" -I"'+sysroot+'/usr/include/widget_service" -I"'+sysroot+'/usr/include/widget_viewer_evas" -I"'+sysroot+'/usr/include/wifi-direct" -I"'+sysroot+'/usr/lib/dbus-1.0/include" -I"'+sysroot+'/usr/lib/glib-2.0/include"').split(' ')
    env.Append(CPPFLAGS=cflags)


    #env.Append(LINKFLAGS=['--sysroot="'+sysroot+'"'])
    env.Append(LINKFLAGS=['--gcc-toolchain="' + tizen_root + '/tools/'+target+'-linux-gnueabi-gcc-4.9"'])
    env.Append(LINKFLAGS=['-target', target+'-tizen-linux-gnueabi', '-ccc-gcc-name', target+'-linux-gnueabi-g++'])
    env.Append(LIBS='-lBrokenLocale -laccounts-svc -lanl -lappcore-agent -lattach-panel -lbadge -lbase-utils-i18n -lbundle -lcairo -lcalendar-service2 -lcapi-appfw-alarm -lcapi-appfw-app-common -lcapi-appfw-app-control -lcapi-appfw-application -lcapi-appfw-app-manager -lcapi-appfw-event -lcapi-appfw-package-manager -lcapi-appfw-preference -lcapi-appfw-widget-application -lcapi-base-common -lcapi-content-media-content -lcapi-content-mime-type -lcapi-data-control -lcapi-geofence-manager -lcapi-location-manager -lcapi-maps-service -lcapi-media-audio-io -lcapi-media-camera -lcapi-media-codec -lcapi-media-controller -lcapi-media-image-util -lcapi-media-metadata-editor -lcapi-media-metadata-extractor -lcapi-media-player -lcapi-media-radio -lcapi-media-recorder -lcapi-media-screen-mirroring -lcapi-media-sound-manager -lcapi-media-thumbnail-util -lcapi-media-tone-player -lcapi-media-tool -lcapi-media-video-util -lcapi-media-vision -lcapi-media-wav-player -lcapi-message-port -lcapi-messaging-email -lcapi-messaging-messages -lcapi-network-bluetooth -lcapi-network-connection -lcapi-network-nfc -lcapi-network-smartcard -lcapi-network-wifi -lcapi-system-device -lcapi-system-info -lcapi-system-media-key -lcapi-system-runtime-info -lcapi-system-sensor -lcapi-system-system-settings -lcapi-telephony -lcapi-ui-efl-util -lcapi-ui-inputmethod-manager -lcapi-ui-inputmethod -lcapi-web-url-download -lcidn -lcontacts-service2 -lcontext -lcore-context-manager -lcore-sync-client -lcrypto -lcrypt -lc -lcurl -ldali-adaptor -ldali-core -ldali-toolkit -ldlog -ldl -lebluez -leconnman0_7x -lecore_buffer -lecore_con -lecore_evas -lecore_file -lecore_imf_evas -lecore_imf -lecore_input_evas -lecore_input -lecore_ipc -lecore -lecore_x -ledbus -ledje -leet -lefl-extension -lefreet_mime -lefreet -lefreet_trash -lehal -leina -leio -lelementary -lembryo -lenotify -leofono -leom -leo -lethumb_client -lethumb -leukit -levas -lewebkit2 -lexif -lfeedback -lfontconfig -lfreetype -lgio-2.0 -lglib-2.0 -lgmodule-2.0 -lgobject-2.0 -lgthread-2.0 -lharfbuzz-icu -lharfbuzz -ljson-glib-1.0 -lkey-manager-client -lminicontrol-provider -lminicontrol-viewer -lminizip -lm -lnotification -lnsl -lnss_compat -lnss_dns -lnss_files -lnss_hesiod -lnss_nisplus -lnss_nis -loauth2 -loauth -lopenal -lphonenumber-utils -lprivilege-info -lpthread -lpush -lresolv -lrt -lservice-adaptor-client -lshortcut -lsqlite3 -lssl -lstorage -lstt -ltbm -lthread_db -lttrace -ltts -lutil -lvc-elm -lvc -lwidget_service -lwidget_viewer_evas -lwifi-direct -lxml2 -lz'.split(' '))
    lflags = ('-march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3-d16 -mtune=cortex-a8 -Xlinker --as-needed -pie -lpthread -Xlinker -L"'+sysroot+'/usr/lib" -Werror-implicit-function-declaration').split(' ')
    env.Append(LINKFLAGS=lflags)
    env.Append(CPPFLAGS=['-DTIZEN_ENABLED', '-DUNIX_ENABLED', '-DGLES2_ENABLED'])#
    env.Append(LIBS=['pthread', 'z'])#,
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

    env.Append(BUILDERS={'GLSL120': env.Builder(action=methods.build_legacygl_headers, suffix='glsl.h', src_suffix='.glsl')})
    env.Append(BUILDERS={'GLSL': env.Builder(action=methods.build_glsl_headers, suffix='glsl.h', src_suffix='.glsl')})
    env.Append(BUILDERS={'GLSL120GLES': env.Builder(action=methods.build_gles2_headers, suffix='glsl.h', src_suffix='.glsl')})
    env.Append( BUILDERS = { 'HLSL9' : env.Builder(action = methods.build_hlsl_dx9_headers, suffix = 'hlsl.h',src_suffix = '.hlsl') } )

    if (env["use_static_cpp"] == "yes"):
        env.Append(LINKFLAGS=['-static-libstdc++'])

    #list_of_x86 = ['x86_64', 'x86', 'i386', 'i586']
    #if any(platform.machine() in s for s in list_of_x86):
     #   env["x86_libtheora_opt_gcc"] = True
