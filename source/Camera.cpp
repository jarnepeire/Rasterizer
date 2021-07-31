#pragma once
#include "pch.h"
#include "Camera.h"
#include "SDL.h"
#include <iostream>

using namespace Elite;

Camera::Camera()
	: m_IsLH(false)
	, m_AspectRatio()
	, m_FOV()
	, m_TranslationSpeed(2.f)
	, m_RotationSpeed(0.3f)
	, m_Height()
	, m_Width()
	, m_LookAt(Elite::FMatrix4::Identity())
	, m_Right(1.f, 0.f, 0.f)
	, m_Up(0.f, 1.f, 0.f)
	, m_WorldUp(0.f, 1.f, 0.f)
	, m_Forward(0.f, 0.f, 1.f)
	, m_Position()
	, m_ProjMatrix(Elite::FMatrix4::Identity())
	, m_NearPlane(0.1f)
	, m_FarPlane(100.f)
{
}

void Camera::Initialize(const Elite::FPoint3& position, float width, float height, float fovAngle, bool isLH)
{
	m_IsLH = isLH;
	m_Position = position;
	m_AspectRatio = width / height;
	m_FOV = (float)tan(fovAngle * E_TO_RADIANS / 2.f);

	MakeLookAtMatrix();
	MakeProjMatrix();
}

void Camera::Update(const InputManager& input, float deltaT)
{
	//Get mouse data
	const auto& relativeMouseData = input.GetRelativeMouseStateData();

	//If we haven't moved our mouse -> early return
	if (Elite::AreEqual(0.f, float(relativeMouseData.y + relativeMouseData.x)))
		return;

	//Check for button presses
	if (input.IsPressed(EMouseInput::LRMB))
	{
		//Translation only (UP AND DOWN)
		m_Position.y -= relativeMouseData.y * m_TranslationSpeed * deltaT;

		//Remake ONB with new position
		MakeLookAtMatrix();
	}
	else if (input.IsPressed(EMouseInput::LMB))
	{
		//Rotation First
		Elite::FMatrix3 yRotation = MakeRotation(-relativeMouseData.x * m_RotationSpeed * deltaT, m_Up);
		m_Forward = yRotation * m_Forward;

		//Translation Second
		float speed = relativeMouseData.y * m_TranslationSpeed * deltaT;
		m_Position.x += speed * m_Forward.x;
		m_Position.z += speed * m_Forward.z;

		//Remake ONB with newly updated vectors/position
		MakeLookAtMatrix();
	}
	else if (input.IsPressed(EMouseInput::RMB))
	{
		//Rotation
		Elite::FMatrix3 xRotation = MakeRotation(-relativeMouseData.y * m_RotationSpeed * deltaT, m_Right);
		Elite::FMatrix3 yRotation = MakeRotation(-relativeMouseData.x * m_RotationSpeed * deltaT, m_Up);
		m_Forward = xRotation * yRotation * m_Forward;

		//Remake ONB with new forward
		MakeLookAtMatrix();
	}
}

void Camera::SetPosition(const Elite::FPoint3& position)
{
	SetPosition(position.x, position.y, position.z);
}

void Camera::SetPosition(float x, float y, float z)
{
	m_Position = Elite::FPoint3(x, y, z); 
	MakeLookAtMatrix();
}

void Camera::SetNearPlane(float nearPlane)
{
	m_NearPlane = nearPlane; 
	MakeProjMatrix();
}

void Camera::SetFarPlane(float farPlane)
{
	m_FarPlane = farPlane; 
	MakeProjMatrix();
}

void Camera::SetLeftHanded(bool isLH)
{
	m_IsLH = isLH; 
	MakeProjMatrix(); 
	MakeLookAtMatrix();
}

void Camera::MakeLookAtMatrix()
{
	if (m_IsLH)
	{
		//LHS
		//To get same rotation behavior on a flipped z for position, we will need to flip xy on forward
		FVector3 newForward{ -m_Forward.x, -m_Forward.y, m_Forward.z };
		FPoint3 newPosition{ m_Position.x, m_Position.y, -m_Position.z };
		m_Right = Elite::GetNormalized(Cross(m_WorldUp, newForward));
		m_Up = Elite::GetNormalized(Cross(newForward, m_Right));

		m_LookAt = Elite::FMatrix4(m_Right, m_Up, newForward, FVector4(FVector3(newPosition), 1.f));
	}
	else
	{
		//RHS
		m_Right = Elite::GetNormalized(Cross(m_WorldUp, m_Forward));
		m_Up = Elite::GetNormalized(Cross(m_Forward, m_Right));

		m_LookAt = Elite::FMatrix4(m_Right, m_Up, m_Forward, FVector4(FVector3(m_Position), 1.f));
	}
}

void Camera::MakeProjMatrix()
{
	if (m_IsLH)
	{
		m_ProjMatrix = Elite::FMatrix4
		{
			FVector4{ 1.f / (m_AspectRatio * m_FOV), 0.f, 0.f, 0.f },
			FVector4{ 0.f, 1.f / m_FOV, 0.f, 0.f },
			FVector4{ 0.f, 0.f, m_FarPlane / (m_FarPlane - m_NearPlane), 1.f },
			FVector4{ 0.f, 0.f, -(m_FarPlane * m_NearPlane) / (m_FarPlane - m_NearPlane), 0.f }
		};
	}
	else
	{
		m_ProjMatrix = Elite::FMatrix4
		{
			FVector4{ 1.f / (m_AspectRatio * m_FOV), 0.f, 0.f, 0.f },
			FVector4{ 0.f, 1.f / m_FOV, 0.f, 0.f },
			FVector4{ 0.f, 0.f, m_FarPlane / (m_NearPlane - m_FarPlane), -1.f },
			FVector4{ 0.f, 0.f, (m_FarPlane * m_NearPlane) / (m_NearPlane - m_FarPlane), 0.f }
		};
	}

}