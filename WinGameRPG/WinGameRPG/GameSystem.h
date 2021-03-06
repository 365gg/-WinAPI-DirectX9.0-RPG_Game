#pragma once
#include <Windows.h>
#include<D3DX9.h>
#include<D3D9.h>
#include<vector>
#include<list>
#define RELEASE_COM(x) {if(x){x->Release();x=NULL;}}
#define SAVE_DELETE(x) {if(x){delete x;x=NULL;}}

class GameTimer;
class Component;
class Stage;
//class Font;
class GameSystem
{
private:
	static GameSystem* _instance;
	GameSystem();
public:
	~GameSystem();
	static GameSystem* GetInstance();
private:
	D3DPRESENT_PARAMETERS _d3dpp;
	UINT _4xMsaaQuality;
	HWND _hMainWnd;
	LPDIRECT3DDEVICE9 _device3d;
	LPD3DXSPRITE _sprite;

	bool _isFullScreen;
public:
	bool InitSystem(HINSTANCE hInstance, int nCmdShow);
	int Update();
	bool InitDirect3D();
	void CheckDeviceLost();
private:
	int _WindowWidth;
	int _WindowHeight;
	GameTimer* _gameTimer;

//get System
public:
	int GetWindowWidth();
	int GetWindowHeight();
	LPD3DXSPRITE GetSprite();
	LPDIRECT3DDEVICE9 GetDevice3d();
//Input System
public:
	enum eKeyState {
		KEY_DOWN,
		KEY_UP,
	};
private:
	bool _isMouseDown;
public:
	bool GetMouseDown() { return _isMouseDown; }
	void MouseDown(int mouseX, int mouseY);
	void MouseUp() { _isMouseDown = false; }
private:
	eKeyState _keyState[256];
	int _mouseX;
	int _mouseY;
public:
	int GetMouseX() { return _mouseX; }
	int GetMouseY() { return _mouseY; }

	void InitInput();
	void KeyDown(unsigned int keyCode);
	void KeyUp(unsigned int keyCode);
	bool IsKeyDown(int keyCode);
	//state
private:
	Stage* _stage;
public:
	Stage* GetStage() { return _stage; }
};