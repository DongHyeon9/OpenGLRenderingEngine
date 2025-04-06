#include "CameraManager.h"
#include "Engine.h"
#include "InputManager.h"

bool CameraManager::Init()
{
	return true;
}

void CameraManager::Update(float DeltaTime)
{
	if (KEY_PRESS(E_KEY_TYPE::W))
	{
		position += forward * cameraMoveSpeed * GLOBAL::UNIT * DeltaTime;
	}
	if (KEY_PRESS(E_KEY_TYPE::S))
	{
		position -= forward * cameraMoveSpeed * GLOBAL::UNIT * DeltaTime;
	}
	if (KEY_PRESS(E_KEY_TYPE::A))
	{
		position -= right * cameraMoveSpeed * GLOBAL::UNIT * DeltaTime;
	}
	if (KEY_PRESS(E_KEY_TYPE::D))
	{
		position += right * cameraMoveSpeed * GLOBAL::UNIT * DeltaTime;
	}
	if (KEY_PRESS(E_KEY_TYPE::Q))
	{
		position -= up * cameraMoveSpeed * GLOBAL::UNIT * DeltaTime;
	}
	if (KEY_PRESS(E_KEY_TYPE::E))
	{
		position += up * cameraMoveSpeed * GLOBAL::UNIT * DeltaTime;
	}

	if (KEY_PRESS(E_KEY_TYPE::UP))
	{
		rotation.x += cameraRotationSpeed * DeltaTime;
	}
	if (KEY_PRESS(E_KEY_TYPE::DOWN))
	{
		rotation.x -= cameraRotationSpeed * DeltaTime;
	}
	if (KEY_PRESS(E_KEY_TYPE::LEFT))
	{
		rotation.y -= cameraRotationSpeed * DeltaTime;
	}
	if (KEY_PRESS(E_KEY_TYPE::RIGHT))
	{
		rotation.y += cameraRotationSpeed * DeltaTime;
	}

	rotation.x = glm::clamp(rotation.x, -89.0f, +89.0f);

	// 1) 전방 방향 벡터 계산 (pitch, yaw 만 사용)
	float pitch = glm::radians(rotation.x);
	float yaw = glm::radians(rotation.y);

	//roll 은 lookAt 으로 처리되지 않음
	forward.x = cos(pitch) * cos(yaw);
	forward.y = sin(pitch);
	forward.z = cos(pitch) * sin(yaw);
	forward = glm::normalize(forward);

	// 2) 월드 업 벡터로부터 카메라 Up 벡터 계산
	glm::vec3 worldUp{ 0.0f, 1.0f, 0.0f };
	right = glm::normalize(glm::cross(forward, worldUp));
	up = glm::cross(right, forward);
}

glm::mat4 CameraManager::GetViewMatrix() const
{
    return glm::lookAt(position, position + forward, up);
}

glm::mat4 CameraManager::GetProjectionMatrix() const
{
    glm::vec2 windowSize{ GEngine->GetWindowSize() };
    return bPerspective ?
        glm::perspective(glm::radians(fov), windowSize.x / windowSize.y, nearZ, farZ) :
        glm::ortho(0.0f, windowSize.x, 0.0f, windowSize.y, nearZ, farZ);
}

void CameraManager::SetProjectionMode(bool bIsPerspective)
{
	bPerspective = bIsPerspective;
}
