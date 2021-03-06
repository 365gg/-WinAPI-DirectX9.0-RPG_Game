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
#include "Font.h"
#include "GameSystem.h"
#include "Stage.h"
#include "PathfindingImmediateState.h"
Character::Character(wstring name, wstring scriptName, wstring pngName) : Component(name)
{
	_state = NULL;
	_moveTime = 0.1f;
	//_spriteList.clear();
	_scrpitName = scriptName;
	_pngName = pngName;
	_attackPoint = 50;
	_hp = 100;

	_targetX = 0.0f;
	_targetY = 0.0f;
	_moveDistancePerTimeX = 0.0f;
	_moveDistancePerTimeY = 0.0f;
	_deltaX = _deltaY = 0.0f;

	_attackCoolTimeDuration = 0.0f;
	_attackCoolTime = 2.0f;
	_font = NULL;
}

Character::~Character()
{
	
}
void Character::Init()
{
	{
		Map* map = GameSystem::GetInstance()->GetStage()->GetMap();
		_tileX = rand() % map->GetWidth();
		_tileY = rand() % map->GetHeight();
		while (!map->CanMoveTileMap(_tileX, _tileY))
		{
			_tileX = rand() % map->GetWidth();
			_tileY = rand() % map->GetHeight();
		}
		/*_tileX = 3;
		_tileY = 3;
		while (!map->CanMoveTileMap(_tileX, _tileY))
		{
			_tileX = 3;
			_tileY = 5;
		}*/
		_x = map->GetPositionX(_tileX, _tileY);
		_y = map->GetPositionY(_tileX, _tileY);
		map->setTileComponent(_tileX, _tileY, this, false);
	}
	InitMove();
	InitState();
	ChangeState(eStateType::ET_IDLE);
	//_state->NextState(eStateType::ET_IDLE);
	{
		D3DCOLOR color = D3DCOLOR_ARGB(255, 0, 0, 0);
		_font = new Font(L"Arial", 15, color);

		_font->SetRect(0, 0, 100, 100);
		UpdateText();
	}
}
void Character::InitTilePosition(int tileX, int tileY)
{
	Map* map = GameSystem::GetInstance()->GetStage()->GetMap();
	map->ResetTileComponent(_tileX, _tileY, this);
	_tileX = tileX;
	_tileY = tileY;

	_x = map->GetPositionX(_tileX, _tileY);
	_y = map->GetPositionY(_tileX, _tileY);
	map->setTileComponent(_tileX, _tileY, this, false);
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
	/*if (NULL != _font)
	{
		_font->Dinit();
		delete _font;
	}*/
}
void Character::Update(float deltaTime)
{
	UpdateAttackCoolTime(deltaTime);
	UpdateText();
	_state->Update(deltaTime);
}
void Character::Render()
{
	_state->Render();
	_font->SetPosition(_x - 50, _y - 50);
	_font->Render();
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
void Character::InitState()
{
	ReplaceState(eStateType::ET_IDLE, new IdleState());
	ReplaceState(eStateType::ET_MOVE, new MoveState());
	ReplaceState(eStateType::ET_ATTACK, new AttackState());
	ReplaceState(eStateType::ET_DEFENSE, new DefenseState());
	ReplaceState(eStateType::ET_DEAD, new DeadState());
	ReplaceState(eStateType::ET_PATHFINDING, new PathfindingImmediateState());
}
void Character::ReplaceState(eStateType changeType, State* replaceState)
{
	map<eStateType, State*>::iterator it
		= _stateMap.find(changeType);
	if (it != _stateMap.end())
	{
		delete it->second;
		_stateMap.erase(changeType);
	}
	State* state = replaceState;
	state->Init(this);
	_stateMap[changeType] = state;
}
void Character::InitMove()
{
	_isMoving = false;
	_currentDirection = eDirection::DOWN;
}
void Character::MoveStart(int newTileX, int newTileY)
{
	//Map* map = (Map*)ComponentSystem::GetInstance()->FindComponent(L"tileMap");
	Map* map = GameSystem::GetInstance()->GetStage()->GetMap();
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
	Map* map = GameSystem::GetInstance()->GetStage()->GetMap();
	//캐릭 위치 보정된 값
	float correctionX = map->GetPositionX(_tileX, _tileY) - _x;
	float correctionY = map->GetPositionY(_tileX, _tileY) - _y;

	_x = map->GetPositionX(_tileX, _tileY);
	_y = map->GetPositionY(_tileX, _tileY);
	_moveDistancePerTimeX = correctionX;
	_moveDistancePerTimeY = correctionY;
	map->ViewerScroll(this, _moveDistancePerTimeX, _moveDistancePerTimeY);
}
void Character::Moving(float deltaTime)
{
	float moveDistanceX = _moveDistancePerTimeX * deltaTime;
	float moveDistanceY = _moveDistancePerTimeY * deltaTime;
	_x += moveDistanceX;
	_y += moveDistanceY;

	Map* map = GameSystem::GetInstance()->GetStage()->GetMap();
	map->ViewerScroll(this, moveDistanceX, moveDistanceY);
}

void Character::UpdateAttackCoolTime(float deltaTime)
{
	if (_attackCoolTimeDuration < _attackCoolTime)
	{
		_attackCoolTimeDuration += deltaTime;
	}
	else
	{
		_attackCoolTimeDuration = _attackCoolTime;
	}
}
bool Character::IsAttackCoolTime()
{
	if (_attackCoolTime<= _attackCoolTimeDuration)
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

void Character::IncreaseHP(int increaseHpPoint)
{
	_hp += increaseHpPoint;
	if (_hp >= 100)
	{
		_hp = 100;
	}
}
void Character::DecreaseHP(int decreaseHpPoint)
{
	_hp -= decreaseHpPoint;
	if (_hp <= 0)
	{
		_state->NextState(eStateType::ET_DEAD);
		_isLive = false;
	}
}
void Character::ReceiveMessage(const sComponentMsgParam msgParam)
{
	if (L"Attack" == msgParam.message)
	{
		_attackedPoint = msgParam.attackPoint;
		_state->NextState(eStateType::ET_DEFENSE);
	}
	if (L"WaveAttack" == msgParam.message)
	{
		_attackedPoint = msgParam.attackPoint;
		_state->NextState(eStateType::ET_DEFENSE);
	}
}
void Character::UpdateText()
{
	int coolTime = (int)(_attackCoolTimeDuration * 1000.f);
	
	WCHAR text[256] = L"";
	switch (_state->GetState())
	{
	case eStateType::ET_IDLE:
		wsprintf(text, L"HP %d\nCT %d\nIDLE", _hp, coolTime);
		break;
	case eStateType::ET_MOVE:
		wsprintf(text, L"HP %d\nCT %d\nMOVE", _hp, coolTime);
		break;
	case eStateType::ET_ATTACK:
		wsprintf(text, L"HP %d\nCT %d\nATTACK", _hp, coolTime);
		break;
	case eStateType::ET_DEFENSE:
		wsprintf(text, L"HP %d\nCT %d\nDEFENSE", _hp, coolTime);
		break;
	case eStateType::ET_DEAD:
		wsprintf(text, L"HP %d\nCT %d\nDEAD", _hp, coolTime);
		break;
	case eStateType::ET_NONE:
		wsprintf(text, L"HP %d\nCT %d\nNONE", _hp, coolTime);
		break;
	default:
		break;
	}
	printf("%s", text);
	_font->SetText(text);
}
TileCell* Character::GetTargetCell()
{
	return _targetTileCell;
}
void Character::SetTargetTileCell(TileCell* tileCell)
{
	_targetTileCell = tileCell;
	_state->NextState(eStateType::ET_PATHFINDING);
}
void Character::SetDirection(eDirection direction)
{
	_currentDirection = direction;
}
void Character::ClearPathTileCellStack()
{
	while (0 != _pathTileCellStack.size())
	{
		_pathTileCellStack.pop();
	}
}