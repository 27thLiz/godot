/* openvr.h */
#ifndef OPENVR_H
#define OPENVR_H

#include "reference.h"
#include "scene/main/node.h"
#include "scene/3d/spatial.h"
#include "scene/3d/camera.h"
#include "scene/main/viewport.h"
#include "scene/gui/control.h"
#include "drivers/openvr/headers/openvr.h"
#include "drivers/gles2/rasterizer_gles2.h"

class Openvr : public Node {
	OBJ_TYPE(Openvr,Node);
	
private:
	
	Camera *cam_l, *cam_r;
	Viewport *vp_l, *vp_r;
	Control *c1, *c2;
	Spatial *spatial;
	
	vr::IVRSystem *hmd;
	vr::IVRRenderModels *render_models;
	vr::TrackedDevicePose_t tracked_device_pose[ vr::k_unMaxTrackedDeviceCount ];
	Transform device_transforms[vr::k_unMaxTrackedDeviceCount];
	Transform hmd_transform;
	String driver;
	String display;
	
	bool inited;
	int m_iTrackedControllerCount;
	int m_iTrackedControllerCount_Last;
	int m_iValidPoseCount;
	int m_iValidPoseCount_Last;
	uint32_t render_width, render_height;
	
	RasterizerGLES2 *rasterizer;
	
	bool init_compositor();
	void update_hmd_pose();
	void process();
	
	Transform hmd_matrix_to_transform(const vr::HmdMatrix34_t &mat_pose);
protected:
	static void _bind_methods();
	virtual void _notification(int p_notification);
	
public:
	
	bool init();
	Transform get_hmd_transform();
	Openvr();
	~Openvr();
};

#endif
