/*************************************************************************/
/*  godot_x11.cpp                                                        */
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
#include "main/main.h"
#include "os_tizen.h"
#include "platform_config.h"
#include <app.h>
#include <system_settings.h>
#include <efl_extension.h>

EVAS_GL_GLOBAL_GLES2_DEFINE();

static void
win_delete_request_cb(void *data, Evas_Object *obj, void *event_info) {
	ui_app_exit();
}

static void
win_back_cb(void *data, Evas_Object *obj, void *event_info) {
	appdata_s *ad = (appdata_s*)data;
	/* Let window go to hide state. */
	elm_win_lower(ad->win);
}

static void _step(void* data, Evas_Object *obj) {
	OS_Tizen* os = (OS_Tizen*)data;
	os->step();
}

static void img_del_cb(void *data, Evas *e , Evas_Object *obj , void *event_info) {
	appdata_s *ad = (appdata_s *)data;
	ecore_animator_del(ad->ani);

	/* Free the gl resources when image object is deleted. */
	evas_gl_make_current(ad->evasgl, ad->sfc, ad->ctx);

	evas_gl_surface_destroy(ad->evasgl, ad->sfc);
	evas_gl_context_destroy(ad->evasgl, ad->ctx);
	evas_gl_config_free(ad->cfg);

	evas_gl_free(ad->evasgl);
}

static Eina_Bool animate_cb(void *data) {

	Evas_Object *img = (Evas_Object*)data;
	/* Animate here whenever an animation tick happens and then mark the image as
	   "dirty" meaning it needs an update next time evas renders. it will call the
	   pixel get callback then. */
	evas_object_image_pixels_dirty_set(img, EINA_TRUE);

	return ECORE_CALLBACK_RENEW;
}

static void mouse_down_cb(void *data, Evas *e , Evas_Object *obj , void *event_info) {
	appdata_s *ad = (appdata_s*)data;
	ad->mouse_down = EINA_TRUE;
	Evas_Event_Mouse_Down *ev;
	ev = (Evas_Event_Mouse_Down *)event_info;
	print_line("got mouse event, button: " + itos(ev->button) + ", pos: " + Vector2(ev->output.x, ev->output.y));
}

static void mouse_move_cb(void *data, Evas *e , Evas_Object *obj , void *event_info) {

	Evas_Event_Mouse_Move *ev;
	ev = (Evas_Event_Mouse_Move *)event_info;
	appdata_s *ad = (appdata_s*)data;
	float dx = 0, dy = 0;

//	if(ad->mouse_down) {
//		dx = ev->cur.canvas.x - ev->prev.canvas.x;
//		dy = ev->cur.canvas.y - ev->prev.canvas.y;
//		ad->xangle += dy;
//		ad->yangle += dx;
//	}
}

static void mouse_up_cb(void *data, Evas *e , Evas_Object *obj , void *event_info) {
	appdata_s *ad = (appdata_s*)data;
	ad->mouse_down = EINA_FALSE;
}

static void win_resize_cb(void *data, Evas *e , Evas_Object *obj , void *event_info) {
	appdata_s *ad = (appdata_s*)data;

	if(ad->sfc) {
		evas_object_image_native_surface_set(ad->img, NULL);
		evas_gl_surface_destroy(ad->evasgl, ad->sfc);
		ad->sfc = NULL;
	}

	evas_object_geometry_get(obj, NULL, NULL, &ad->surface_w, &ad->surface_h);
	evas_object_image_size_set(ad->img, ad->surface_w, ad->surface_h);
	evas_object_resize(ad->img, ad->surface_w, ad->surface_h);
	evas_object_show(ad->img);

	if(!ad->sfc) {
		Evas_Native_Surface ns;

		ad->sfc = evas_gl_surface_create(ad->evasgl, ad->cfg, ad->surface_w, ad->surface_h);
		evas_gl_native_surface_get(ad->evasgl, ad->sfc, &ns);
		evas_object_image_native_surface_set(ad->img, &ns);
		evas_object_image_pixels_dirty_set(ad->img, EINA_TRUE);
	}

	if (OS::get_singleton())
		((OS_Tizen*)OS::get_singleton())->resize(Size2(ad->surface_w, ad->surface_h));
}

static void canvas_resize_cb(void *data, Evas *e, Evas_Object *obj, void *event_info) {
	printf("resized canvas!!\n");
}

