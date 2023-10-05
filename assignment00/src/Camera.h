#include "Matrix3f.h"
#include "Matrix4f.h"
#include <iostream>

class Camera
{
private:
	
	double camX = 0.0;
	double camY = 0.0;
	double camZ = 0.0;
	double targetX = 0.0;
	double targetY = 0.0;
	double targetZ = 0.0;
	double upDirX = 0.0;
	double upDirY = 0.0;
	double upDirZ = 0.0;
	double* camDirVec = nullptr;
	double fieldOfView = 50.0;
	double aspectRatio = 1.0;
	double camNear = 1.0;
	double camFar = 100.0;
	const double pi = 3.14159265359;
	double* worldAngles = nullptr;
	
	

public:

	Camera();
	Camera(double camX, double camY, double camZ, double targetX, double targetY, double targetZ, double upDirX, double upDirY, double upDirZ);
	Camera(const Matrix3f& camData);
	~Camera();

	void CalcDirVec();
	void CalcWorldAngles();
	double* CrLookAtArr();
	double GetFOV();
	double GetAspectRatio();
	double GetNear();
	double GetFar();
	Vector4f GetCamPosVec();  
	Vector4f GetTargetPosVec();
	Vector4f GetUpVec();
	Vector4f GetBaseVec();
	Vector4f GetCamDirVec();

	void SetFOV(double fov);
	void SetAspectRatio(double asprat);
	void SetNear();
	void SetFar();
	void SetNewCamPos(const Vector4f& cam, const Vector4f& target);
	void SetUpVector(const Vector4f& upvec);

	void PrimitiveZoom(double zoom);	// zoom from camera to target through axis Z
	void PrimitiveMove(double movementX, double movementY);		// move camera and target together on screen plane
	void PrimitiveRotation(double rotX, double rotY, double rotZ);

	void applyCamRotation(double rotX, double rotY, double rotZ);
	// rotate around the center of the object
	// half a screen = 90 degrees of rotation
	// rotation over axis X stops when reaching 90 degrees
	// no rotation over axis Z
	void applyCamRotation(double rotX, double rotY, double rotZ, const Vector4f& camStartPos, const Vector4f& targetStartPos);
};