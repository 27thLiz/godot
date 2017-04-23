/*************************************************************************/
/*  mobile_interface.h                                                   */
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
#ifndef MOBILE_INTERFACE_H
#define MOBILE_INTERFACE_H

#include "servers/arvr/arvr_interface.h"
#include "servers/arvr/arvr_positional_tracker.h"

/*
	The mobile interface is a native VR interface that can be used on Android and iOS phones.
	It contains a basic implementation supporting 3DOF tracking if a gyroscope and accelerometer are
	present and sets up the proper projection matrices based on the values provided.

	We're planning to eventually do separate interfaces towards mobile SDKs that have far more capabilities and
	do not rely on the user providing most of these settings (though enhancing this with auto detection features
	based on the device we're running on would be cool). I'm mostly adding this as an example or base plate for
	more advanced interfaces. 

	@TODO: Currently this relies on the lens distortion being applied within Godot when the viewport is rendered through a sprite
*/

class MobileInterface : public ArVrInterface {
	GDCLASS(MobileInterface, ArVrInterface);

private:
	ArVrPositionalTracker *hmd_tracker;

	real_t intraocular_dist;
	real_t display_width;
	real_t display_to_lens;
	real_t oversample;

	//@TODO not yet used, these are needed in our distortion shader...
	real_t k1;
	real_t k2;

protected:
	static void _bind_methods();

public:
	virtual bool is_installed();
	virtual bool supports_hmd();
	virtual bool hmd_is_present();

	virtual bool is_initialized();
	virtual bool initialize();
	virtual void uninitialize();

	virtual Size2 get_recommended_render_targetsize();
	virtual Transform get_transform_for_eye(ArVrInterface::Eyes p_eye, const Transform &p_cam_transform);
	virtual CameraMatrix get_projection_for_eye(ArVrInterface::Eyes p_eye, real_t p_aspect, real_t p_z_near, real_t p_z_far);

	virtual bool handles_output();
	virtual void commit_viewport(RID p_render_target_texture);

	virtual void process(float p_delta_time);

	MobileInterface();
};

#endif