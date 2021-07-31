#pragma once
#include "EMath.h"
#include "InputManager.h"

class Camera final
{
public:
	Camera();
	Camera(const Camera& l) = delete;
	Camera(Camera&& l) = delete;
	Camera& operator=(const Camera& l) = delete;
	Camera& operator=(Camera&& l) = delete;
	~Camera() = default;

	/* Initialize camera with some base variables */
	void Initialize(const Elite::FPoint3& position, float width, float height, float fovAngle, bool isLH = false);

	/* Update camera according to the input and time */
	void Update(const InputManager& input, float deltaT);

	/* Return the aspect ratio of the camera */
	float GetAspectRatio() const { return m_AspectRatio; }

	/* Return the fov value (!= the fov angle) */
	float GetFOV() const { return m_FOV; }

	/* Return reference to the camera position */
	const Elite::FPoint3& GetPosition() const { return m_Position; }

	/* Return reference to the look at matrix (also known as cameraToWorld matrix) */
	const Elite::FMatrix4& GetLookAtMatrix() const { return m_LookAt; }

	/* Return reference to the world view projection matrix (right handed system) */
	const Elite::FMatrix4& GetProjMatrix() const { return m_ProjMatrix; }

	/* Returns whether the camera is using a left handed system */
	bool IsLH() const { return m_IsLH; }

	/* Set new fov angle for camera */
	void SetFOV(float fovAngle) { m_FOV = (float)tan(fovAngle * E_TO_RADIANS / 2.f);  MakeProjMatrix(); }

	/* Set new camera position by FPoint3 */
	void SetPosition(const Elite::FPoint3& position);

	/* Set new camera position by x,y,z floats */
	void SetPosition(float x, float y, float z);

	/* Set new translation speed (how fast the camera moves through the scene) */
	void SetTranslationSpeed(float speed) { m_TranslationSpeed = speed; }

	/* Set new rotation speed (how fast the camera rotates around the scene) */
	void SetRotationSpeed(float speed) { m_RotationSpeed = speed; }

	/* Set new value for the near plane distance */
	void SetNearPlane(float nearPlane);

	/* Set new value for the far plane distance */
	void SetFarPlane(float farPlane);

	/* Sets the camera to adjust to a left hand system or not */
	void SetLeftHanded(bool isLH);

private:
	/* Camera Settings */
	bool m_IsLH;
	float m_AspectRatio;
	float m_FOV;
	float m_TranslationSpeed;
	float m_RotationSpeed;

	/* Window info */
	int m_Height;
	int m_Width;

	/* View Matrix */
	Elite::FMatrix4 m_LookAt;
	Elite::FVector3 m_Right;
	Elite::FVector3 m_Up;
	Elite::FVector3 m_WorldUp;
	Elite::FVector3 m_Forward;
	Elite::FPoint3 m_Position;

	/* Projection Matrix */
	Elite::FMatrix4 m_ProjMatrix;
	float m_NearPlane;
	float m_FarPlane;

	/* Private functions */
	void MakeLookAtMatrix();
	void MakeProjMatrix();
};