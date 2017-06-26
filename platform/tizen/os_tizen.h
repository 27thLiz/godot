/*************************************************************************/
/*  OS_Tizen.h                                                             */
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
#ifndef OS_TIZEN_H
#define OS_TIZEN_H


#include "os/input.h"
#include "drivers/unix/os_unix.h"
#include "servers/visual_server.h"
#include "servers/visual/visual_server_wrap_mt.h"
#include "servers/visual/rasterizer.h"
#include "servers/physics_server.h"
#include "servers/audio/audio_server_sw.h"
#include "servers/audio/sample_manager_sw.h"
#include "servers/spatial_sound/spatial_sound_server_sw.h"
#include "servers/spatial_sound_2d/spatial_sound_2d_server_sw.h"
#include "drivers/rtaudio/audio_driver_rtaudio.h"
#include "servers/audio/audio_driver_dummy.h"
#include "servers/physics_2d/physics_2d_server_sw.h"
#include "servers/physics_2d/physics_2d_server_wrap_mt.h"
#include "main/input_default.h"

#include <dlog.h>
#include <Elementary.h>



typedef struct appdata {
	Evas_Object *win;
	Evas_Object *conform;
	Evas_Object *label;
	Ecore_Animator *ani;


	Evas_GL *evasgl;
	Evas_GL_Context *ctx;
	Evas_GL_Surface *sfc;
	Evas_GL_Config  *cfg;
	Evas_Object     *img;
	Evas_Coord       surface_w;
	Evas_Coord       surface_h;
	Eina_Bool        mouse_down : 1;
	Eina_Bool        initialized : 1;
	int argc;
	char* argv[];
} appdata_s;

class OS_Tizen : public OS_Unix {

	Rasterizer *rasterizer;
	VisualServer *visual_server;
	VideoMode current_videomode;
	List<String> args;
	MainLoop *main_loop;
	unsigned int event_id;

	PhysicsServer *physics_server;
	Physics2DServer *physics_2d_server;

	//Point2i center;

	virtual void delete_main_loop();
	//IP_Unix *ip_unix;

	AudioServerSW *audio_server;
	SampleManagerMallocSW *sample_manager;
	SpatialSoundServerSW *spatial_sound_server;
	SpatialSound2DServerSW *spatial_sound_2d_server;

	AudioDriverDummy audio_driver_dummy;
	bool force_quit;

	InputDefault *input;

	void _process_events();

	int argc;
	//char* argv[];

protected:

	virtual int get_video_driver_count() const;
	virtual const char * get_video_driver_name(int p_driver) const;
	virtual VideoMode get_default_video_mode() const;

	virtual int get_audio_driver_count() const;
	virtual const char * get_audio_driver_name(int p_driver) const;

	virtual void initialize(const VideoMode& p_desired,int p_video_driver,int p_audio_driver);

	virtual void finalize();

	virtual void set_main_loop( MainLoop * p_main_loop );


public:
	appdata_s ad;
	void step();
	bool _create_tizen_app();
	void set_args(int p_argc, char* p_argv[]);
	virtual String get_name();

	virtual void set_cursor_shape(CursorShape p_shape);

	void set_mouse_mode(MouseMode p_mode);
	MouseMode get_mouse_mode() const;

	virtual void warp_mouse_pos(const Point2& p_to);
	virtual Point2 get_mouse_pos() const;
	virtual int get_mouse_button_state() const;
	virtual void set_window_title(const String& p_title);

	virtual void set_icon(const Image& p_icon);

	virtual MainLoop *get_main_loop() const;
	virtual void vprint(const char *p_format, va_list ap );
	virtual void print(const char *p_format, ...);
	virtual bool can_draw() const;

	virtual void set_clipboard(const String& p_text);
	virtual String get_clipboard() const;

	virtual void release_rendering_thread();
	virtual void make_rendering_thread();
	virtual void swap_buffers();

	virtual String get_system_dir(SystemDir p_dir) const;

	virtual Error shell_open(String p_uri);

	virtual void set_video_mode(const VideoMode& p_video_mode,int p_screen=0);
	virtual VideoMode get_video_mode(int p_screen=0) const;
	virtual void get_fullscreen_mode_list(List<VideoMode> *p_list,int p_screen=0) const;

	virtual bool has_touchscreen_ui_hint() const;

	virtual int get_screen_count() const;
	virtual int get_current_screen() const;
	virtual void set_current_screen(int p_screen);
	virtual Point2 get_screen_position(int p_screen=0) const;
	virtual Size2 get_screen_size(int p_screen=0) const;
	virtual int get_screen_dpi(int p_screen=0) const;
	virtual Point2 get_window_position() const;
	virtual void set_window_position(const Point2& p_position);
	virtual Size2 get_window_size() const;
	virtual void set_window_size(const Size2 p_size);
	virtual void set_window_fullscreen(bool p_enabled);
	virtual bool is_window_fullscreen() const;
	virtual void set_window_resizable(bool p_enabled);
	virtual bool is_window_resizable() const;
	virtual void set_window_minimized(bool p_enabled);
	virtual bool is_window_minimized() const;
	virtual void set_window_maximized(bool p_enabled);
	virtual bool is_window_maximized() const;
	virtual void request_attention();

	virtual void move_window_to_foreground();
	virtual void alert(const String& p_alert,const String& p_title="ALERT!");

	virtual bool is_joy_known(int p_device);
	virtual String get_joy_guid(int p_device) const;

	virtual void set_context(int p_context);

	virtual void set_use_vsync(bool p_enable);
	virtual bool is_vsync_enabled() const;
	virtual String get_resource_dir() const;
	virtual String get_data_dir() const;
	void run();
	void resize(Size2 p_size);

	OS_Tizen();
	~OS_Tizen();

	// OS interface
//protected:
//	void initialize_core();
//	void finalize_core();

public:
	void vprint(const char *p_format, va_list p_list, bool p_stderr);
	//String get_stdin_string(bool p_block);
//	Error execute(const String &p_path, const List<String> &p_arguments, bool p_blocking, ProcessID *r_child_id, String *r_pipe, int *r_exitcode);
//	Error kill(const ProcessID &p_pid);
//	bool has_environment(const String &p_var) const;
//	String get_environment(const String &p_var) const;
//	Date get_date(bool local) const;
//	Time get_time(bool local) const;
//	TimeZoneInfo get_time_zone_info() const;
//	void delay_usec(uint32_t p_usec) const;

};
#endif
