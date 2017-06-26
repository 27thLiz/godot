/*************************************************************************/
/*  OS_Tizen.cpp                                                           */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                    http://www.godotengine.org                         */
/*************************************************************************/
/* Copyright (c) 2007-2016 Juan Linietsky, Ariel Manzur.                 */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/
#include "drivers/unix/memory_pool_static_malloc.h"
#include "os/memory_pool_dynamic_static.h"
#include "servers/visual/visual_server_raster.h"
#include "servers/visual/rasterizer_dummy.h"
#include "drivers/gles2/rasterizer_gles2.h"
#include "drivers/unix/thread_posix.h"
#include "drivers/unix/semaphore_posix.h"
#include "drivers/unix/mutex_posix.h"
#include "drivers/unix/file_access_unix.h"
#include "drivers/unix/dir_access_unix.h"
#include "drivers/unix/tcp_server_posix.h"
#include "drivers/unix/stream_peer_tcp_posix.h"
#include "drivers/unix/packet_peer_udp_posix.h"
#include "os_tizen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "print_string.h"
#include "servers/physics/physics_server_sw.h"
#include "errno.h"
#include <app.h>
#include <system_settings.h>
#include <efl_extension.h>

#include "main/main.h"
#ifdef  LOG_TAG
#undef  LOG_TAG
#endif
#define LOG_TAG "godot"

#if !defined(PACKAGE)
#define PACKAGE "org.godotengine.godot"
#endif

OS_Tizen::~OS_Tizen() {

}

void OS_Tizen::step() {
	static int step = 0;
	evas_gl_make_current(ad.evasgl, ad.sfc, ad.ctx);
	if (step == 0) {
		printf("step 0");
		Error err = Main::setup(ad.argv[0],ad.argc-1,&ad.argv[1]);
		ERR_FAIL_COND(err != OK);
		Main::start();
		force_quit = false;
		ERR_FAIL_COND(!main_loop);
		main_loop->init();
		step++;
		return;
	}
	//TODO Event processing
	if (Main::iteration()) {
		ERR_FAIL();
		//TODO break out of main loop
	}
}

void OS_Tizen::run() {
	print_line("TIZEN: starting main loop");


//	while (!force_quit) {
//		//TODO: event processing

//		if (Main::iteration())
//			break;
//	}
	main_loop->finish();
}

void OS_Tizen::resize(Size2 p_size) {
	//print_line(String("resize callback, new size: ") + p_size);
	printf("resize callback, new size: (%d,%d)\n", p_size.x, p_size.y);
	OS::VideoMode vm = current_videomode;
	vm.width = p_size.width;
	vm.height = p_size.height;
	set_video_mode(vm);
}

OS_Tizen::OS_Tizen() {
	//print_line("OS_TIZEN INIT");
	print("OS constructor");
	AudioDriverManagerSW::add_driver(&audio_driver_dummy);
	event_id = 0;
}

void OS_Tizen::delete_main_loop() {
	if (main_loop)
		memdelete(main_loop);
	main_loop = NULL;
}

void OS_Tizen::_process_events() {

}

int OS_Tizen::get_video_driver_count() const {
	return 1;
}

const char *OS_Tizen::get_video_driver_name(int p_driver) const {
 return "GLES2";
}

OS::VideoMode OS_Tizen::get_default_video_mode() const {
	return OS::VideoMode(ad.surface_w, ad.surface_h, true, false, false);
}

int OS_Tizen::get_audio_driver_count() const {
	return 1;
}

const char *OS_Tizen::get_audio_driver_name(int p_driver) const {
	return "";
}

