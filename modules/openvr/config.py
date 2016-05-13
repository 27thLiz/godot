
def can_build(platform):
    return True

def configure(env):
    env.Append(LIBPATH=['#modules/openvr/openvr/lib/win32'])
    env.Append(LINKFLAGS=['openvr_api.lib'])
    #env.Append(CPPFLAGS=['-std=c++11'])
