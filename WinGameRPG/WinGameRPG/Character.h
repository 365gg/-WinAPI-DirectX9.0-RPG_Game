#pragma once
#include<Windows.h>
#include<vector>
#include"Component.h"
struct Sprite;
class Character : public Component
{
private:
	//Sprite* _sprite;
	std::vector<Sprite*> _spriteList;

	float _x;
	float _y;

	float _deltaX;
	float _deltaY;

	int _tileX;
	int _tileY;
public:
	Character(LPCWSTR name);
	~Character();

	void Init();
	void DInit();
	void Update(float deltaTime);
	void Render();
	void Release();
	void Reset();

	//AI
public:
	void UpdateAI();
	//Move
public:
	enum eDirection {
		LEFT,RIGHT,UP,DOWN
	};
private:
	bool _isMoving;
	float _moveTime;
	float _movingDuration;
	eDirection _currentDirection;
public:
	void InitMove();
	void MoveStart(eDirection direction);
	void UpdateMove(float deltaTime);
};