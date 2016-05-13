
#include "openvr.h"
#include "core/os/os.h"
#include "platform/windows/os_windows.h"


String GetTrackedDeviceString( vr::IVRSystem *pHmd, vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop, vr::TrackedPropertyError *peError = NULL )
{
	uint32_t unRequiredBufferLen = pHmd->GetStringTrackedDeviceProperty( unDevice, prop, NULL, 0, peError );
	if( unRequiredBufferLen == 0 )
		return "";

	char *pchBuffer = new char[ unRequiredBufferLen ];
	unRequiredBufferLen = pHmd->GetStringTrackedDeviceProperty( unDevice, prop, pchBuffer, unRequiredBufferLen, peError );
	String sResult = pchBuffer;
	delete [] pchBuffer;
	return sResult;
}

Transform Openvr::hmd_matrix_to_transform(const vr::HmdMatrix34_t &mat_pose) {
	Transform ret;
	ret.set(mat_pose.m[0][0], mat_pose.m[1][0], mat_pose.m[2][0],
			mat_pose.m[0][1], mat_pose.m[1][1], mat_pose.m[2][1],
			mat_pose.m[0][2], mat_pose.m[1][2], mat_pose.m[2][2],
			mat_pose.m[0][3], mat_pose.m[1][3], mat_pose.m[2][3]);
	return ret;
}

void Openvr::update_hmd_pose() {
	if ( !hmd )
		return;
	
		vr::VRCompositor()->WaitGetPoses(tracked_device_pose, vr::k_unMaxTrackedDeviceCount, NULL, 0 );
		
		m_iValidPoseCount = 0;
			//m_strPoseClasses = "";
			for ( int nDevice = 0; nDevice < vr::k_unMaxTrackedDeviceCount; ++nDevice )
			{
				if ( tracked_device_pose[nDevice].bPoseIsValid )
				{
					m_iValidPoseCount++;
					device_transforms[nDevice] = hmd_matrix_to_transform(tracked_device_pose[nDevice].mDeviceToAbsoluteTracking);
//					if (m_rDevClassChar[nDevice]==0)
//					{
//						switch (m_pHMD->GetTrackedDeviceClass(nDevice))
//						{
//						case vr::TrackedDeviceClass_Controller:        m_rDevClassChar[nDevice] = 'C'; break;
//						case vr::TrackedDeviceClass_HMD:               m_rDevClassChar[nDevice] = 'H'; break;
//						case vr::TrackedDeviceClass_Invalid:           m_rDevClassChar[nDevice] = 'I'; break;
//						case vr::TrackedDeviceClass_Other:             m_rDevClassChar[nDevice] = 'O'; break;
//						case vr::TrackedDeviceClass_TrackingReference: m_rDevClassChar[nDevice] = 'T'; break;
//						default:                                       m_rDevClassChar[nDevice] = '?'; break;
//						}
//					}
//					m_strPoseClasses += m_rDevClassChar[nDevice];
				}
			}
		
			if ( tracked_device_pose[vr::k_unTrackedDeviceIndex_Hmd].bPoseIsValid )
			{
				hmd_transform = device_transforms[vr::k_unTrackedDeviceIndex_Hmd].inverse();
				//print_line(String("hmd transform is: ") + hmd_transform);
			}
}

bool Openvr::init() {

	if (inited) return false;
	printf("running\n");
	set_process(true);
	vr::EVRInitError err = vr::VRInitError_None;
	hmd = vr::VR_Init(&err, vr::VRApplication_Scene);
	print_line(String("VR init returned: ") + vr::VR_GetVRInitErrorAsEnglishDescription(err));
	render_models = (vr::IVRRenderModels *)vr::VR_GetGenericInterface( vr::IVRRenderModels_Version, &err );
	if( !render_models )
	{
		hmd = NULL;
		vr::VR_Shutdown();
		
		char buf[1024];
		print_line(String("Unable to get render model interface: %s") + vr::VR_GetVRInitErrorAsEnglishDescription( err ) );
		return false;
	}
	driver = "No Driver";
	display = "No Display";

	
	driver = GetTrackedDeviceString( hmd, vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_TrackingSystemName_String );
	display = GetTrackedDeviceString( hmd, vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_SerialNumber_String );
	print_line(String("Display: ") + display);
	print_line(String("Driver: ") + driver);
	if (!init_compositor()) return false;
	
	hmd->GetRecommendedRenderTargetSize(&render_width, &render_height);
	print_line(String("recommended render target x: ") + itos(render_width) + " y: " + itos(render_height));
	OS::get_singleton()->set_window_size(Size2(render_width*2, render_height));
	inited = true;
	return true;
}

