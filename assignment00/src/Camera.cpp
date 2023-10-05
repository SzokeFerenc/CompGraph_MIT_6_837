#include "Camera.h"
#include "Vector3f.h"
#include "Vector4f.h"

Camera::Camera()
{
	this->camX = 0.0;
	this->camY = 0.0;
	this->camZ = 5.0;
	this->targetX = 0.0;
	this->targetY = 0.0;
	this->targetY = 0.0;
	this->upDirX = 0.0;
	this->upDirY = 1.0;
	this->upDirZ = 0.0;
}

Camera::Camera(
	double _camX, double _camY, double _camZ,
	double _targetX, double _targetY, double _targetZ,
	double _upDirX, double _upDirY, double _upDirZ) :
	camX(_camX), camY(_camY), camZ(_camZ),
	targetX(_targetX), targetY(_targetY), targetZ(_targetZ),
	upDirX(_upDirX), upDirY(_upDirY), upDirZ(_upDirZ)
{

}

Camera::Camera(const Matrix3f& camData) : 
	camX(camData.getRow(0)[0]), camY(camData.getRow(0)[1]), camZ(camData.getRow(0)[2]),
	targetX(camData.getRow(1)[0]), targetY(camData.getRow(1)[1]), targetZ(camData.getRow(1)[2]),
	upDirX(camData.getRow(2)[0]), upDirY(camData.getRow(2)[1]), upDirZ(camData.getRow(2)[2])
{

}

Camera::~Camera()
{
	if (camDirVec != nullptr)
	{
		std::cout << "camDirVec destroyed!" << std::endl;
		delete[] camDirVec;
	}
	if (worldAngles != nullptr)
	{
		std::cout << "worldAngles destroyed!" << std::endl;
		delete[] worldAngles;
	}
}

void Camera::CalcDirVec()
{
	if (this->camDirVec == nullptr)
	{
		double* result = new double[3];
		result[0] = this->camX - this->targetX;
		result[1] = this->camY - this->targetY;
		result[2] = this->camZ - this->targetZ;
		this->camDirVec = result;
	}
	else
	{
		this->camDirVec[0] = this->camX - this->targetX;
		this->camDirVec[1] = this->camY - this->targetY;
		this->camDirVec[2] = this->camZ - this->targetZ;
	}
}

void Camera::CalcWorldAngles()
{
	if (this->worldAngles == nullptr)
	{
		double* result = new double[3];
		// angle around axis y
		result[0] = atan2((this->camX - this->targetX), (this->camZ - this->targetZ));
		// angle around axis x
		result[1] = atan2((this->camY - this->targetY), (this->camZ - this->targetZ));
		// angle around axis z
		if ((this->camX - this->targetX) != 0.0)
		{
			result[2] = atan2((this->camY - this->targetY), (this->camX - this->targetX));
		}
		else
			result[2] = 0.0;
		this->worldAngles = result;
	}
	else
	{
		// angle around axis y
		this->worldAngles[0] = atan2((this->camX - this->targetX), (this->camZ - this->targetZ));
		// angle around axis x
		this->worldAngles[1] = atan2((this->camY - this->targetY), (this->camZ - this->targetZ));
		// angle around axis z
		this->worldAngles[2] = atan2((this->camY - this->targetY), (this->camX - this->targetX));
	}
	std::cout << "angle axis y: " << this->worldAngles[0] << " angle axis x: " << this->worldAngles[1] << " angle axis z: " << this->worldAngles[2] << std::endl;
}

double* Camera::CrLookAtArr()
{
	double lookAt[9] = { 
	this->camX, this->camY, this->camZ,
	this->targetX, this->targetY, this->targetZ,
	this->upDirX, this->upDirY, this->upDirZ 
	};
	return lookAt;
}


double Camera::GetFOV()
{
	return this->fieldOfView;
}

double Camera::GetAspectRatio()
{
	return this->aspectRatio;
}

double Camera::GetNear()
{
	return this->camNear;
}

double Camera::GetFar()
{
	return this->camFar;
}

Vector4f Camera::GetCamPosVec()
{
	Vector4f result = {
		(float)this->camX, (float)this->camY, (float)this->camZ, 1.0
	};
	return result;
}

Vector4f Camera::GetTargetPosVec()
{
	Vector4f result = {
		(float)this->targetX, (float)this->targetY, (float)this->targetZ, 1.0
	};
	return result;
}

Vector4f Camera::GetUpVec()
{
	Vector4f result = {
		(float)this->upDirX, (float)this->upDirY, (float)this->upDirZ, 0.0
	};
	return result;
}

Vector4f Camera::GetBaseVec()
{
	double deltaX = this->camX - this->targetX;
	double deltaY = this->camX - this->targetX;
	double deltaZ = this->camX - this->targetX;
	Vector4f result = {	(float)deltaX, (float)deltaY, (float)deltaZ, 0.0 };
	return result;
}

