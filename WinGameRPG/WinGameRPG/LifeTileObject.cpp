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
	//�ֺ� ĳ���� ���� ���� ����
	std::vector<eComponentType> compareTypeList;
	//FindComponentInRange �� ����� ���Ͽ� ĳ���� ���� �� ������ ��(�ڽ��� ������ Ž��)
	Component* findEnemy = ComponentSystem::GetInstance()->FindComponentInRange(map, this, 4, compareTypeList);
}