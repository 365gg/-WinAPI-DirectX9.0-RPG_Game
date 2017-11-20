#include "Character.h"
#include "Sprite.h"
#include "ComponentSystem.h"
#include "Map.h"
#include "sComponentMsgParam.h"
#include "MoveState.h"
#include "IdleState.h"
#include "AttackState.h"
#include "DefenseState.h"
#include "DeadState.h"
Character::Character(LPCWSTR name, LPCWSTR scriptName, LPCWSTR pngName) : Component(name)
{
	_state = NULL;
	_moveTime = 1.0f;
	//_spriteList.clear();
	_scrpitName = scriptName;
	_pngName = pngName;
	_attackPoint = 10;
	_hp = 100;

	_targetX = 0.0f;
	_targetY = 0.0f;
	_moveDistancePerTimeX = 0.0f;
	_moveDistancePerTimeY = 0.0f;
	_deltaX = _deltaY = 0.0f;

	_attackCoolTimeDuration = 0.0f;
	_attackCoolTime = 2.0f;
}

Character::~Character()
{
	
}
void Character::Init()
{
	{
		Map* map = (Map*)ComponentSystem::GetInstance()->FindComponent(L"tileMap");
		_tileX = rand() % map->GetWidth();
		_tileY = rand() % map->GetHeight();
		while (!map->CanMoveTileMap(_tileX, _tileY))
		{
			_tileX = rand() % map->GetWidth();
			_tileY = rand() % map->GetHeight();
		}
		_x = map->GetPositionX(_tileX, _tileY);
		_y = map->GetPositionY(_tileX, _tileY);
		map->setTileComponent(_tileX, _tileY, this, true);
	}
	InitMove();
	{
		State* state = new IdleState();
		state->Init(this);
		_stateMap[eStateType::ET_IDLE] = state;
	}
	{
		State* state = new MoveState();
		state->Init(this);
		_stateMap[eStateType::ET_MOVE] = state;
	}
	{
		State* state = new AttackState();
		state->Init(this);
		_stateMap[eStateType::ET_ATTACK] = state;
	}
	{
		State* state = new DefenseState();
		state->Init(this);
		_stateMap[eStateType::ET_DEFENSE] = state;
	}
	{
		State* state = new DeadState();
		state->Init(this);
		_stateMap[eStateType::ET_DEAD] = state;
	}
	ChangeState(eStateType::ET_IDLE);
}
void Character::DInit()
{
	std::map<eStateType, State*>::iterator it = _stateMap.begin();
	while(it != _stateMap.end())
	{
		State* state = it->second;
		delete state;
		it++;
	}
	_stateMap.clear();
}
void Character::Update(float deltaTime)
{
	UpdateAttackCoolTime(deltaTime);
	_state->Update(deltaTime);
}
void Character::Render()
{
	_state->Render();
}
void Character::Release()
{
	_state->Release();
}
void Character::Reset()
{
	_state->Reset();
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
	_currentDirection = (eDirection)(rand() % 4);
	//ChangeState(eStateType::ET_MOVE);
	_state->NextState(eStateType::ET_MOVE);
}
void Character::ChangeState(eStateType stateType)
{
	if (NULL != _state)
	{
		_state->Stop();
	}
	_state = _stateMap[stateType];
	_state->Start();
}
void Character::InitMove()
{
	_isMoving = false;
	_currentDirection = eDirection::DOWN;
}
void Character::MoveStart(int newTileX, int newTileY)
{
	Map* map = (Map*)ComponentSystem::GetInstance()->FindComponent(L"tileMap");
	map->ResetTileComponent(_tileX, _tileY, this);
	_x = map->GetPositionX(_tileX, _tileY);
	_y = map->GetPositionY(_tileX, _tileY);

	_tileX = newTileX;
	_tileY = newTileY;
	{
		//자연스러운 이동 보간
		map->setTileComponent(_tileX, _tileY, this, false);
		//이동거리
		_targetX = map->GetPositionX(_tileX, _tileY);
		_targetY = map->GetPositionY(_tileX, _tileY);

		float distanceX = _targetX - _x;
		float distanceY = _targetY - _y;

		//최소 이동거리
		_moveDistancePerTimeX = distanceX / _moveTime;
		_moveDistancePerTimeY = distanceY / _moveTime;
	}
	_isMoving = true;
}
void Character::MoveStop()
{
	_isMoving = false;
	Map* map = (Map*)ComponentSystem::GetInstance()->FindComponent(L"tileMap");
	_x = map->GetPositionX(_tileX, _tileY);
	_y = map->GetPositionY(_tileX, _tileY);
	_moveDistancePerTimeX = 0.0f;
	_moveDistancePerTimeY = 0.0f;
}
void Character::Moving(float deltaTime)
{
	float moveDistanceX = _moveDistancePerTimeX * deltaTime;
	float moveDistanceY = _moveDistancePerTimeY * deltaTime;
	_x += moveDistanceX;
	_y += moveDistanceY;
}
void Character::UpdateAttackCoolTime(float deltaTime)
{
	if (_attackCoolTimeDuration < _attackCoolTime)
	{
		_attackCoolTimeDuration += deltaTime;
	}
}
bool Character::IsAttackCoolTime()
{
	if (_attackCoolTimeDuration <= _attackCoolTime)
		return true;
	return false;
}
void Character::ResetAttackCoolTime()
{
	_attackCoolTimeDuration = 0.0f;
}
Component* Character::Collision(std::list<Component*>& collisionList)
{
	return NULL;
}

void Character::DecreaseHP(int decreaseHPpont)
{
	_hp -= decreaseHPpont;
	if (_hp < 0)
	{
		_isLive = false;
	}
}
void Character::ReceiveMessage(const sComponentMsgParam msgParam)
{
	if (L"Attack" == msgParam.message)
	{
		_attackedPoint = msgParam.attackPoint;
		ChangeState(eStateType::ET_DEFENSE);
	}
}