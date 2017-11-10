#include"Monster.h"
#include"ComponentSystem.h"
#include"Map.h"
Monster::Monster(LPCWSTR name, LPCWSTR scriptName, LPCWSTR pngName) : Character(name, scriptName, pngName)
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
	if (false == _isMoving)
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
		//Component* findEnemy = NULL;
		//int range = 4;
		//int minX = _tileX - range;
		//int minY = _tileY - range;
		//int maxX = _tileX + range;
		//int maxY = _tileY + range;
		//if (minX < 0) {
		//	minX = 0;
		//}
		//if (minY < 0) {
		//	minY = 0;
		//}
		//if (map->GetWidth() <= maxX) {
		//	maxX = map->GetWidth() - 1;
		//}
		//if (map->GetHeight() <= maxY) {
		//	maxY = map->GetHeight() - 1;
		//}
		//{
		//	for (int y = minY; y < maxY; y++)
		//	{
		//		for (int x = minX; x < maxX; x++)
		//		{
		//			std::list<Component*> collisionList;
		//			if (false == map->GetTileCollisionList(x, y, collisionList))
		//			{
		//				//collisionList ��ȯ
		//				for (std::list<Component*>::iterator it = collisionList.begin();
		//					it != collisionList.end();
		//					it++)
		//				{
		//					Component* component = (*it);
		//					if (component->GetType() == eComponentType::CT_NPC ||
		//						component->GetType() == eComponentType::CT_PLAYER)
		//					{
		//						findEnemy = component;
		//						break;
		//					}
		//				}
		//			}
		//		}
		//	}
		//}
		std::vector<eComponentType> compareTypeList;
		compareTypeList.push_back(eComponentType::CT_NPC);
		compareTypeList.push_back(eComponentType::CT_PLAYER);
		Component* findEnemy = ComponentSystem::GetInstance()->FindComponentInRange(this, 4, compareTypeList);
		if (NULL != findEnemy)
		{
			//_moveTime = 0.5f;
			//�߰�
			eDirection direction = eDirection::NONE;
			/*int distanceX = _tileX - findEnemy->GetTileX();
			int distanceY = _tileY - findEnemy->GetTileY();
			int randXY = rand() % 2;
			if (randXY)
			{
				distanceY = _tileY - findEnemy->GetTileY();
				if (distanceY < 0)
					direction = eDirection::DOWN;
				else
					direction = eDirection::UP;
			}
			else
			{
				distanceX = _tileX - findEnemy->GetTileX();
				if (distanceX < 0)
					direction = eDirection::RIGHT;
				else
					direction = eDirection::LEFT;
			}*/
			if (findEnemy->GetTileX() < _tileX)
				direction = eDirection::LEFT;
			if (findEnemy->GetTileX() > _tileX)
				direction = eDirection::RIGHT;
			if (findEnemy->GetTileY() < _tileY)
				direction = eDirection::UP;
			if (findEnemy->GetTileY() > _tileY)
				direction = eDirection::DOWN;
			MoveStart(direction);
		}
		else
		{
			//_moveTime = 1.0f;
			Character::UpdateAI();
		}
	}
}