#include"NPC.h"
#include"Map.h"
#include"ComponentSystem.h"
#include"sComponentMsgParam.h"
#include"MoveState.h"
NPC::NPC(LPCWSTR name, LPCWSTR scriptName, LPCWSTR pngName) : Character(name, scriptName, pngName)
{
	_componentType = eComponentType::CT_NPC;
	int speed = (rand() % 1500) + 400;
	_moveTime = (float)speed / 1000.0f;
}

NPC::~NPC()
{
}
void NPC::UpdateAI()
{
	if (false == _isLive)
		return;
	if (false == _state->IsMoving())
	{
		/*
		���� �������� ��ĭ�� Ÿ�� �˻����� ����
		�˻��ϴ� Ÿ�� ���� ���Ѵ�
		������ ��� Ÿ���� �˻�
		npc or player ������
		���� ã�Ҵ�
		��ã����
		��� rand ������
		*/
		Map* map = (Map*)ComponentSystem::GetInstance()->FindComponent(L"tileMap");
		std::vector<eComponentType> compareTypeList;
		compareTypeList.push_back(eComponentType::CT_MONSTER);
		Component *findEnemy = ComponentSystem::GetInstance()->FindComponentInRange(this, 2, compareTypeList);
		if (NULL != findEnemy)
		{
			//_moveTime = 0.3f;
			//�߰�
			eDirection direction = eDirection::NONE;
			for (int findDir = 0; findDir < 4; findDir++)
			{
				int newTileX = _tileX;
				int newTileY = _tileY;
				switch (findDir)
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
				}
				if (map->CanMoveTileMap(newTileX, newTileY))
				{
					direction = (eDirection)findDir;
					_currentDirection = direction;
					_state->Start();
					break;
				}
			}
		}
		else
		{
			//_moveTime = 1.0f;
			Character::UpdateAI();
		}
	}
}