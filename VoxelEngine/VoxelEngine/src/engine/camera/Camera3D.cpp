#include "Camera3D.h"

#include <algorithm>
#include <limits>

Camera3D::Camera3D(const glm::vec3& position, const glm::vec3& up, const float yaw, const float pitch)
	: m_position(position), m_worldUp(up), m_yaw(yaw), m_pitch(pitch)
{
	UpdateVectors();
}

void Camera3D::ProcessInput(const KeyboardState& keyboardState, const MouseState& mouseState, const Sint32 scrollState, const GameControllerState& gameControllerState)
{
	m_velocity = glm::vec3{ 0.0f, 0.0f, 0.0f };
	m_movementSpeed = keyboardState.currentKeys[SDL_SCANCODE_LSHIFT] || gameControllerState.currentButtons.a ? s_DefaultSpeed * 4.0f : s_DefaultSpeed;

	constexpr Sint16 ControllerAxisDeadZone = 8'000;

	if (keyboardState.currentKeys[SDL_SCANCODE_W] || gameControllerState.currentButtons.dPad.up || gameControllerState.currentAxes.left.y < -ControllerAxisDeadZone)
	{
		m_velocity += m_front * m_movementSpeed;
	}

	if (keyboardState.currentKeys[SDL_SCANCODE_S] || gameControllerState.currentButtons.dPad.down || gameControllerState.currentAxes.left.y > ControllerAxisDeadZone)
	{
		m_velocity -= m_front * m_movementSpeed;
	}

	if (keyboardState.currentKeys[SDL_SCANCODE_A] || gameControllerState.currentButtons.dPad.left || gameControllerState.currentAxes.left.x < -ControllerAxisDeadZone)
	{
		m_velocity -= m_right * m_movementSpeed;
	}

	if (keyboardState.currentKeys[SDL_SCANCODE_D] || gameControllerState.currentButtons.dPad.right || gameControllerState.currentAxes.left.x > ControllerAxisDeadZone)
	{
		m_velocity += m_right * m_movementSpeed;
	}

	const float xOffset = mouseState.relativeX * m_mouseSensitivity;
	const float yOffset = mouseState.relativeY * m_mouseSensitivity;

	const Sint16 gameControllerXOffset = gameControllerState.currentAxes.right.x;
	const Sint16 gameControllerYOffset = gameControllerState.currentAxes.right.y;

	m_yaw += xOffset;
	m_pitch -= yOffset;

	constexpr float ControllerSensitivity = 1.5f;

	if (gameControllerXOffset > ControllerAxisDeadZone || gameControllerXOffset < -ControllerAxisDeadZone)
	{
		m_yaw += gameControllerXOffset / (static_cast<float>(std::numeric_limits<Sint16>::max()) / ControllerSensitivity);
	}

	if (gameControllerYOffset > ControllerAxisDeadZone || gameControllerYOffset < -ControllerAxisDeadZone)
	{
		m_pitch -= gameControllerYOffset / (static_cast<float>(std::numeric_limits<Sint16>::max()) / ControllerSensitivity);
	}

	constexpr float MaxPitch = 89.5f;
	constexpr float MinPitch = -89.5f;

	m_pitch = std::clamp(m_pitch, MinPitch, MaxPitch);

	UpdateVectors();
}

void Camera3D::Update(const float deltaTime)
{
	m_position += m_velocity * deltaTime;
}

void Camera3D::Render() const
{ }

void Camera3D::UpdateVectors()
{
	glm::vec3 updatedFront{
		glm::cos(glm::radians(m_yaw)) * glm::cos(glm::radians(m_pitch)),
		glm::sin(glm::radians(m_pitch)),
		glm::sin(glm::radians(m_yaw)) * glm::cos(glm::radians(m_pitch))
	};

	m_front = glm::normalize(updatedFront);
	m_right = glm::normalize(glm::cross(m_front, m_worldUp));
	m_up = glm::normalize(glm::cross(m_right, m_front));
}