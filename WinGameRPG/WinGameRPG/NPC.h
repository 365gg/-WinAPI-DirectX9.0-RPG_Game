#pragma once
#include"Character.h"
class NPC : public Character
{
private:
public:
	NPC(LPCWSTR name, LPCWSTR pngName);
	~NPC();
};