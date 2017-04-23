#include "arvr_script_interface.h"

ArVrScriptInterface::ArVrScriptInterface() {
}

StringName ArVrScriptInterface::get_name() const {
	if (get_script_instance() && get_script_instance()->has_method("get_name")) {
		return get_script_instance()->call("get_name");
	} else {
		// just return something for now
		return "ARVR Script interface";
	}
}

bool ArVrScriptInterface::is_installed() {
	ERR_FAIL_COND_V(!(get_script_instance() && get_script_instance()->has_method("is_installed")), false);
	return get_script_instance()->call("is_installed");
}

bool ArVrScriptInterface::hmd_is_present() {
	ERR_FAIL_COND_V(!(get_script_instance() && get_script_instance()->has_method("hmd_is_present")), false);
	return get_script_instance()->call("hmd_is_present");
}

bool ArVrScriptInterface::supports_hmd() {
	ERR_FAIL_COND_V(!(get_script_instance() && get_script_instance()->has_method("supports_hmd")), false);
	return get_script_instance()->call("supports_hmd");
}

bool ArVrScriptInterface::is_stereo() {
	ERR_FAIL_COND_V(!(get_script_instance() && get_script_instance()->has_method("is_stereo")), false);
	return get_script_instance()->call("is_stereo");
}

bool ArVrScriptInterface::is_initialized() {
	return false;
}

bool ArVrScriptInterface::initialize() {
	return false;
}

void ArVrScriptInterface::uninitialize() {
}

Size2 ArVrScriptInterface::get_recommended_render_targetsize() {
	ERR_FAIL_COND_V(!(get_script_instance() && get_script_instance()->has_method("get_recommended_render_targetsize")), Size2());
	return get_script_instance()->call("get_recommended_render_targetsize");
}

Transform ArVrScriptInterface::get_transform_for_eye(Eyes p_eye, const Transform &p_cam_transform) {
	ERR_FAIL_COND_V(!(get_script_instance() && get_script_instance()->has_method("get_transform_for_eye")), Transform());
	return get_script_instance()->call("get_transform_for_eye", p_eye, p_cam_transform);
}

CameraMatrix ArVrScriptInterface::get_projection_for_eye(Eyes p_eye, real_t p_aspect, real_t p_z_near, real_t p_z_far) {
	ERR_FAIL_COND_V(!(get_script_instance() && get_script_instance()->has_method("get_projection_for_eye")), CameraMatrix());
	//@TODO CameraMatrix is unknown in script-world
	//return get_script_instance()->call("get_projection_for_eye", p_eye, p_aspect, p_z_near, p_z_far);
	return CameraMatrix();
}

void ArVrScriptInterface::commit_for_eye(ArVrInterface::Eyes p_eye, RID p_render_target) {
	ERR_FAIL_COND(!(get_script_instance() && get_script_instance()->has_method("commit_for_eye")));
	get_script_instance()->call("commit_for_eye");
}

void ArVrScriptInterface::process(float p_delta_time) {
	ERR_FAIL_COND(!(get_script_instance() && get_script_instance()->has_method("process")));
	get_script_instance()->call("process");
}

void ArVrScriptInterface::_bind_methods() {
	//ClassDB::add_virtual_method(get_class_static(), MethodInfo(Variant::BOOL, "is_installed"));
}
