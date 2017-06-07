/*************************************************************************/
/*  arvr_server.cpp                                                      */
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
#include "arvr_server.h"
#include "arvr/arvr_interface.h"
#include "arvr/arvr_positional_tracker.h"
#include "global_config.h"

ArVrServer *ArVrServer::singleton = NULL;

ArVrServer *ArVrServer::get_singleton() {
	return singleton;
};

void ArVrServer::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_world_scale"), &ArVrServer::get_world_scale);
	ClassDB::bind_method(D_METHOD("set_world_scale"), &ArVrServer::set_world_scale);

	ADD_PROPERTY(PropertyInfo(Variant::REAL, "world_scale"), "set_world_scale", "get_world_scale");

	ClassDB::bind_method(D_METHOD("get_interface_count"), &ArVrServer::get_interface_count);
	ClassDB::bind_method(D_METHOD("get_interface:ArVrInterface", "idx"), &ArVrServer::get_interface);
	ClassDB::bind_method(D_METHOD("find_interface:ArVrInterface", "name"), &ArVrServer::find_interface);
	ClassDB::bind_method(D_METHOD("get_tracker_count"), &ArVrServer::get_tracker_count);
	ClassDB::bind_method(D_METHOD("get_tracker:ArVrPositionalTracker", "idx"), &ArVrServer::get_tracker);

	ClassDB::bind_method(D_METHOD("set_primary_interface"), &ArVrServer::set_primary_interface);

	ClassDB::bind_method(D_METHOD("add_interface"), &ArVrServer::add_interface);
	ClassDB::bind_method(D_METHOD("remove_interface"), &ArVrServer::remove_interface);

	BIND_CONSTANT(TRACKER_HMD);
	BIND_CONSTANT(TRACKER_CONTROLLER);
	BIND_CONSTANT(TRACKER_BASESTATION);
	BIND_CONSTANT(TRACKER_UNKNOWN);
	BIND_CONSTANT(TRACKER_HMD_AND_CONTROLLER);
	BIND_CONSTANT(TRACKER_ANY_KNOWN);
	BIND_CONSTANT(TRACKER_ANY);

	ADD_SIGNAL(MethodInfo("interface_added", PropertyInfo(Variant::STRING, "name")));
	ADD_SIGNAL(MethodInfo("interface_removed", PropertyInfo(Variant::STRING, "name")));

	ADD_SIGNAL(MethodInfo("tracker_added", PropertyInfo(Variant::STRING, "name"), PropertyInfo(Variant::INT, "type")));
	ADD_SIGNAL(MethodInfo("tracker_removed", PropertyInfo(Variant::STRING, "name")));
};

real_t ArVrServer::get_world_scale() {
	return world_scale;
};

void ArVrServer::set_world_scale(real_t p_world_scale) {
	if (world_scale < 0.01) {
		world_scale = 0.01;
	} else if (world_scale > 1000.0) {
		world_scale = 1000.0;
	};

	world_scale = p_world_scale;
};

void ArVrServer::add_interface(const Ref<ArVrInterface> &p_interface) {
	ERR_FAIL_COND(p_interface.is_null());

	///@TODO check if we already have this interface and fail

	interfaces.push_back(p_interface);
	emit_signal("interface_added", p_interface->get_name());
};

void ArVrServer::remove_interface(const Ref<ArVrInterface> &p_interface) {
	ERR_FAIL_COND(p_interface.is_null());

	int idx = -1;
	for (int i = 0; i < interfaces.size(); i++) {

		if (interfaces[i] == p_interface) {

			idx = i;
			break;
		};
	};

	ERR_FAIL_COND(idx == -1);

	emit_signal("interface_removed", p_interface->get_name());
	interfaces.remove(idx);
};

int ArVrServer::get_interface_count() const {
	return interfaces.size();
};

Ref<ArVrInterface> ArVrServer::get_interface(int p_index) const {
	ERR_FAIL_INDEX_V(p_index, interfaces.size(), NULL);

	return interfaces[p_index];
};

Ref<ArVrInterface> ArVrServer::find_interface(const String &p_name) const {
	int idx = -1;
	for (int i = 0; i < interfaces.size(); i++) {

		if (interfaces[i]->get_name() == p_name) {

			idx = i;
			break;
		};
	};

	ERR_FAIL_COND_V(idx == -1, NULL);

	return interfaces[idx];
};

void ArVrServer::add_tracker(ArVrPositionalTracker *p_tracker) {
	ERR_FAIL_NULL(p_tracker);

	trackers.push_back(p_tracker);
	emit_signal("tracker_added", p_tracker->get_name(), p_tracker->get_type());
};

void ArVrServer::remove_tracker(ArVrPositionalTracker *p_tracker) {
	ERR_FAIL_NULL(p_tracker);

	int idx = -1;
	for (int i = 0; i < trackers.size(); i++) {

		if (trackers[i] == p_tracker) {

			idx = i;
			break;
		};
	};

	ERR_FAIL_COND(idx == -1);

	emit_signal("tracker_removed", p_tracker->get_name());
	trackers.remove(idx);
};

int ArVrServer::get_tracker_count() const {
	return trackers.size();
};

ArVrPositionalTracker *ArVrServer::get_tracker(int p_index) const {
	ERR_FAIL_INDEX_V(p_index, trackers.size(), NULL);

	return trackers[p_index];
};

Ref<ArVrInterface> ArVrServer::get_primary_interface() const {
	return primary_interface;
};

void ArVrServer::set_primary_interface(const Ref<ArVrInterface> &p_primary_interface) {
	primary_interface = p_primary_interface;
};

void ArVrServer::clear_primary_interface_if(const Ref<ArVrInterface> &p_primary_interface) {
	if (primary_interface == p_primary_interface) {
		primary_interface.unref();
	};
};

ArVrServer::ArVrServer() {
	singleton = this;
	world_scale = 1.0;
};

ArVrServer::~ArVrServer() {
	singleton = NULL;
};
