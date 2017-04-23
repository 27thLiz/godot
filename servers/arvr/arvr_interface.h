/*************************************************************************/
/*  arvr_interface.h                                                     */
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
#ifndef ARVR_INTERFACE_H
#define ARVR_INTERFACE_H

#include "core/math/camera_matrix.h"
#include "os/thread_safe.h"
#include "scene/main/viewport.h"
#include "servers/arvr_server.h"

/**
	@author Bastiaan Olij <mux213@gmail.com>

	The ARVR interface is a template class ontop of which we build interface to differt AR, VR and tracking SDKs. 
	The idea is that we subclass this class, implement the logic, and then instantiate a singleton of each interface
	when Godot starts. These instances do not initialize themselves but register themselves with the AR/VR server.

	If the user wants to enable AR/VR the choose the interface they want to use and initialize it.

	Note that we may make this into a fully instantiable class for GDNative support.
*/

class ArVrInterface : public Reference {
	GDCLASS(ArVrInterface, Reference);

protected:
	_THREAD_SAFE_CLASS_

	ArVrServer *arvr_server;

	static void _bind_methods();

public:
	enum Eyes {
		EYE_MONO,
		EYE_LEFT,
		EYE_RIGHT
	};

	virtual StringName get_name() const;

	bool is_primary();
	void set_is_primary(bool p_is_primary);

	virtual bool is_installed() = 0; /* returns true if the middle ware related to this interface has been installed */
	virtual bool hmd_is_present() = 0; /* returns true if our HMD is connected */
	virtual bool supports_hmd() = 0; /* returns true is this interface handles output to an HMD or only handles positioning */

	virtual bool is_initialized(); /* returns true if we've initialized this interface */
	virtual bool initialize(); /* initialize this interface, if this has an HMD it becomes the primary interface */
	virtual void uninitialize(); /* deinitialize this interface */

	virtual Size2 get_recommended_render_targetsize() = 0; /* returns the recommended render target size per eye for this device */
	virtual bool is_stereo() = 0; /* returns true if this interface requires stereo rendering (for VR HMDs) or mono rendering (for mobile AR) */
	virtual Transform get_transform_for_eye(ArVrInterface::Eyes p_eye, const Transform &p_cam_transform) = 0; /* get each eyes camera transform */
	virtual CameraMatrix get_projection_for_eye(ArVrInterface::Eyes p_eye, real_t p_aspect, real_t p_z_near, real_t p_z_far) = 0; /* get each eyes projection matrix */
	virtual void commit_for_eye(ArVrInterface::Eyes p_eye, RID p_render_target) = 0; /* output the left or right eye */

	///@TODO change this to become thread safe and call this from the render loop
	virtual void process(float p_delta_time) = 0;

	ArVrInterface();
	virtual ~ArVrInterface();
};

VARIANT_ENUM_CAST(ArVrInterface::Eyes);

#endif
