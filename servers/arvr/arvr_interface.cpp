/*************************************************************************/
/*  arvr_interface.cpp                                                   */
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
#include "arvr_interface.h"

void ArVrInterface::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_name"), &ArVrInterface::get_name);

	ClassDB::bind_method(D_METHOD("is_primary"), &ArVrInterface::is_primary);
	ClassDB::bind_method(D_METHOD("set_is_primary", "enable"), &ArVrInterface::set_is_primary);

	ClassDB::bind_method(D_METHOD("is_installed"), &ArVrInterface::is_installed);
	ClassDB::bind_method(D_METHOD("supports_hmd"), &ArVrInterface::supports_hmd);
	ClassDB::bind_method(D_METHOD("hmd_is_present"), &ArVrInterface::hmd_is_present);
	ClassDB::bind_method(D_METHOD("is_initialized"), &ArVrInterface::is_initialized);
	ClassDB::bind_method(D_METHOD("initialize"), &ArVrInterface::initialize);
	ClassDB::bind_method(D_METHOD("uninitialize"), &ArVrInterface::uninitialize);

	ClassDB::bind_method(D_METHOD("get_recommended_render_targetsize"), &ArVrInterface::get_recommended_render_targetsize);

	//	These are now purely used internally, we may expose them again if we expose CameraMatrix through Variant but reduz is not a fan for good reasons :)
	//	ClassDB::bind_method(D_METHOD("get_transform_for_eye", "eye", "cam_transform"), &ArVrInterface::get_transform_for_eye);
	//	ClassDB::bind_method(D_METHOD("get_projection_for_eye", "eye"), &ArVrInterface::get_projection_for_eye);

	ClassDB::bind_method(D_METHOD("handles_output"), &ArVrInterface::handles_output);
	//	ClassDB::bind_method(D_METHOD("commit_viewport", "node:viewport"), &ArVrInterface::commit_viewport);

	///@TODO once this is automatic, we can remove this
	ClassDB::bind_method(D_METHOD("process"), &ArVrInterface::process);

	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "primary"), "set_is_primary", "is_primary");

	BIND_CONSTANT(EYE_LEFT);
	BIND_CONSTANT(EYE_RIGHT);
};

void ArVrInterface::set_name(const String p_name) {
	name = p_name;
};

StringName ArVrInterface::get_name() const {
	return name;
};

bool ArVrInterface::is_primary() {
	return arvr_server->get_primary_interface() == this;
};

void ArVrInterface::set_is_primary(bool p_is_primary) {
	if (p_is_primary) {
		ERR_FAIL_COND(!is_initialized());
		ERR_FAIL_COND(!supports_hmd());

		arvr_server->set_primary_interface(this);
	} else {
		arvr_server->clear_primary_interface_if(this);
	};
};

bool ArVrInterface::is_initialized() {
	// must be implemented in subclass
	return false;
};

bool ArVrInterface::initialize() {
	// must be implemented in subclass
	return false;
};

void ArVrInterface::ArVrInterface::uninitialize(){
	// must be implemented in subclass
};

ArVrInterface::ArVrInterface() {
	// this should be changed by the subclass
	name = "Default";

	// grab a link to our arvr server singleton for convinience
	arvr_server = (ArVrServer *)ArVrServer::get_singleton();

	// register our interface (to early... need to think of a way to allow this)
	// arvr_server->add_interface(this);
};

ArVrInterface::~ArVrInterface() {
	// Just in case, if we are the primary interface, clear us. This should also be called when uninitialize is called but we're playing it safe
	ArVrServer::get_singleton()->clear_primary_interface_if(this);

	// now remove it:
	arvr_server->remove_interface(this);

	// and make sure we cleanup if we haven't already
	if (is_initialized()) {
		uninitialize();
	};
};
