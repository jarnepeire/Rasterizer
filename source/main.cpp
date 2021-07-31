#pragma once
#include "pch.h"
#undef main

//Standard includes
#include <iostream>
#include <vld.h>
#include "SceneManager.h"
#include "InputManager.h"

//Project includes
#include "ETimer.h"
#include "ERenderer.h"

//Scene includes
#include "MainScene.h"
#include "CustomScene.h"

void ShutDown(SDL_Window* pWindow)
{
	SDL_DestroyWindow(pWindow);
	SDL_Quit();
}

int main(int argc, char* args[])
{
	//Unreferenced parameters
	(void)argc;
	(void)args;

	//Create window + surfaces
	SDL_Init(SDL_INIT_VIDEO);

	const uint32_t width = 720;
	const uint32_t height = 540;
	SDL_Window* pWindow = SDL_CreateWindow(
		"Exam Project: DirectX/SRAS Merge - **Jarne Peire**",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		width, height, 0);

	if (!pWindow)
		return 1;

	//Initialize "framework"
	auto pTimer{ std::make_unique<Elite::Timer>() };
	auto pScenegraph{ std::make_unique<SceneManager>() };
	pScenegraph->AddScene(new MainScene(pWindow, "MainScene"));
	pScenegraph->AddScene(new CustomScene(pWindow, "CustomScene"));
	pScenegraph->SetActiveScene("MainScene");

	//Start loop
	pTimer->Start();
	bool isLooping = true;

	//Game loop reference: https://gameprogrammingpatterns.com/game-loop.html
	//Set settings for capped FPS
	float desiredFPS = 240.f;
	float lag{};
	float ms_per_update = 1.f / desiredFPS;
	int frames{};
	float totalTimePassed{};

	//Start timer for capped FPS counter
	Uint32 previous = SDL_GetTicks();

	while (isLooping)
	{
		//--------- Update at Fixed Frametime ---------
		Uint32 current = SDL_GetTicks();
		float elapsedTime = float(current - previous);
		previous = current;
		lag += elapsedTime / 1000.f; //-> it's in miliseconds

		//Update when possible
		while (lag >= ms_per_update)
		{
			frames++;
			lag -= ms_per_update;
			pScenegraph->Update(ms_per_update);
		}

		//--------- Render ---------
		pScenegraph->Render();

		//--------- Timer ---------
		//Update for capped FPS:
		totalTimePassed += elapsedTime;

		//Timer update for uncapped FPS:
		pTimer->Update();

		//Output frames every second
		if (totalTimePassed >= 1000.f)
		{
			std::cout << "MAX FPS: " << pTimer->GetFPS() << " (CAPPED AT: " << frames << " FPS)\n";
			frames = 0;
			totalTimePassed -= 1000.f;
		}

		//Screenshot
		auto pScene = pScenegraph->GetCurrentScene();
		const auto& input = pScene->GetInput();
		if (input.IsPressed(EKeyboardInput::TakeScreenshot))
		{
			if (!pScene->GetRenderer()->SaveBackbufferToImage())
				std::cout << "Screenshot saved!" << std::endl;
			else
				std::cout << "Something went wrong. Screenshot not saved!" << std::endl;
		}

		//Update looping
		isLooping = input.IsLooping();
	}
	pTimer->Stop();

	//Shutdown "framework"
	ShutDown(pWindow);
	return 0;
}