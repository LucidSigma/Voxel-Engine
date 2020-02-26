#pragma once
#ifndef INPUT_H
#define INPUT_H

#include <SDL2/SDL.h>

struct KeyboardState
{
	const Uint8* currentKeys;
	const Uint8* previousKeys;
};

struct MouseState
{
	Uint32 currentButtonStates;
	Uint32 previousButtonStates;

	float x;
	float y;

	float relativeX;
	float relativeY;
};

struct GameControllerState
{
	struct Buttons
	{
		struct DPad
		{
			bool up;
			bool down;
			bool left;
			bool right;
		} dPad;

		bool a;
		bool b;
		bool x;
		bool y;

		bool back;
		bool guide;
		bool start;

		bool leftStick;
		bool rightStick;

		bool leftShoulder;
		bool rightShoulder;
	};

	struct Axes
	{
		struct Left
		{
			Sint16 x;
			Sint16 y;
		} left;

		struct Right
		{
			Sint16 x;
			Sint16 y;
		} right;

		Sint16 triggerLeft;
		Sint16 triggerRight;
	};
	
	Buttons currentButtons;
	Buttons previousButtons;

	Axes currentAxes;
	Axes previousAxes;
};

#endif