Vector4f Camera::GetCamDirVec()
{
	this->CalcDirVec();
	return Vector4f((float)this->camDirVec[0], (float)this->camDirVec[1], (float)this->camDirVec[2], 0.0f );
}

void Camera::PrimitiveZoom(double zoom)
{
	// v1.0
	//this->camZ += zoom;
	//this->targetZ += zoom;

	// v2.0
	/*this->CalcDirVec();
	Vector4f moveVec = { (float)this->camDirVec[0], (float)this->camDirVec[1], (float)this->camDirVec[2], 0.0f };
	moveVec = moveVec * zoom;
	Vector4f cam = this->GetCamPosVec();
	Vector4f target = this->GetTargetPosVec();
	cam = cam + moveVec;
	target = target + moveVec;
	this->SetNewCamPos(cam, target);*/

	// v3.0
	this->CalcDirVec();
	Vector4f moveVec = { (float)this->camDirVec[0], (float)this->camDirVec[1], (float)this->camDirVec[2], 0.0f };
	moveVec = moveVec * zoom;
	Vector4f cam = this->GetCamPosVec();
	cam = cam + moveVec;
	Vector4f target = this->GetTargetPosVec();
	Vector4f sum = target - cam;
	if (sum.abs() >= 0.0001)
	{
		this->SetNewCamPos(cam, target);
	}
}

void Camera::PrimitiveMove(double movementX, double movementY)
{
	// v1.0
	/*this->camX -= movementX;
	this->camY += movementY;
	this->targetX -= movementX;
	this->targetY += movementY;*/

	// v2.0
	//this->CalcWorldAngles();
	//
	//Vector4f dirOX = { -1.0f, 0.0f, 0.0f, 0.0f };
	//Vector4f dirOY = { 0.0f, 1.0f, 0.0f, 0.0f };
	//Matrix4f rotMatX;
	//rotMatX = rotMatX.rotateX((float)this->worldAngles[1]);
	//Matrix4f rotMatY;
	//rotMatY = rotMatY.rotateY((float)this->worldAngles[0]);
	//Matrix4f rotMatZ;
	//rotMatZ = rotMatZ.rotateZ((float)this->worldAngles[2]);
	//Matrix4f rotMat;
	//rotMat = rotMatX * rotMatY * rotMatZ;
	////rotMat.print();
	//Vector4f movSideway = rotMat * dirOX * movementX;
	//Vector4f movUpway = rotMat * dirOY * movementY;
	//Vector4f cam = this->GetCamPosVec();
	//Vector4f target = this->GetTargetPosVec();
	//cam = cam + movSideway + movUpway;
	//target = target + movSideway + movUpway;
	//this->SetNewCamPos(cam, target);

	// v3.0
	this->CalcDirVec();
	
	Vector4f camViewVec = this->GetCamDirVec();
	Vector4f dirAhead = { camViewVec[0], 0.0f,  camViewVec[2], 0.0f };
	Vector4f dirSide = { -1.0f * camViewVec[2], 0.0f, camViewVec[0], 0.0f };
	float length = dirAhead.abs();
	//std::cout << "DirAhead length: " << length << std::endl;
	Vector4f dirUp = { 0.0f, length, 0.0f, 0.0f };
	dirSide = dirSide * movementX;
	dirUp = dirUp * movementY;
	Vector4f cam = this->GetCamPosVec();
	Vector4f target = this->GetTargetPosVec();
	cam = cam + dirSide + dirUp;
	target = target + dirSide + dirUp;
	this->SetNewCamPos(cam, target);

}

void Camera::PrimitiveRotation(double rotX, double rotY, double rotZ)
{
	this->CalcWorldAngles();
	this->CalcDirVec();
	Vector4f upvec = this->GetUpVec();
	Vector4f dirvec = this->GetCamDirVec();
	Matrix4f rotMatX;
	rotMatX = rotMatX.rotateX((float)this->worldAngles[1]*-1.0f);
	Matrix4f rotMatY;
	rotMatY = rotMatY.rotateY((float)this->worldAngles[0]*-1.0f);
	Matrix4f rotMatZ;
	rotMatZ = rotMatZ.rotateZ((float)this->worldAngles[2]*-1.0f);
	Matrix4f rotMat;
	rotMat = rotMatX * rotMatY * rotMatZ;
	//dirvec = rotMat * dirvec;
	//upvec = rotMat * upvec;
	Matrix4f rotMatXMov;
	rotMatXMov = rotMatXMov.rotateX((float)rotX);
	Matrix4f rotMatYMov;
	rotMatYMov = rotMatYMov.rotateY((float)rotY);
	Matrix4f rotMatZMov;
	rotMatZMov = rotMatZMov.rotateZ((float)rotZ);
	Matrix4f rotMatMov;
	rotMatMov = rotMatXMov * rotMatYMov * rotMatZMov;
	rotMatX = rotMatX.rotateX((float)this->worldAngles[1]);
	rotMatY = rotMatY.rotateY((float)this->worldAngles[0]);
	rotMatZ = rotMatZ.rotateZ((float)this->worldAngles[2]);
	rotMat = rotMatX * rotMatY * rotMatZ;
	rotMatMov = rotMatMov * rotMat;
	dirvec = rotMatMov * dirvec;
	Vector4f cam = this->GetTargetPosVec() + dirvec;
	Vector4f target = this->GetTargetPosVec();
	this->SetNewCamPos(cam, target);
}

