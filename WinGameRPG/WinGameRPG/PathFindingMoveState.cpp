#include"PathFindingMoveState.h"
#include"Character.h"
#include"Map.h"
#include"ComponentSystem.h"
#include"GameSystem.h"
#include"Stage.h"
#include "GlobalType.h"
#include "TileCell.h"
PathFindingMoveState::PathFindingMoveState()
{
	_nowState = eStateType::ET_MOVE;
}

PathFindingMoveState::~PathFindingMoveState()
{
}
void PathFindingMoveState::Init(Character* character)
{
	State::Init(character);
	_nextState = eStateType::ET_NONE;
	_movingDuration = 0.0f;
}
void PathFindingMoveState::Update(float deltaTime)
{
	State::Update(deltaTime);
	if (false == _character->IsLive())
		return;
	if (_character->GetMoveTime() <= _movingDuration)
	{
		if (0 != _pathTileCellStack.size())
		{
			TileCell* tileCell = _pathTileCellStack.top();
			_pathTileCellStack.pop();

			//ĳ���Ͱ� ���� �ٲ㰡�� �̵�, �ʹ� ������ ���°� ��ġ��
			/*TilePosition to;
			TilePosition from;
			eDirection direction = GetDirection(to, from);
			_character->SetDirection(direction);*/

			_character->MoveStart(tileCell->GetTileX(), tileCell->GetTileY());
			_character->MoveStop();
		}
		//_nextState = eStateType::ET_IDLE;
	}
	else
	{
		_movingDuration += deltaTime;
	}
}
void PathFindingMoveState::Start()
{
	State::Start();
	
	_movingDuration = 0.0f;	//add code
	_nextState = eStateType::ET_NONE;
	_pathTileCellStack = _character->GetPathTileCellStack();
	if (0 != _pathTileCellStack.size())
	{
		_pathTileCellStack.pop();
	}
}
void PathFindingMoveState::Stop()
{
	State::Stop();
	Map* map = GameSystem::GetInstance()->GetStage()->GetMap();
	map->ViewerScroll(_character, 0.0f, 0.0f);
}