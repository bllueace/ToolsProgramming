#pragma once

struct InputCommands

{
	bool forward;
	bool back;
	bool right;
	bool left;
	bool up;
	bool down;
	bool rotRight;
	bool rotLeft;
	bool rotUp;
	bool rotDown;
	bool scaleUp;
	bool scaleDown;
	bool objFocus;
	////////////////////////
	int RawMouseX;
	int RawMouseY;
	int mouse_X;
	int mouse_Y;
	bool rightMouseDown;
	bool leftMouseDown;
	bool showCursor;
	bool wireFrame;
	bool terrainPainting;
	bool rotateMode;
	bool scaleMode;

	bool buttonDown;
	bool Undo;

	float mouseDragX;
	float mouseDragY;
	/////////////////
	bool test;
};