void Camera::SetNewCamPos(const Vector4f& cam, const Vector4f& target)
{
	this->camX = cam[0];
	this->camY = cam[1];
	this->camZ = cam[2];
	this->targetX = target[0];
	this->targetY = target[1];
	this->targetZ = target[2];
}

void Camera::SetUpVector(const Vector4f& upvec)
{
	this->upDirX = upvec.x();
	this->upDirY = upvec.y();
	this->upDirZ = upvec.z();
}

void Camera::applyCamRotation(
	double rotX, double rotY, double rotZ)
{
	// rotation around center of object (have to calculate) now it's 0,0,0
	this->CalcDirVec();
	this->CalcWorldAngles();
	Vector4f camViewVec = this->GetCamDirVec();
	//camViewVec.print();
	Vector4f dirAhead = { camViewVec[0], 0.0f,  camViewVec[2], 1.0f };
	//std::cout << "dirAhead length: " << dirAhead.abs() << std::endl;
	Vector3f dirSide = { camViewVec[2], 0.0f, camViewVec[0]};
	//dirSide.print();
	/*float length = dirAhead.abs();
	Vector4f dirUp = { 0.0f, length, 0.0f, 1.0f };*/
	Vector4f dirUp = { 0.0f, 1.0f, 0.0f, 1.0f };
	//dirUp.print();
	// calculate camViewVec angle with axis Y

	double rotXAngle = atan(camViewVec[1] / dirAhead.abs());
	//std::cout << "rotXAngle: " << rotXAngle << std::endl;

	Vector4f cam = this->GetCamPosVec();
	//cam.print();
	Vector4f target = this->GetTargetPosVec();
	//target.print();
	
	double rotXAngleInc = rotXAngle + rotX;
	//std::cout << "RotXAngle: " << rotXAngle << std::endl;

	if(rotXAngleInc >= (this->pi / 2.0) || rotXAngleInc <= (this->pi / -2.0))
	{
		rotX = this->pi / 2.0 - rotXAngle;
	}

	Matrix4f rotMatX = {
			1.0, 0.0, 0.0, 0.0,
			0.0, 1.0, 0.0, 0.0,
			0.0, 0.0, 1.0, 0.0,
			0.0, 0.0, 0.0, 1.0 };	// egyelõre 0 mátrix ki kell tölteni a megfelelõ adatatokkal
		//rotMatX.print();
	rotMatX = rotMatX.rotation(dirSide, (float)rotX);
	//rotMatX.print();
	cam = rotMatX * cam;
	//cam.print();
	target = rotMatX * target;
	//target.print();
	
	Matrix4f rotMatY = {
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0 };
	
	rotMatY = rotMatY.rotateY((float)rotY);

	cam = rotMatY * cam;
	target = rotMatY * target;

	this->SetNewCamPos(cam, target);
}

void Camera::applyCamRotation(
	double rotX, double rotY, double rotZ,
	const Vector4f& camStartPos, const Vector4f& targetStartPos
)
{
	// rotation around center of object (have to calculate) now it's 0,0,0
	this->CalcDirVec();
	this->CalcWorldAngles();
	Vector4f camViewVec = this->GetCamDirVec();
	Vector4f dirAhead = { camViewVec[0], 0.0f,  camViewVec[2], 1.0f };
	Vector3f dirSide = { camViewVec[2], 0.0f, camViewVec[0] };
	Vector4f dirUp = { 0.0f, 1.0f, 0.0f, 1.0f };
	// calculate camViewVec angle with axis Y

	double rotXAngle = atan(camViewVec[1] / dirAhead.abs());

	rotXAngle += rotX;

	Vector4f target;
	Vector4f cam;

	if (rotXAngle < (this->pi / 2.0) && rotXAngle >(this->pi / -2.0))
	{
		Matrix4f rotMatX = {
			1.0, 0.0, 0.0, 0.0,
			0.0, 1.0, 0.0, 0.0,
			0.0, 0.0, 1.0, 0.0,
			0.0, 0.0, 0.0, 1.0 };	// egyelõre 0 mátrix ki kell tölteni a megfelelõ adatatokkal
		//rotMatX.print();
		rotMatX = rotMatX.rotation(dirSide, (float)rotX);
		//rotMatX.print();
		cam = rotMatX * camStartPos;
		//cam.print();
		target = rotMatX * targetStartPos;
		//target.print();
	}
	Matrix4f rotMatY;
	rotMatY = rotMatY.rotateY((float)rotY);

	cam = rotMatY * camStartPos;
	target = rotMatY * targetStartPos;

	this->SetNewCamPos(cam, target);
}