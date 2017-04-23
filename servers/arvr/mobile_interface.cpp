/*************************************************************************/
/*  mobile_interface.cpp                                                 */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                    http://www.godotengine.org                         */
/*************************************************************************/
/* Copyright (c) 2007-2017 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2017 Godot Engine contributors (cf. AUTHORS.md)    */
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

#include "mobile_interface.h"
#include "core/os/os.h"
#include "servers/visual/visual_server_global.h"

StringName MobileInterface::get_name() const {
	return "Native mobile";
};

void MobileInterface::_bind_methods(){
	///@TODO add properties and methods for iod, display width, display to lens, oversample and k1 and k2
};

bool MobileInterface::is_installed() {
	// we don't have any middle ware here, if we have our interface, we can use it
	return true;
};

bool MobileInterface::hmd_is_present() {
	// our device is our HMD
	return true;
};

bool MobileInterface::supports_hmd() {
	// our device is our HMD
	return true;
};

bool MobileInterface::is_stereo() {
	// needs stereo...
	return true;
};

bool MobileInterface::is_initialized() {
	return (hmd_tracker != NULL);
};

bool MobileInterface::initialize() {
	if (hmd_tracker == NULL) {
		// create a new tracker for our device
		hmd_tracker = memnew(ArVrPositionalTracker);
		hmd_tracker->set_name("MobileHMD");
		hmd_tracker->set_type(ArVrServer::TRACKER_HMD);

		// init these to set our flags, note that we only support orientation
		Basis orientation;
		hmd_tracker->set_orientation(orientation);

		// register our tracker
		arvr_server->add_tracker(hmd_tracker);

		// make this our primary interface
		arvr_server->set_primary_interface(this);
	};

	return true;
};

void MobileInterface::uninitialize() {
	if (hmd_tracker != NULL) {
		// no longer our primary interface
		arvr_server->clear_primary_interface_if(this);

		// deregister our tracker
		arvr_server->remove_tracker(hmd_tracker);

		// free up memory
		memdelete(hmd_tracker);
		hmd_tracker = NULL;
	};
};

Size2 MobileInterface::get_recommended_render_targetsize() {
	_THREAD_SAFE_METHOD_

	// we use half our window size
	Size2 target_size = OS::get_singleton()->get_window_size();
	target_size.x *= 0.5 * oversample;
	target_size.y *= oversample;

	return target_size;
};

Transform MobileInterface::get_transform_for_eye(ArVrInterface::Eyes p_eye, const Transform &p_cam_transform) {
	_THREAD_SAFE_METHOD_

	Transform transform_for_eye;

	if (hmd_tracker != NULL) {
		// we don't need to check for the existance of our HMD, doesn't effect our values...
		// note * 0.01 to convert cm to m and * 0.5 as we're moving half in each direction...
		if (p_eye == ArVrInterface::EYE_LEFT) {
			transform_for_eye.origin.x = -(intraocular_dist * 0.01 * 0.5 * arvr_server->get_world_scale());
		} else if (p_eye == ArVrInterface::EYE_RIGHT) {
			transform_for_eye.origin.x = intraocular_dist * 0.01 * 0.5 * arvr_server->get_world_scale();
		} else {
			// for mono we don't reposition
		};

		// Apply, we no longer leave it up to the user to apply the trackers orientation to our camera node instead
		// treating our camera node as an origin point. This way we get as little as possible latency in rendering to the HMD
		// We do keep the tracker so our game logic also has access to this information
		transform_for_eye = p_cam_transform * (hmd_tracker->get_orientation()) * transform_for_eye;
	} else {
		// huh? well just return what we got....
		transform_for_eye = p_cam_transform;
	};

	return transform_for_eye;
};

CameraMatrix MobileInterface::get_projection_for_eye(ArVrInterface::Eyes p_eye, real_t p_aspect, real_t p_z_near, real_t p_z_far) {
	_THREAD_SAFE_METHOD_

	CameraMatrix eye;

	if (p_eye == ArVrInterface::EYE_MONO) {
		///@TODO for now hardcode some of this, what is really needed here is that this needs to be in sync with the real cameras properties
		// which probably means implementing a specific class for iOS and Android. For now this is purely here as an example.
		// Note also that if you use a normal viewport with AR/VR turned off you can still use the tracker output of this interface
		// to position a stock standard Godot camera and have control over this.
		// This will make more sense when we implement ARkit on iOS (probably a separate interface).
		eye.set_perspective(60.0, p_aspect, p_z_near, p_z_far, false);
	} else {
		eye.set_for_hmd(p_eye == ArVrInterface::EYE_LEFT ? 1 : 2, p_aspect, intraocular_dist, display_width, display_to_lens, oversample, p_z_near, p_z_far);
	};

	return eye;
};

void MobileInterface::commit_for_eye(ArVrInterface::Eyes p_eye, RID p_render_target) {
	_THREAD_SAFE_METHOD_
	///@TODO need to blit this out to the screen using a lens distortion shader.

	Rect2 screen_rect;
	screen_rect.size = OS::get_singleton()->get_window_size();
	if (p_eye == ArVrInterface::EYE_LEFT) {
		screen_rect.size.x /= 2.0;
	} else if (p_eye == ArVrInterface::EYE_RIGHT) {
		screen_rect.size.x /= 2.0;
		screen_rect.position.x = screen_rect.size.x;
	}

	VSG::rasterizer->set_current_render_target(RID());
	VSG::rasterizer->blit_render_target_to_screen(p_render_target, screen_rect, 0);
};

void MobileInterface::process(float p_delta_time) {
	_THREAD_SAFE_METHOD_

	if (hmd_tracker != NULL) {
		hmd_tracker->set_position_from_sensors(p_delta_time);
	};
};

MobileInterface::MobileInterface() {
	hmd_tracker = NULL;

	// Just set some defaults for these. At some point we need to look at adding a lookup table for common device + headset combos and/or support reading cardboard QR codes
	intraocular_dist = 6.0;
	display_width = 13.0;
	display_to_lens = 4.0;
	oversample = 1.0;
	k1 = 0.1;
	k2 = 0.1;
};
