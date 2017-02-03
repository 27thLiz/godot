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
#include "servers/visual/visual_server_raster.h"
#include "drivers/gles2/rasterizer_gles2.h"
#include "os_tizen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "print_string.h"
#include "servers/physics/physics_server_sw.h"
#include "errno.h"
#include <app.h>
#include <Elementary.h>
#include <system_settings.h>
#include <efl_extension.h>
#include <dlog.h>

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

void OS_Tizen::run() {

}

OS_Tizen::OS_Tizen()
{

}

void OS_Tizen::delete_main_loop() {
	memdelete(main_loop);
}

int OS_Tizen::get_video_driver_count() const {
	return 1;
}

const char *OS_Tizen::get_video_driver_name(int p_driver) const {
 return "GLES2";
}

OS::VideoMode OS_Tizen::get_default_video_mode() const {
	return OS::VideoMode();
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
	rasterizer = memnew(RasterizerGLES2(false, true, true, false));
	//rasterizer->set_force_16_bits_fbo(use_16bits_fbo);
	visual_server = memnew(VisualServerRaster(rasterizer));
	visual_server->init();
	visual_server->cursor_set_visible(false, 0);


	sample_manager = memnew(SampleManagerMallocSW);


	physics_server = memnew(PhysicsServerSW);
	physics_server->init();
	physics_2d_server = Physics2DServerWrapMT::init_server<Physics2DServerSW>();
	physics_2d_server->init();

	input = memnew(InputDefault);
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

}

void OS_Tizen::set_main_loop(MainLoop *p_main_loop) {
	main_loop = p_main_loop;
	input->set_main_loop(p_main_loop);
}

String OS_Tizen::get_name() {
	return "Tizen";
}

void OS_Tizen::set_cursor_shape(CursorShape p_shape) {

}

void OS_Tizen::set_mouse_mode(MouseMode p_mode) {

}

OS::MouseMode OS_Tizen::get_mouse_mode() const {

}

void OS_Tizen::warp_mouse_pos(const Point2 &p_to) {

}

Point2 OS_Tizen::get_mouse_pos() const {

}

int OS_Tizen::get_mouse_button_state() const {

}

void OS_Tizen::set_window_title(const String &p_title) {

}

void OS_Tizen::set_icon(const Image &p_icon) {

}

MainLoop *OS_Tizen::get_main_loop() const {

}

bool OS_Tizen::can_draw() const {

}

void OS_Tizen::set_clipboard(const String &p_text) {

}

String OS_Tizen::get_clipboard() const {

}

void OS_Tizen::release_rendering_thread() {

}

void OS_Tizen::make_rendering_thread() {

}

void OS_Tizen::swap_buffers() {

}

String OS_Tizen::get_system_dir(SystemDir p_dir) const {

}

Error OS_Tizen::shell_open(String p_uri) {

}

void OS_Tizen::set_video_mode(const VideoMode &p_video_mode, int p_screen) {

}

OS::VideoMode OS_Tizen::get_video_mode(int p_screen) const {
	return OS::VideoMode(); // TODO
}

void OS_Tizen::get_fullscreen_mode_list(List<VideoMode> *p_list, int p_screen) const {

}

int OS_Tizen::get_screen_count() const {

}

int OS_Tizen::get_current_screen() const {

}

void OS_Tizen::set_current_screen(int p_screen) {

}

Point2 OS_Tizen::get_screen_position(int p_screen) const {

}

Size2 OS_Tizen::get_screen_size(int p_screen) const {

}

int OS_Tizen::get_screen_dpi(int p_screen) const {

}

Point2 OS_Tizen::get_window_position() const {

}

void OS_Tizen::set_window_position(const Point2 &p_position) {

}

Size2 OS_Tizen::get_window_size() const {

}

void OS_Tizen::set_window_size(const Size2 p_size) {

}

void OS_Tizen::set_window_fullscreen(bool p_enabled) {

}

bool OS_Tizen::is_window_fullscreen() const {

}

void OS_Tizen::set_window_resizable(bool p_enabled) {

}

bool OS_Tizen::is_window_resizable() const {

}

void OS_Tizen::set_window_minimized(bool p_enabled) {

}

bool OS_Tizen::is_window_minimized() const {

}

void OS_Tizen::set_window_maximized(bool p_enabled) {

}

bool OS_Tizen::is_window_maximized() const {

}

void OS_Tizen::request_attention() {

}

void OS_Tizen::move_window_to_foreground() {

}

void OS_Tizen::alert(const String &p_alert, const String &p_title) {

}

bool OS_Tizen::is_joy_known(int p_device) {

}

String OS_Tizen::get_joy_guid(int p_device) const {

}

void OS_Tizen::set_context(int p_context) {

}

void OS_Tizen::set_use_vsync(bool p_enable) {

}

bool OS_Tizen::is_vsync_enabled() const {
	return false; // FIXME
}


void OS_Tizen::initialize_core() {
	OS_Unix::initialize_core();
}

void OS_Tizen::finalize_core()
{
}

void OS_Tizen::vprint(const char *p_format, va_list p_list, bool p_stderr)
{
}

String OS_Tizen::get_stdin_string(bool p_block)
{
}

Error OS_Tizen::execute(const String &p_path, const List<String> &p_arguments, bool p_blocking, ProcessID *r_child_id, String *r_pipe, int *r_exitcode)
{
}

Error OS_Tizen::kill(const ProcessID &p_pid)
{
}

bool OS_Tizen::has_environment(const String &p_var) const
{
}

String OS_Tizen::get_environment(const String &p_var) const
{
}

OS::Date OS_Tizen::get_date(bool local) const
{
}

OS::Time OS_Tizen::get_time(bool local) const
{
}

OS::TimeZoneInfo OS_Tizen::get_time_zone_info() const
{
}

void OS_Tizen::delay_usec(uint32_t p_usec) const
{
}

uint64_t OS_Tizen::get_ticks_usec() const
{
}
