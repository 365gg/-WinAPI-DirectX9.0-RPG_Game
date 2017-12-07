#include "PathfinderStageLoader.h"
#include "Stage.h"
#include "Map.h"
#include<list>
#include "PathfinderPlayer.h"
PathfinderStageLoader::PathfinderStageLoader(Stage* stage) : StageLoader(stage)
{
}

PathfinderStageLoader::~PathfinderStageLoader()
{
}
void PathfinderStageLoader::CreateComponents()
{
	StageLoader::CreateComponents();
	Player* player = new PathfinderPlayer(L"player", L"player", L"player");
	_stage->AddStageComponent(player);
	
	int midTileX = _stage->GetMap()->GetWidth() / 2;
	int midTileY = _stage->GetMap()->GetHeight() / 2;
	std::list<Component*> comList = _stage->GetMap()->GetTileComponentList(midTileX, midTileY);
	_stage->GetMap()->InitViewer(comList.front());
}