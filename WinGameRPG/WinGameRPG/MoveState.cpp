#include"MoveState.h"
#include"Character.h"
#include"Map.h"
#include"ComponentSystem.h"
#include"GameSystem.h"
#include"Stage.h"
#include "GlobalType.h"
MoveState::MoveState()
{
	_nowState = eStateType::ET_MOVE;
}

MoveState::~MoveState()
{
}
void MoveState::Init(Character* character)
{
	State::Init(character);
	_movingDuration = 0.0f;
}
void MoveState::Update(float deltaTime)
{
	State::Update(deltaTime);
	if (false == _character->IsLive())
		return;
	//add code
	//if (false == _character->IsMoving())
	//	return;
	//if (_character->GetMoveTime() <= _movingDuration)
	//{
	//	_movingDuration = 0.0f;
	//	_character->MoveStop();
	//	_nextState = eStateType::ET_IDLE;
	//}
	//else
	//{
	//	_movingDuration += deltaTime;
	//	_character->Moving(deltaTime); 
	//}
	if (_character->GetMoveTime() <= _movingDuration)
	{
		_nextState = eStateType::ET_IDLE;
	}
	else
	{
		_movingDuration += deltaTime;
	}
}
void MoveState::Start()
{
	State::Start();
	if (true == _character->IsMoving())
		return;
	//Map* map = (Map*)ComponentSystem::GetInstance()->FindComponent(L"tileMap");
	Map* map = GameSystem::GetInstance()->GetStage()->GetMap();

	TilePosition currentTilePos;
	currentTilePos.x = _character->GetTileX();
	currentTilePos.y = _character->GetTileY();
	TilePosition nextTilePos = GetNextTilePosition(currentTilePos, _character->GetDirection());
	/*switch (_character->GetDirection())
	{
	case eDirection::LEFT:
		newTileX--;
		break;
	case eDirection::RIGHT:
		newTileX++;
		break;
	case eDirection::UP:
		newTileY--;
		break;
	case eDirection::DOWN:
		newTileY++;
		break;
	}*/

	std::list<Component*> collisionList;
	bool canMove = map->GetTileCollisionList(nextTilePos.x, nextTilePos.y, collisionList);
	if (false == canMove)
	{
		Component* target = _character->Collision(collisionList);
		if (NULL != target && _character->IsAttackCoolTime()) // ���� �����ϱ�
		{
			_character->ResetAttackCoolTime();
			_character->SetTarget(target);
			_nextState = eStateType::ET_ATTACK;
		}
		else
		{
			_nextState = eStateType::ET_IDLE;
		}
	}
	else
	{
		_character->MoveStart(nextTilePos.x, nextTilePos.y);
		_movingDuration = 0.0f;	//add code
		_character->MoveStop(); //add code
	}
	/*wchar_t distanceXCheck[256];
	swprintf(distanceXCheck, L"char Start GetX %f\n", _character->GetX());
	OutputDebugString(distanceXCheck);*/
}
void MoveState::Stop()
{
	State::Stop();
	Map* map = GameSystem::GetInstance()->GetStage()->GetMap();
	//character->Set_moveDistancePerTimeX = character->_moveDistancePerTimeY = 0.0f;
	map->ViewerScroll(_character, 0.0f, 0.0f);
}