static bool init_evasgl(appdata_s* ad) {
	/* Set config of the surface for evas gl */
	ad->cfg = evas_gl_config_new();
	ad->cfg->color_format = EVAS_GL_RGB_888;
	ad->cfg->depth_bits = EVAS_GL_DEPTH_BIT_24;
	ad->cfg->stencil_bits = EVAS_GL_STENCIL_NONE;
	ad->cfg->options_bits = EVAS_GL_OPTIONS_NONE;

	/* Get the window size */
	evas_object_geometry_get(ad->win, NULL, NULL, &ad->surface_w, &ad->surface_h);

	/* Get the evas gl handle for doing gl things */
	ad->evasgl = evas_gl_new(evas_object_evas_get(ad->win));
	ERR_FAIL_COND_V(!ad->evasgl, false);
	/* Create a surface and context */
	ad->sfc = evas_gl_surface_create(ad->evasgl, ad->cfg, ad->surface_w, ad->surface_h);
	ad->ctx = evas_gl_context_create(ad->evasgl, NULL);

	EVAS_GL_GLOBAL_GLES2_USE(ad->evasgl, ad->ctx);

	ad->mouse_down = EINA_FALSE;
	ad->initialized = EINA_FALSE;

	ad->img = evas_object_image_filled_add(evas_object_evas_get(ad->conform));
	evas_object_event_callback_add(ad->img, EVAS_CALLBACK_DEL, img_del_cb, ad);
	evas_object_image_pixels_get_callback_set(ad->img, _step, OS::get_singleton());

	/* Add Mouse Event Callbacks */
	evas_object_event_callback_add(ad->img, EVAS_CALLBACK_MOUSE_DOWN, mouse_down_cb, ad);
	evas_object_event_callback_add(ad->img, EVAS_CALLBACK_MOUSE_UP, mouse_up_cb, ad);
	evas_object_event_callback_add(ad->img, EVAS_CALLBACK_MOUSE_MOVE, mouse_move_cb, ad);

	ad->ani = ecore_animator_add(animate_cb, ad->img);
	elm_object_content_set(ad->conform,ad->img);
	return true;
}

static bool app_create(void *data) {
	//OS_Tizen* os = (OS_Tizen*)data;
	//return os->_create_tizen_app();
	appdata_s* ad = (appdata_s*)data;
	Evas_Object *win;
	elm_config_accel_preference_set("opengl");
	win = elm_win_util_standard_add("Godot", "Godot"); //TODO replace name on export
	ERR_FAIL_COND_V(!win, false);
	evas_object_show(win);
	ad->win = win;
	elm_win_conformant_set(ad->win,EINA_TRUE);
	elm_win_indicator_mode_set(ad->win,ELM_WIN_INDICATOR_SHOW);
	elm_win_indicator_opacity_set(ad->win,ELM_WIN_INDICATOR_TRANSPARENT);

	Evas_Object *conform;
	conform = elm_conformant_add(ad->win);
	evas_object_size_hint_weight_set(conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(ad->win, conform);
	evas_object_show(conform);
	ad->conform = conform;
	ERR_FAIL_COND_V(!init_evasgl(ad), false);
	eext_object_event_callback_add(win, EEXT_CALLBACK_BACK, win_back_cb, ad);
	evas_object_event_callback_add(ad->win, EVAS_CALLBACK_RESIZE, win_resize_cb, ad);
	evas_object_event_callback_add(ad->win, EVAS_CALLBACK_CANVAS_VIEWPORT_RESIZE, canvas_resize_cb, NULL);
	evas_object_smart_callback_add(ad->win, "delete,request", win_delete_request_cb, NULL);

	return true;
}

static void app_control(app_control_h app_control, void *data)
{
	/* Handle the launch request. */
}

static void app_pause(void *data) {
	/* Take necessary actions when application becomes invisible. */
}

static void app_resume(void *data) {
	/* Take necessary actions when application becomes visible. */
}

static void app_terminate(void *data) {
	Main::cleanup();
	/* Release all resources. */

}

static void ui_app_lang_changed(app_event_info_h event_info, void *user_data) {
	/*APP_EVENT_LANGUAGE_CHANGED*/

	int ret;
	char *language;

	ret = app_event_get_language(event_info, &language);
	if (ret != APP_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "app_event_get_language() failed. Err = %d.", ret);
		return;
	}

	if (language != NULL) {
		elm_language_set(language);
		free(language);
	}
}

static void ui_app_orient_changed(app_event_info_h event_info, void *user_data) {
	/*APP_EVENT_DEVICE_ORIENTATION_CHANGED*/
	return;
}

static void ui_app_region_changed(app_event_info_h event_info, void *user_data) {
	/*APP_EVENT_REGION_FORMAT_CHANGED*/
}

static void ui_app_low_battery(app_event_info_h event_info, void *user_data) {
	/*APP_EVENT_LOW_BATTERY*/
}

static void ui_app_low_memory(app_event_info_h event_info, void *user_data) {
	/*APP_EVENT_LOW_MEMORY*/
}

int main(int argc, char* argv[]) {

	OS_Tizen os;
//	os.set_args(argc, argv);
//	//dlog_print(DLOG_ERROR, "godot", "INIT TIZEN !!");
//	//os.print("OS TIZEN INIT !!");
//	Error err  = Main::setup(argv[0],argc-1,&argv[1]);
//	if (err!=OK) {
//		print_line("tizen: godot init error");
//		return 255;
//	}

//	if (Main::start())
//		os.run(); // it is actually the OS that decides how to run
//	Main::cleanup();


	int ret = 0;

	ui_app_lifecycle_callback_s event_callback = {NULL,};
	app_event_handler_h handlers[5] = {NULL, };

	event_callback.create = app_create;
	event_callback.terminate = app_terminate;
	event_callback.pause = app_pause;
	event_callback.resume = app_resume;
	event_callback.app_control = app_control;
	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY], APP_EVENT_LOW_BATTERY, ui_app_low_battery, &os.ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY], APP_EVENT_LOW_MEMORY, ui_app_low_memory, &os.ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED], APP_EVENT_DEVICE_ORIENTATION_CHANGED, ui_app_orient_changed, &os.ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, ui_app_lang_changed, &os.ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED], APP_EVENT_REGION_FORMAT_CHANGED, ui_app_region_changed, &os.ad);
	ret = ui_app_main(argc, argv, &event_callback, &os.ad);


	return os.get_exit_code();
}
