#include "PathfinderPlayer.h"
#include "IdleState.h"
#include "PathfindingState.h"
PathfinderPlayer::PathfinderPlayer(wstring name, wstring scriptName, wstring pngName) : Player(name, scriptName, pngName)
{
}

PathfinderPlayer::~PathfinderPlayer()
{
}
void PathfinderPlayer::UpdateAI()
{

}
void PathfinderPlayer::InitState()
{
	Player::InitState();
	ReplaceState(eStateType::ET_ATTACK, new IdleState());
	ReplaceState(eStateType::ET_PATHFINDING, new PathfindingState());
}