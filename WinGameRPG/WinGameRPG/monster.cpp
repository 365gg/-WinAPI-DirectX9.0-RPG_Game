#include"Monster.h"
#include"ComponentSystem.h"
#include"Map.h"
#include "sComponentMsgParam.h"
#include "MoveState.h"
#include "GameSystem.h"
#include "Stage.h"
Monster::Monster(wstring name, wstring scriptName, wstring pngName) : Character(name, scriptName, pngName)
{
	_componentType = eComponentType::CT_MONSTER;
	int speed = (rand() % 1500) + 200;
	_moveTime = (float)speed / 1000.0f;
}

Monster::~Monster()
{
}
void Monster::UpdateAI()
{
	Map* map = GameSystem::GetInstance()->GetStage()->GetMap();
	std::vector<eComponentType> compareTypeList;
	compareTypeList.push_back(eComponentType::CT_NPC);
	compareTypeList.push_back(eComponentType::CT_PLAYER);
	Component* findEnemy = ComponentSystem::GetInstance()->FindComponentInRange(map, this, 4, compareTypeList);
	if (NULL != findEnemy)
	{
		//�߰�
		SetTargetTileCell(map->GetTileCell(findEnemy->GetTileX(), findEnemy->GetTileY()));
		////_moveTime = 0.5f;
		// �� ��û�ϴϱ� ���� ���� �Ϸ�
		/*eDirection direction = eDirection::NONE;
		if (findEnemy->GetTileX() < _tileX)
			direction = eDirection::LEFT;
		if (findEnemy->GetTileX() > _tileX)
			direction = eDirection::RIGHT;
		if (findEnemy->GetTileY() < _tileY)
			direction = eDirection::UP;
		if (findEnemy->GetTileY() > _tileY)
			direction = eDirection::DOWN;
		_currentDirection = direction;
		_state->NextState(eStateType::ET_MOVE);*/
	}
	else
	{
		Character::UpdateAI();
	}
}
Component* Monster::Collision(std::list<Component*>& collisionList)
{
	for (std::list<Component*>::iterator it = collisionList.begin(); it != collisionList.end(); it++)
	{
		if ((*it)->GetType() == eComponentType::CT_NPC ||
			(*it)->GetType() == eComponentType::CT_PLAYER)
		{
			_targetComponent = (*it);
			//_state->NextState(eStateType::ET_ATTACK);
			return (*it);
		}
	}
	return NULL;
}