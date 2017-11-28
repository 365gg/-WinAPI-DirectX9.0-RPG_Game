#include"Map.h"
#include "Sprite.h"
#include"TileCell.h"
#include"TileObject.h"
#include"GameSystem.h"
#include"LifeTileObject.h"
#include<fstream>
Map::Map(wstring name) : Component(name)
{
	_startX = _startY = _deltaX = _deltaY = 0.0f;
	_tileSize = 32;
	_spriteList.clear();
	_viewer = NULL;
}
Map::~Map()
{

}
void Map::Init()
{
	//32x32 ũ�� Ÿ��
	//512x512���� ũ�⸦ ���� ������ Ÿ�� ����

	int srcX = 0;
	int srcY = 0;
	for (int y = 0; y < 16; y++)
	{
		for (int x = 0; x < 16; x++)
		{
			Sprite* sprite = new Sprite(L"MapSprite.png", L"MapSprite.json");
			sprite->Init(srcX, srcY, 32, 32, 1.0f);
			_spriteList.push_back(sprite);
			srcX += 32;
		}
		srcX = 0;
		srcY += 32;
	}
	std::wstring wname = _name;
	std::string name = "";
	name.assign(wname.begin(), wname.end());
	char layer01Name[256];
	sprintf(layer01Name, "%sData_layer1.csv", name.c_str());
	char layer02Name[256];
	sprintf(layer02Name, "%sData_layer2.csv", name.c_str());
	//Load Script Script 1��
	{
		char record[1024];
		int line = 0;
		int index = 0;
		ifstream infile(layer01Name);
		while (!infile.eof())
		{
			infile.getline(record, 1024);

			char* token = strtok(record, ",");
			switch (line)
			{
			case 0:
				if (NULL != token)
				{
					token = strtok(NULL, ",");
					_width = atoi(token);
					token = strtok(NULL, ",");
					_height = atoi(token);
				}
				break;
			case 1:
				break;
			default:
				//map data read
				if (NULL != token)
				{
					vector<TileCell*> rowList;
					for (int x = 0; x < _width; x++)
					{
						index = atoi(token);

						TileCell* tileCell = new TileCell();
						WCHAR componetSetName[256];
						wsprintf(componetSetName, L"map_layer01_%d_%d", line, x);
						wstring componentName = componetSetName;
						TileObject* tileObject = new TileObject(componentName, _spriteList[index]);
						tileObject->SetCanMove(true);
						tileCell->AddComponent(tileObject, true);
						rowList.push_back(tileCell);
						token = strtok(NULL, ",");
					}
					_tileMap.push_back(rowList);
				}
				break;
			}
			line++;
		}
	}
	{

	}
	//Load Script Script 2��
	{
		char record[1024];
		int line = 0;
		int row = 0;
		int index = 0;
		ifstream infile(layer02Name);
		while (!infile.eof())
		{
			infile.getline(record, 1024);

			char* token = strtok(record, ",");
			switch (line)
			{
			case 0:
				break;
			case 1:
				break;
			default:
				//map data read
				if (NULL != token)
				{
					vector<TileCell*> rowList = _tileMap[row];
					for (int x = 0; x < _width; x++)
					{
						index = atoi(token);
						TileCell* tileCell = rowList[x];
						WCHAR componetName[256];
						wsprintf(componetName, L"map_layer02_%d_%d", line, x);
						if (0 <= index)
						{
							if (100100 == index)
							{
								LifeTileObject* tileObject = new LifeTileObject(x, row, componetName, _spriteList[17]);
								tileCell->AddComponent(tileObject, true);
								tileObject->SetCanMove(true);
							}
							else
							{
								TileObject* tileObject = new TileObject(componetName, _spriteList[index]);
								tileCell->AddComponent(tileObject, true);
							}
						}
						token = strtok(NULL, ",");
					}
					row++;
				}
				break;
			}
			line++;
		}
	}
}
void Map::DInit()
{
	for (int y = 0; y < _height; y++)
	{
		for (int x = 0; x < _width; x++)
		{
			_tileMap[y][x]->DInit();
			delete _tileMap[y][x];
		}
	}
	/*if (NULL != _viewer)
	{
		delete _viewer;
		_viewer = NULL;
	}*/
}
void Map::Update(float deltaTime)
{
	for (int y = 0; y < _height; y++)
	{
		for (int x = 0; x < _width; x++)
		{
			_tileMap[y][x]->MoveDeltaPosition(_deltaX, _deltaY);
			_tileMap[y][x]->Update(deltaTime);
		}
	}
}
void Map::Render()
{
	int midX = GameSystem::GetInstance()->GetWindowWidth() / 2;
	int midY = GameSystem::GetInstance()->GetWindowHeight() / 2;
	
	int minX = _viewer->GetTileX() - (midX / _tileSize) - 2;
	int maxX = _viewer->GetTileX() + (midX / _tileSize) + 2;
	int minY = _viewer->GetTileY() - (midX / _tileSize) - 2;
	int maxY = _viewer->GetTileY() + (midX / _tileSize) + 2;

	if (minX < 0)
	minX = 0;
	if (_width <= maxX)
	maxX = _width;
	if (minY < 0)
	minY = 0;
	if (_height <= maxY)
	maxY = _height;

	for (int y = minY; y < maxY; y++)
	{
		for (int x = minX; x < maxX; x++)
		{
			_tileMap[y][x]->Render();
		}
	}
}
void Map::Release()
{
	for (int y = 0; y < _height; y++)
	{
		for (int x = 0; x < _width; x++)
		{
			_tileMap[y][x]->Release();
		}
	}
}
void Map::Reset()
{
	for (int y = 0; y < _height; y++)
	{
		for (int x = 0; x < _width; x++)
		{
			_tileMap[y][x]->Reset();
		}
	}
}
void Map::Scroll(float moveX, float moveY)
{
	_deltaX = moveX;
	_deltaY = moveY;
}
float Map::GetPositionX(int tileX, int tileY)
{
	return _tileMap[tileY][tileX]->GetPositionX();
}
float Map::GetPositionY(int tileX, int tileY)
{
	return _tileMap[tileY][tileX]->GetPositionY();
}

