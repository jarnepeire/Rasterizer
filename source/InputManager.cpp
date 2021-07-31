#pragma once
#include "pch.h"
#include "InputManager.h"

InputManager::InputManager()
	: m_MouseButtonState()
	, m_RelativeMouseStateData()
	, m_Event()
	, m_KeyboardInput()
	, m_IsLooping(true)
{
}

void InputManager::ProcessInput()
{
	//Reset keyboard input (else it would hold its value)
	m_KeyboardInput = EKeyboardInput::Empty;

	//SDL_Event e;
	while (SDL_PollEvent(&m_Event))
	{
		switch (m_Event.type)
		{
		case SDL_QUIT:
			m_IsLooping = false;
			break;

		case SDL_KEYUP:
			//Converting the underlying int value of scancode to the according keyboard action defined in header!
			m_KeyboardInput = EKeyboardInput(m_Event.key.keysym.scancode);
			break;

		default:
			m_KeyboardInput = EKeyboardInput::Empty;
			break;

		}
	}

	//Mouse Input
	//Only tracking button state to compare to 
	m_MouseButtonState = SDL_GetRelativeMouseState(&m_RelativeMouseStateData.x, &m_RelativeMouseStateData.y);
}

bool InputManager::IsPressed(EMouseInput button) const
{
	return (m_MouseButtonState == Uint32(button));
}

bool InputManager::IsPressed(EKeyboardInput button) const
{
	return (m_KeyboardInput == button);
}
