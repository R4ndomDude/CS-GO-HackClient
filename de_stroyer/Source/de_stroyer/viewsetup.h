#pragma once

class CViewSetup
{
public:
	// left side of view window
	int			x, x_old;
	// top side of view window
	int			y, y_old;
	// width of view window
	int			width, width_old;
	// height of view window
	int			height, height_old;
	// the rest are only used by 3D views
	// Orthographic projection?
	bool		m_bOrtho;
	// View-space rectangle for ortho projection.
	float		m_OrthoLeft;
	float		m_OrthoTop;
	float		m_OrthoRight;
	float		m_OrthoBottom;
	bool		m_bCustomViewMatrix; // 0x34
	matrix3x4_t	m_matCustomViewMatrix; // 0x38
	char		pad_0x68[0x48]; // 0x68
	// horizontal FOV in degrees
	float		fov; //	0xB0
	// horizontal FOV in degrees for in-view model
	float		fovViewmodel;
	// 3D origin of camera
	Vector		origin;
	// heading of camera (pitch, yaw, roll)
	Angle		angles;
	// local Z coordinate of near plane of camera
	float		zNear;
	// local Z coordinate of far plane of camera
	float		zFar;
	// local Z coordinate of near plane of camera ( when rendering view model )
	float		zNearViewmodel;
	// local Z coordinate of far plane of camera ( when rendering view model )
	float		zFarViewmodel;
	// The aspect ratio to use for computing the perspective projection matrix
	// (0.0f means use the viewport)
	float		m_flAspectRatio;
	// Camera settings to control depth of field
	float		m_flNearBlurDepth;
	float		m_flNearFocusDepth;
	float		m_flFarFocusDepth;
	float		m_flFarBlurDepth;
	float		m_flNearBlurRadius;
	float		m_flFarBlurRadius;
	int			m_nDoFQuality;
	// Camera settings to control motion blur
	int			m_nMotionBlurMode;
	float		m_flShutterTime;				// In seconds
	Vector		m_vShutterOpenPosition;			// Start of frame or "shutter open"
	Angle		m_shutterOpenAngles;			//
	Vector		m_vShutterClosePosition;		// End of frame or "shutter close"
	Angle		m_shutterCloseAngles;			// 
	// Controls for off-center projection (needed for poster rendering)
	float		m_flOffCenterTop;
	float		m_flOffCenterBottom;
	float		m_flOffCenterLeft;
	float		m_flOffCenterRight;
	int			m_EdgeBlur;
};