bool Map::GetTileCollisionList(int tileX, int tileY, std::list<Component*>& collisionList)
{
	if (_width <= tileX)
		return false;
	if (tileX < 0)
		return false;
	if (_height <= tileY)
		return false;
	if (tileY < 0)
		return false;
	return _tileMap[tileY][tileX]->GetCollisionList(collisionList);
}
std::list<Component*>  Map::GetTileComponentList(int tileX, int tileY)
{
	return _tileMap[tileY][tileX]->GetComponentList();
}
void Map::setTileComponent(int tileX, int tileY, Component* component, bool isRender)
{
	_tileMap[tileY][tileX]->AddComponent(component, isRender);
}
void Map::ResetTileComponent(int tileX, int tileY, Component* component)
{
	_tileMap[tileY][tileX]->RemoveComponent(component);
}
bool Map::CanMoveTileMap(int tileX, int tileY)
{
	if (_width <= tileX)
		return false;
	if (tileX < 0)
		return false;
	if (_height <= tileY)
		return false;
	if (tileY < 0)
		return false;
	return _tileMap[tileY][tileX]->CanMove();
}
int Map::GetWidth()
{
	return _width;
}
int Map::GetHeight()
{
	return _height;
}
void Map::InitViewer(Component* viewer)
{
	_viewer = viewer;
	//����߽� ������ ���� ���ϱ���
	int midX = GameSystem::GetInstance()->GetWindowWidth() / 2;
	int midY = GameSystem::GetInstance()->GetWindowHeight() / 2;

	_startX = (-_viewer->GetTileX() * _tileSize) + midX + _tileSize / 2;
	_startY = (-_viewer->GetTileY() * _tileSize) + midY + _tileSize / 2;
	float posX = _startX;
	float posY = _startY;
	for (int y = 0; y < _height; y++)
	{
		for (int x = 0; x < _width; x++)
		{
			_tileMap[y][x]->SetPosition(posX, posY);
			posX += _tileSize;
		}
		posX = _startX;
		posY += _tileSize;
	}
}
void Map::ViewerScroll(Component* viewer, float deltaX, float deltaY)
{
	if (NULL != _viewer)
	{
		if (_viewer->GetType() == viewer->GetType())
		{
			Scroll(-deltaX, -deltaY);
		}
	}
}