Transform Openvr::get_hmd_transform() {
	return hmd_transform;
}

void Openvr::_notification(int p_notification) {
	switch (p_notification) {
	case NOTIFICATION_ENTER_TREE:
		if (!get_tree()->is_editor_hint()) {
			init();
			
			c1 = memnew(Control);
			c2 = memnew(Control);
			vp_l = memnew(Viewport);
			vp_r = memnew(Viewport);
			
			spatial =memnew(Spatial);
			cam_l = memnew(Camera);
			cam_r = memnew(Camera);
			cam_l->set_name("Cam Left");
			cam_r->set_name("Cam Right");			
			//c1->set_size(Size2(render_width, render_height));
			//c2->set_size(Size2(render_width, render_height));
			//c2->set_pos(Point2(render_width, 0));
			vp_l->set_name("VP Left");
			vp_r->set_name("VP Right");
			vp_l->set_as_render_target(true);
			vp_r->set_as_render_target(true);
			vp_l->set_render_target_to_screen_rect(Rect2(0, 0, render_width, render_height));
			vp_r->set_render_target_to_screen_rect(Rect2(render_width, 0, render_width, render_height));
			vp_l->set_render_target_filter(false);
			vp_l->set_rect(Rect2(0, 0, render_width, render_height));
			vp_r->set_rect(Rect2(render_width, 0, render_width, render_height));
			spatial->set_name("pos");
			add_child(spatial);
			//spatial->add_child(c1);
			//spatial->add_child(c2);
			//c1->add_child(vp_l);
			//c2->add_child(vp_r);
			spatial->add_child(vp_l);
			spatial->add_child(vp_r);
			vp_l->add_child(cam_l);
			vp_r->add_child(cam_r);
			OS_Windows* os = (OS_Windows*)OS::get_singleton();
			rasterizer = (RasterizerGLES2*)os->get_rasterizer();
		}
		break;
	case NOTIFICATION_PROCESS:
		process();
		break;
	case NOTIFICATION_READY:
		
		break;
	default:
		break;
	}
}

void Openvr::process() {
	//Get render target textures
	RenderTargetTexture *tex_l = vp_l->get_render_target_texture().ptr();
	GLuint left_tex = rasterizer->_texture_get_name(tex_l->get_rid());
	
	RenderTargetTexture *tex_r = vp_r->get_render_target_texture().ptr();
	GLuint right_tex = rasterizer->_texture_get_name(tex_r->get_rid());
	
	vr::Texture_t leftEyeTexture = {(void*)&left_tex, vr::API_OpenGL, vr::ColorSpace_Auto };
	vr::EVRCompositorError err = vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture );
	print_line(String("Submit returned ") + itos(err));
	
	vr::Texture_t rightEyeTexture = {(void*)&right_tex, vr::API_OpenGL, vr::ColorSpace_Auto };
	vr::VRCompositor()->Submit(vr::Eye_Left, &rightEyeTexture );
	
	update_hmd_pose();
}

bool Openvr::init_compositor() {
	//vr::EVRInitError peError = vr::VRInitError_None;

	if ( !vr::VRCompositor() )
	{
		print_line( String("VR Compositor initialization failed. See log file for details\n"));
		return false;
	}

	return true;
}

void Openvr::_bind_methods() {

	ObjectTypeDB::bind_method("init",&Openvr::init);
	ObjectTypeDB::bind_method("get_hmd_transform", &Openvr::get_hmd_transform);
}

Openvr::Openvr() {
	hmd = nullptr;
	inited = false;
	hmd_transform = Transform();
}
Openvr::~Openvr() {
	printf("shutting down openvr\n");
	vr::VR_Shutdown();
}
