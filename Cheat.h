#pragma once
#include <string>
#include <windows.h>

class Cheat
{
private:
	float speed;
	std::wstring processName;

	DWORD findProcessId(const std::wstring& processName);
	DWORD findAddress(DWORD processId, const std::wstring& moduleName, const std::wstring& symbolName);
public:
	Cheat();
	Cheat(std::wstring processName);
	void ChangeSpeed(float newSpeed);
};