void OS_Tizen::initialize(const VideoMode &p_desired, int p_video_driver, int p_audio_driver) {
	dlog_print(DLOG_ERROR, LOG_TAG, "INITIALIZE GODOT!!\n");
	//AudioDriverManagerSW::add_driver()
#ifdef GLES2_ENABLED
	rasterizer = memnew(RasterizerGLES2(false, false, false));
	//rasterizer->set_force_16_bits_fbo(use_16bits_fbo);
	visual_server = memnew(VisualServerRaster(rasterizer));
#else
	rasterizer = memnew(RasterizerDummy);
	visual_server = memnew(VisualServerRaster(rasterizer));
#endif
	ERR_FAIL_COND(!visual_server);
	visual_server->init();
	visual_server->cursor_set_visible(false, 0);

	current_videomode = p_desired;
	AudioDriverManagerSW::get_driver(p_audio_driver)->set_singleton();
	if (AudioDriverManagerSW::get_driver(p_audio_driver)->init() != OK) {
		ERR_PRINT("Initializing audio failed.");
	}
	sample_manager = memnew(SampleManagerMallocSW);
	audio_server = memnew(AudioServerSW(sample_manager));
	spatial_sound_server = memnew( SpatialSoundServerSW );
	spatial_sound_server->init();
	spatial_sound_2d_server = memnew( SpatialSound2DServerSW );
	spatial_sound_2d_server->init();
	physics_server = memnew(PhysicsServerSW);
	physics_server->init();
	physics_2d_server = Physics2DServerWrapMT::init_server<Physics2DServerSW>();
	physics_2d_server->init();

	print_line("init input");
	input = memnew(InputDefault);
	print_line("init complete");
}

void OS_Tizen::vprint(const char *p_format, va_list ap ) {

	//va_start(argp, p_format);
	dlog_vprint(DLOG_ERROR, LOG_TAG, p_format, ap);
	//vprintf(p_format, argp );
	//va_end(argp);

}

void OS_Tizen::print(const char *p_format, ...) {
	va_list argp;
	va_start(argp, p_format);
	dlog_vprint(DLOG_ERROR, LOG_TAG, p_format, argp);
	//vprintf(p_format, argp );
	va_end(argp);
}
void OS_Tizen::finalize() {
	print_line("TIZEN FINALIZE");
	delete_main_loop();
	memdelete(input);
	memdelete(sample_manager);
	audio_server->finish();
	memdelete(audio_server);
#ifdef GLES2_ENABLED
	visual_server->finish();
	memdelete(visual_server);
	memdelete(rasterizer);
#endif

	physics_server->finish();
	memdelete(physics_server);
}

void OS_Tizen::set_main_loop(MainLoop *p_main_loop) {
	main_loop = p_main_loop;
	input->set_main_loop(p_main_loop);
}

bool OS_Tizen::_create_tizen_app() {

}

void OS_Tizen::set_args(int p_argc, char *p_argv[]) {
	//argc = p_argc;
	//*argv = *p_argv;
}

String OS_Tizen::get_name() {
	return "Tizen";
}

void OS_Tizen::set_cursor_shape(CursorShape p_shape) {

}

void OS_Tizen::set_mouse_mode(MouseMode p_mode) {

}

OS::MouseMode OS_Tizen::get_mouse_mode() const {
	return MOUSE_MODE_HIDDEN;
}

void OS_Tizen::warp_mouse_pos(const Point2 &p_to) {

}

Point2 OS_Tizen::get_mouse_pos() const {
	return Point2();
}

int OS_Tizen::get_mouse_button_state() const {
	return 0;
}

void OS_Tizen::set_window_title(const String &p_title) {

}

void OS_Tizen::set_icon(const Image &p_icon) {

}

MainLoop *OS_Tizen::get_main_loop() const {
	return main_loop;
}

bool OS_Tizen::can_draw() const {
	return true; //FIXME
}

String OS_Tizen::get_resource_dir() const {

	return app_get_resource_path();
	//	return "/"; //Tizen has it's own filesystem for resources inside the TPK
}

String OS_Tizen::get_data_dir() const {
	return app_get_data_path();
}

void OS_Tizen::set_clipboard(const String &p_text) {

}

String OS_Tizen::get_clipboard() const {
	return ""; //FIXME
}

void OS_Tizen::release_rendering_thread() {
	//evas_gl_release_current(ad.evasgl,ad.sfc, ad.ctx);
}

void OS_Tizen::make_rendering_thread() {
	evas_gl_make_current(ad.evasgl, ad.sfc, ad.ctx);
}

void OS_Tizen::swap_buffers() {

}

String OS_Tizen::get_system_dir(SystemDir p_dir) const {
	return "";
}

