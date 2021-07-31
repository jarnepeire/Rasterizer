#pragma once
#include "SDL.h"
#include "EMath.h"

/* Binded mouse controls to the action description (only add and assign controls here!) */
enum class EMouseInput : Uint32
{
	LMB = SDL_BUTTON(1),
	RMB = SDL_BUTTON(3),
	LRMB = (SDL_BUTTON(1) + SDL_BUTTON(3)),
	Empty = 0
};

/* Binded keyboard controls to the action description (only add and assign controls here!) */
enum class EKeyboardInput : int
{
	DisplayKeyBindInfo = SDL_SCANCODE_I,
	ToggleRasterizer = SDL_SCANCODE_R,
	ToggleSamplerState = SDL_SCANCODE_F,
	ToggleTransparency = SDL_SCANCODE_T,
	ToggleCullMode = SDL_SCANCODE_C,
	StopRotating = SDL_SCANCODE_SPACE,
	TakeScreenshot = SDL_SCANCODE_X,
	PreviousScene = SDL_SCANCODE_F1,
	NextScene = SDL_SCANCODE_F2,
	Empty = 0
};

class InputManager final
{
public:
	InputManager();
	InputManager(const InputManager& im) = delete;
	InputManager(InputManager&& im) = delete;
	InputManager& operator=(const InputManager&& im) = delete;
	InputManager& operator=(InputManager&& im) = delete;
	~InputManager() = default;

	/* Processes inputs from both mouse and keyboard, checking for input can be used with the IsPressed functionality */
	void ProcessInput();

	/* Check if a certain MouseInput value is being pressed */
	bool IsPressed(EMouseInput button) const;

	/* Check if a certain KeyboardInput value is being pressed */
	bool IsPressed(EKeyboardInput button) const;

	/* Return relative mouse data (difference from the previous mouse position to the current one) */
	const Elite::IVector2& GetRelativeMouseStateData() const { return m_RelativeMouseStateData; }

	/* Check if the software is still running and processing (pressing exit button stops loop) */
	bool IsLooping() const { return m_IsLooping; }

private:
	/* Mouse data */
	Uint32 m_MouseButtonState;
	Elite::IVector2 m_RelativeMouseStateData;

	/* Keyboard data */
	SDL_Event m_Event;
	EKeyboardInput m_KeyboardInput;

	/* Other */
	bool m_IsLooping;
};

