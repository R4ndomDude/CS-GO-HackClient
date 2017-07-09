#pragma once

class IInputSystem
{
public:
	void EnableInput(bool bEnable);
	void ResetInputState();
};