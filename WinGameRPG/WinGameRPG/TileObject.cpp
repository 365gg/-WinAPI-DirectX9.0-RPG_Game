#include"TileObject.h"
#include"Sprite.h"
TileObject::TileObject(wstring name, Sprite* sprite, int tileX, int tileY)
	: Component(name)
{
	_sprite = sprite;
	_posX = _posY = 0.0f;
	_componentType = eComponentType::CT_TILE_OBJECT;
	_tileX = tileX;
	_tileY = tileY;
	_distanceWeight = 1.0f;
}

TileObject::~TileObject()
{

}
void TileObject::Init()
{
	_sprite->Init();
}
void TileObject::DInit()
{
	_sprite->DInit();
}
void TileObject::Update(float deltaTime)
{
	_sprite->Update(deltaTime);
}
void TileObject::Render()
{
	_sprite->SetPosition(_posX, _posY);
	_sprite->Render();
}
void TileObject::Release()
{
	_sprite->Release();
}
void TileObject::Reset()
{
	_sprite->Reset();
}
void TileObject::MoveDeltaPosition(float deltaX, float deltaY)
{
	_posX += deltaX;
	_posY += deltaY;
	_sprite->SetPosition(_posX, _posY);
}
void TileObject::SetPosition(float posX, float posY)
{
	_posX = posX;
	_posY = posY;
}