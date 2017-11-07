#include "Character.h"
#include "Sprite.h"
#include "ComponentSystem.h"
#include "Map.h"
Character::Character(LPCWSTR name, LPCWSTR scriptName, LPCWSTR pngName) : Component(name)
{
	_moveTime = 1.0f;
	_spriteList.clear();
	_scrpitName = scriptName;
	_pngName = pngName;
}

Character::~Character()
{
}
void Character::Init()
{
	InitMove();
	WCHAR textureFileName[256];
	wsprintf(textureFileName, L"%s.png", _pngName.c_str());
	WCHAR scriptFileName[256];
	{
		wsprintf(scriptFileName, L"%s_left.json", _scrpitName.c_str());
		Sprite* sprite = new Sprite(textureFileName, scriptFileName);
		sprite->Init();
		_spriteList.push_back(sprite);
	}
	{
		wsprintf(scriptFileName, L"%s_right.json", _scrpitName.c_str());
		Sprite* sprite = new Sprite(textureFileName, scriptFileName);
		sprite->Init();
		_spriteList.push_back(sprite);
	}
	{
		wsprintf(scriptFileName, L"%s_up.json", _scrpitName.c_str());
		Sprite* sprite = new Sprite(textureFileName, scriptFileName);
		sprite->Init();
		_spriteList.push_back(sprite);
	}
	{
		wsprintf(scriptFileName, L"%s_down.json", _scrpitName.c_str());
		Sprite* sprite = new Sprite(textureFileName, scriptFileName);
		sprite->Init();
		_spriteList.push_back(sprite);
	}
	
	{
		Map* map = (Map*)ComponentSystem::GetInstance()->FindComponent(L"tileMap");
		_tileX = rand() % map->GetWidth();
		_tileY = rand() % map->GetHeight();
		_x = map->GetPositionX(_tileX, _tileY);
		_y = map->GetPositionY(_tileX, _tileY);
		map->setTileComponent(_tileX, _tileY, this, true);
	}
}
void Character::DInit()
{
	for (int i = 0; i < _spriteList.size(); i++)
	{
		_spriteList[i]->DInit();
		delete _spriteList[i];
	}
}
void Character::Update(float deltaTime)
{
	_spriteList[(int)_currentDirection]->Update(deltaTime);
	UpdateAI();
	UpdateMove(deltaTime);
}
void Character::Render()
{
	_spriteList[(int)_currentDirection]->SetPosition(_x, _y);
	_spriteList[(int)_currentDirection]->Render();
}
void Character::Release()
{
	for (int i = 0; i < _spriteList.size(); i++)
	{
		_spriteList[i]->Release();
	}
}
void Character::Reset()
{
	for (int i = 0; i < _spriteList.size(); i++)
	{
		_spriteList[i]->Reset();
	}
}
void Character::MoveDeltaPosition(float deltaX, float deltaY)
{
	_x += deltaX;
	_y += deltaY;
}
void Character::SetPosition(float posX, float posY)
{
	_x = posX;
	_y = posY;
}
void Character::UpdateAI()
{
	if (false == _isMoving)
	{
		int direction = rand() % 4;
		MoveStart((eDirection)direction);
	}
}
void Character::InitMove()
{
	_currentDirection = eDirection::DOWN;
	_isMoving = false;
	_movingDuration = 0.0f;
	_targetX = 0.0f;
	_targetY = 0.0f;
	_moveDistancePerTimeX = 0.0f;
	_moveDistancePerTimeY = 0.0f;

	_deltaX = _deltaY = 0.0f;
}
void Character::MoveStart(eDirection direction)
{
	_currentDirection = direction;
	if (true == _isMoving)
		return;

	Map* map = (Map*)ComponentSystem::GetInstance()->FindComponent(L"tileMap");

	int newTileX = _tileX;
	int newTileY = _tileY;
	switch (direction)
	{
	case eDirection::LEFT:
		//Left
		newTileX--;
		break;
	case eDirection::RIGHT:
		//Right
		newTileX++;
		break;
	case eDirection::UP:
		//Up
		newTileY--;
		break;
	case eDirection::DOWN:
		//Down
		newTileY++;
		break;
	}

	/*if (false == map->CanMoveTileMap(newTileX, newTileY))
		return;*/

	std::list<Component*> collisionList;
	bool canMove = map->GetTileCollisionList(newTileX, newTileY, collisionList);
	if (false == canMove)
	{
		//collisionList ��ȯ
		for (std::list<Component*>::iterator it = collisionList.begin(); it != collisionList.end(); it++)
		{
			ComponentSystem::GetInstance()->SendMessage(this, (*it), L"Collision");
		}
		return;
	}

	map->ResetTileComponent(_tileX, _tileY, this);
	_x = map->GetPositionX(_tileX, _tileY);
	_y = map->GetPositionY(_tileX, _tileY);

	_tileX = newTileX;
	_tileY = newTileY;
	
	//switch (direction)
	//{
	//case eDirection::LEFT:
	//	//Left
	//	_tileX--;
	//	if (_tileX < 0)
	//		_tileX = 0;
	//	break;
	//case eDirection::RIGHT:
	//	//Right
	//	_tileX++;
	//	if (49 < _tileX)
	//		_tileX = 49;
	//	break;
	//case eDirection::UP:
	//	//Up
	//	_tileY--;
	//	if (_tileY < 0)
	//		_tileY = 0;
	//	break;
	//case eDirection::DOWN:
	//	//Down
	//	_tileY++;
	//	if (29 < _tileY)
	//		_tileY = 29;
	//	break;
	//}
	{
		//�ڿ������� �̵� ����
		map->setTileComponent(_tileX, _tileY, this, false);
		//�̵��Ÿ�
		_targetX = map->GetPositionX(_tileX, _tileY);
		_targetY = map->GetPositionY(_tileX, _tileY);

		float distanceX = _targetX - _x;
		float distanceY = _targetY - _y;

		//�ּ� �̵��Ÿ�
		_moveDistancePerTimeX = distanceX / _moveTime;
		_moveDistancePerTimeY = distanceY / _moveTime;
	}
	_isMoving = true;
}
void Character::UpdateMove(float deltaTime)
{
	if (false == _isMoving)
		return;
	if (_moveTime <= _movingDuration)
	{
		_movingDuration = 0.0f;
		_isMoving = false;
		_x = _targetX;
		_y = _targetY;
		_moveDistancePerTimeX = _moveDistancePerTimeY = 0.0f;
	}
	else
	{
		_movingDuration += deltaTime;

		float moveDistanceX = _moveDistancePerTimeX * deltaTime;
		float moveDistanceY = _moveDistancePerTimeY * deltaTime;
		_x += moveDistanceX;
		_y += moveDistanceY;
	}
}