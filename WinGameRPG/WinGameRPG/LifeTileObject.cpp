#include"LifeTileObject.h"
#include"GameSystem.h"
#include"Map.h"
#include"Stage.h"
#include"ComponentSystem.h"
LifeTileObject::LifeTileObject(LPCWSTR name, Sprite* sprite) :
	TileObject(name,sprite)
{
}

LifeTileObject::~LifeTileObject()
{

}
void LifeTileObject::Update(float deltaTime)
{
	TileObject::Update(deltaTime);
	Map* map = GameSystem::GetInstance()->GetStage()->GetMap();
	int range = 1;
	//�ֺ� ĳ���� ���� ���� ����
	std::vector<eComponentType> compareTypeList;
	//FindComponentInRange �� ����� ���Ͽ� ĳ���� ���� �� ������ ��(�ڽ��� ������ Ž��)
	Component* findEnemy;

	int minX = GetTileX() - range;
	int minY = GetTileY() - range;
	int maxX = GetTileX() + range;
	int maxY = GetTileY() + range;
	if (minX < 0) {
		minX = 0;
	}
	if (minY < 0) {
		minY = 0;
	}
	if (map->GetWidth() <= maxX) {
		maxX = map->GetWidth() - 1;
	}
	if (map->GetHeight() <= maxY) {
		maxY = map->GetHeight() - 1;
	}
	for (int y = minY; y < maxY; y++)
	{
		for (int x = minX; x < maxX; x++)
		{
			std::list<Component*> collisionList;
			if (false == map->GetTileCollisionList(x, y, collisionList))
			{
				//collisionList ��ȯ
				for (std::list<Component*>::iterator itCollision = collisionList.begin();
					itCollision != collisionList.end();
					itCollision++)
				{
					Component* component = (*itCollision);
					if (component->IsLive())
					{
						for (int i = 0; i < compareTypeList.size(); i++)
						{
							if (component->GetType() == compareTypeList[i])
							{
								compareTypeList.push_back(component->GetType());
							}
						}
					}
				}
			}
		}
	}
}