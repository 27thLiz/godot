/*************************************************************************/
/*  arvr_server.h                                                        */
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

#ifndef ARVR_SERVER_H
#define ARVR_SERVER_H

#include "os/thread_safe.h"
#include "reference.h"
#include "rid.h"
#include "variant.h"

class ArVrInterface;
class ArVrPositionalTracker;

/**
	@author Bastiaan Olij <mux213@gmail.com>

	The ARVR server is a singleton object that gives access to the various
	objects and SDKs that are available on the system.
	Because there can be multiple SDKs active this is exposed as an array
	and our ARVR server object acts as a pass through
	Also each positioning tracker is accessible from here.

	@TODO: add reference frame and null position support
*/
class ArVrServer : public Object {
	GDCLASS(ArVrServer, Object);
	_THREAD_SAFE_CLASS_

private:
	Vector<Ref<ArVrInterface> > interfaces;
	Vector<ArVrPositionalTracker *> trackers;

	Ref<ArVrInterface> primary_interface; /* we'll identify one interface as primary, this will be used by our viewports */

	real_t world_scale; /* scale by which we multiply our tracker positions */

protected:
	static ArVrServer *singleton;

	static void _bind_methods();

public:
	enum TrackerType {
		TRACKER_HMD = 0x01, /* HMD in the broad sense, mobile device position is typed like this too */
		TRACKER_CONTROLLER = 0x02, /* tracks a controller */
		TRACKER_BASESTATION = 0x04, /* tracks location of a base station */
		TRACKER_UNKNOWN = 0x80, /* unknown tracker */

		TRACKER_HMD_AND_CONTROLLER = 0x03, /* common to select both of these */
		TRACKER_ANY_KNOWN = 0x7f, /* all except unknown */
		TRACKER_ANY = 0xff /* used by get_connected_trackers to return all types */
	};

	static ArVrServer *get_singleton();

	real_t get_world_scale();
	void set_world_scale(real_t p_world_scale);

	void add_interface(const Ref<ArVrInterface> &p_interface);
	void remove_interface(const Ref<ArVrInterface> &p_interface);
	int get_interface_count() const;
	Ref<ArVrInterface> get_interface(int p_index) const;
	Ref<ArVrInterface> find_interface(const String &p_name) const;

	/*
		note, more then one interface can technically be active, especially on mobile, but only one interface is used for
		rendering. This interface identifies itself by calling set_primary_interface when it is initialized
	*/
	Ref<ArVrInterface> get_primary_interface() const;
	void set_primary_interface(const Ref<ArVrInterface> &p_primary_interface);
	void clear_primary_interface_if(const Ref<ArVrInterface> &p_primary_interface); /* this is automatically called if an interface destructs */

	void add_tracker(ArVrPositionalTracker *p_tracker);
	void remove_tracker(ArVrPositionalTracker *p_tracker);
	int get_tracker_count() const;
	ArVrPositionalTracker *get_tracker(int p_index) const;

	ArVrServer();
	~ArVrServer();
};

#define ARVR ArVrServer

VARIANT_ENUM_CAST(ArVrServer::TrackerType);

#endif
