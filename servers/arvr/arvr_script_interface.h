#ifndef SCRIPT_INTERFACE_H
#define SCRIPT_INTERFACE_H

#include "arvr_interface.h"

class ArVrScriptInterface : public ArVrInterface {
	GDCLASS(ArVrScriptInterface, ArVrInterface);

protected:
	static void _bind_methods();

public:
	ArVrScriptInterface();

	virtual StringName get_name() const;

	virtual bool is_installed();
	virtual bool hmd_is_present();
	virtual bool supports_hmd();

	virtual bool is_initialized();
	virtual bool initialize();
	virtual void uninitialize();

	virtual Size2 get_recommended_render_targetsize();
	virtual bool is_stereo();
	virtual Transform get_transform_for_eye(ArVrInterface::Eyes p_eye, const Transform &p_cam_transform);
	virtual CameraMatrix get_projection_for_eye(ArVrInterface::Eyes p_eye, real_t p_aspect, real_t p_z_near, real_t p_z_far);
	virtual void commit_for_eye(ArVrInterface::Eyes p_eye, RID p_render_target);

	virtual void process(float p_delta_time);
};

#endif // SCRIPT_INTERFACE_H
