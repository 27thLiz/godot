#ifdef ANGLE_ENABLED

#include "gl_context_egl_angle.h"

#include "EGL/eglext.h"

void ContextEGLAngle::release_current() {

	eglMakeCurrent(mEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, mEglContext);
};

void ContextEGLAngle::make_current() {

	eglMakeCurrent(mEglDisplay, mEglSurface, mEglSurface, mEglContext);
};

int ContextEGLAngle::get_window_width() {

	return width;
};

int ContextEGLAngle::get_window_height() {

	return height;
};

void ContextEGLAngle::reset() {

	cleanup();

	initialize();
};

void ContextEGLAngle::swap_buffers() {

	eglSwapBuffers(mEglDisplay, mEglSurface);
};

Error ContextEGLAngle::initialize() {

	EGLint configAttribList[] = {
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_ALPHA_SIZE, 8,
		EGL_DEPTH_SIZE, 8,
		EGL_STENCIL_SIZE, 8,
		EGL_SAMPLE_BUFFERS, 0,
		EGL_NONE
	};

	EGLint surfaceAttribList[] = {
		EGL_NONE, EGL_NONE
	};

	EGLint numConfigs = 0;
	EGLint majorVersion = 1;
	EGLint minorVersion = 0;
	EGLDisplay display = EGL_NO_DISPLAY;
	EGLContext context = EGL_NO_CONTEXT;
	EGLSurface surface = EGL_NO_SURFACE;
	EGLConfig config = nullptr;
	EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE, EGL_NONE };

	const EGLint displayAttributes[] =
	{
		EGL_PLATFORM_ANGLE_TYPE_ANGLE, EGL_PLATFORM_ANGLE_TYPE_D3D9_ANGLE,
		EGL_PLATFORM_ANGLE_MAX_VERSION_MAJOR_ANGLE, 9,
		EGL_PLATFORM_ANGLE_MAX_VERSION_MINOR_ANGLE, 3,
		EGL_NONE,
	};

	PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT = reinterpret_cast<PFNEGLGETPLATFORMDISPLAYEXTPROC>(eglGetProcAddress("eglGetPlatformDisplayEXT"));

	if (!eglGetPlatformDisplayEXT)
	{
		cleanup();
		return FAILED;
	}

	display = eglGetPlatformDisplayEXT(EGL_PLATFORM_ANGLE_ANGLE, EGL_DEFAULT_DISPLAY, displayAttributes);

	if (display == EGL_NO_DISPLAY)
	{
		cleanup();
		return FAILED;
	}

	if (eglInitialize(display, &majorVersion, &minorVersion) == EGL_FALSE)
	{
		cleanup();
		return FAILED;
	}

	if (eglGetConfigs(display, NULL, 0, &numConfigs) == EGL_FALSE)
	{
		cleanup();
		return FAILED;
	}

	if (eglChooseConfig(display, configAttribList, &config, 1, &numConfigs) == EGL_FALSE)
	{
		cleanup();
		return FAILED;
	}

	surface = eglCreateWindowSurface(display, config, window, surfaceAttribList);
	if (surface == EGL_NO_SURFACE)
	{
		cleanup();
		return FAILED;
	}

	context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);
	if (context == EGL_NO_CONTEXT)
	{
		cleanup();
		return FAILED;
	}

	if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE)
	{
		cleanup();
		return FAILED;
	}

	mEglDisplay = display;
	mEglSurface = surface;
	mEglContext = context;

	eglQuerySurface(display,surface,EGL_WIDTH,&width);
	eglQuerySurface(display,surface,EGL_HEIGHT,&height);

	return OK;
};

void ContextEGLAngle::cleanup() {

	if (mEglDisplay != EGL_NO_DISPLAY && mEglSurface != EGL_NO_SURFACE)
	{
		eglDestroySurface(mEglDisplay, mEglSurface);
		mEglSurface = EGL_NO_SURFACE;
	}

	if (mEglDisplay != EGL_NO_DISPLAY && mEglContext != EGL_NO_CONTEXT)
	{
		eglDestroyContext(mEglDisplay, mEglContext);
		mEglContext = EGL_NO_CONTEXT;
	}

	if (mEglDisplay != EGL_NO_DISPLAY)
	{
		eglTerminate(mEglDisplay);
		mEglDisplay = EGL_NO_DISPLAY;
	}
};

ContextEGLAngle::ContextEGLAngle(HWND p_window) :
	mEglDisplay(EGL_NO_DISPLAY),
	mEglContext(EGL_NO_CONTEXT),
	mEglSurface(EGL_NO_SURFACE)
 {

	window = p_window;
};

ContextEGLAngle::~ContextEGLAngle() {

	cleanup();
};

#endif
