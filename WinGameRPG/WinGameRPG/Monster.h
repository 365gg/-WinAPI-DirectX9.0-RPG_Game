#pragma once
#include<Windows.h>
#include<list>
#include"Character.h"
class Monster : public Character
{
private:
public:
	Monster(LPCWSTR name, LPCWSTR scriptName, LPCWSTR pngName);
	~Monster();
	void UpdateAI();
	void Collision(std::list<Component*>& collisionList);
};