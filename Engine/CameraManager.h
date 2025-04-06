#pragma once
#include "CoreMinimal.h"

class CameraManager
{
	DECLARE_SINGLETON(CameraManager);
public:

protected:

private:
	glm::vec3 position{ -3,0,0 };
	glm::vec3 rotation{};
	bool bPerspective{ true };

	glm::vec3 forward{ 1.0f, 0, 0 };
	glm::vec3 right{ 0, 0, 1.0f };
	glm::vec3 up{ 0, 1.0f, 0 };

	float fov{ 70.0f };
	float nearZ{ 0.1f };
	float farZ{ 100.0f };

	float cameraRotationSpeed{ 100.0f };
	float cameraMoveSpeed{ 1000.0f };

public:
	bool Init();
	void Update(float DeltaTime);
	glm::mat4 GetViewMatrix() const;
	glm::mat4 GetProjectionMatrix() const;
	void SetProjectionMode(bool bIsPerspective);

protected:

private:

};