Error OS_Tizen::shell_open(String p_uri) {
	return FAILED; //FIXME
}

void OS_Tizen::set_video_mode(const VideoMode &p_video_mode, int p_screen) {
	current_videomode = p_video_mode;
}

OS::VideoMode OS_Tizen::get_video_mode(int p_screen) const {
	return current_videomode;
}

void OS_Tizen::get_fullscreen_mode_list(List<VideoMode> *p_list, int p_screen) const {

}

bool OS_Tizen::has_touchscreen_ui_hint() const {
	return true;
}

int OS_Tizen::get_screen_count() const {
	return 1;
}

int OS_Tizen::get_current_screen() const {
	return 0;
}

void OS_Tizen::set_current_screen(int p_screen) {

}

Point2 OS_Tizen::get_screen_position(int p_screen) const {
	return Point2();
}

Size2 OS_Tizen::get_screen_size(int p_screen) const {

}

int OS_Tizen::get_screen_dpi(int p_screen) const {
	return 294; //FIXME
}

Point2 OS_Tizen::get_window_position() const {
	return Point2();
}

void OS_Tizen::set_window_position(const Point2 &p_position) {

}

Size2 OS_Tizen::get_window_size() const {
	return Size2(ad.surface_w, ad.surface_h);
}

void OS_Tizen::set_window_size(const Size2 p_size) {

}

void OS_Tizen::set_window_fullscreen(bool p_enabled) {

}

bool OS_Tizen::is_window_fullscreen() const {
	return true;
}

void OS_Tizen::set_window_resizable(bool p_enabled) {

}

bool OS_Tizen::is_window_resizable() const {
	return false;
}

void OS_Tizen::set_window_minimized(bool p_enabled) {

}

bool OS_Tizen::is_window_minimized() const {
	return false;
}

void OS_Tizen::set_window_maximized(bool p_enabled) {

}

bool OS_Tizen::is_window_maximized() const {
	return true;
}

void OS_Tizen::request_attention() {

}

void OS_Tizen::move_window_to_foreground() {

}

void OS_Tizen::alert(const String &p_alert, const String &p_title) {

}

bool OS_Tizen::is_joy_known(int p_device) {
	return false;
}

String OS_Tizen::get_joy_guid(int p_device) const {
	return "";
}

void OS_Tizen::set_context(int p_context) {

}

void OS_Tizen::set_use_vsync(bool p_enable) {

}

bool OS_Tizen::is_vsync_enabled() const {
	return false; // FIXME
}

static MemoryPoolStaticMalloc *mempool_static=NULL;
static MemoryPoolDynamicStatic *mempool_dynamic=NULL;

//void OS_Tizen::initialize_core() {
//	//OS_Unix::initialize_core();
//	ThreadPosix::make_default();
//	SemaphorePosix::make_default();
//	MutexPosix::make_default();

//	FileAccess::make_default<FileAccessUnix>(FileAccess::ACCESS_RESOURCES);
//	FileAccess::make_default<FileAccessUnix>(FileAccess::ACCESS_USERDATA);
//	FileAccess::make_default<FileAccessUnix>(FileAccess::ACCESS_FILESYSTEM);
//	//FileAccessBufferedFA<FileAccessUnix>::make_default();
//	DirAccess::make_default<DirAccessUnix>(DirAccess::ACCESS_RESOURCES);
//	DirAccess::make_default<DirAccessUnix>(DirAccess::ACCESS_USERDATA);
//	DirAccess::make_default<DirAccessUnix>(DirAccess::ACCESS_FILESYSTEM);

//#ifndef NO_NETWORK
//	TCPServerPosix::make_default();
//	StreamPeerTCPPosix::make_default();
//	PacketPeerUDPPosix::make_default();
//	IP_Unix::make_default();
//#endif

//	mempool_static = new MemoryPoolStaticMalloc;
//	mempool_dynamic = memnew(MemoryPoolDynamicStatic);
//}

//void OS_Tizen::finalize_core() {
//	if (mempool_dynamic)
//		memdelete(mempool_dynamic);
//	delete mempool_static;
//}

void OS_Tizen::vprint(const char *p_format, va_list p_list, bool p_stderr)
{
}



