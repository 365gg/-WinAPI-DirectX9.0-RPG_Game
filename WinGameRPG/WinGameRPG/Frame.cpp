#include "Frame.h"
#include "GameSystem.h"
#include "Texture.h"
Frame::Frame() :_frameDelay(0.0f)
{
}

Frame::~Frame()
{
}
void Frame::Init(Texture* texture, int left, int top, int width, int height, float rotate, float frameDelay)
{
	_texture = texture;
	_sprite = GameSystem::GetInstance()->GetSprite();

	_width = width;
	_height = height;
	_rotate = rotate;
	_srcTextureRect.left = left;
	_srcTextureRect.top = top;
	_srcTextureRect.right = left + _width;
	_srcTextureRect.bottom = top + _height;

	_textureColor = D3DCOLOR_ARGB(255, 255, 255, 255);
	
	_frameDelay = frameDelay;
}
void Frame::DInit()
{

}
void Frame::Render()
{
	//��������Ʈ ��� �� ��� ����(��ġ,ũ��,ȸ��)
	D3DXVECTOR2 spriteCenter = D3DXVECTOR2((float)_width / 2.0f, (float)_height / 2.0f);
	D3DXVECTOR2 translate = D3DXVECTOR2(_x - (float)_width / 2.0f, _y - (float)_height / 2.0f);
	D3DXVECTOR2 scaling = D3DXVECTOR2(1.0f, 1.0f);

	D3DXMATRIX matrix;
	D3DXMatrixTransformation2D(
		&matrix, 
		NULL, 
		0.0f, 
		&scaling, 
		&spriteCenter, 
		_rotate, 
		&translate);

	_sprite->SetTransform(&matrix);
	
	_sprite->Draw(_texture->GetTextureDX(), &_srcTextureRect, NULL, NULL, _textureColor);
}
void Frame::Release()
{
	RELEASE_COM(_texture);
}
void Frame::Reset(Texture* texture, int left, int top, int width, int height, float rotate, float frameDelay)
{
	Init(texture,left,top, width, height, rotate, frameDelay);
}
float Frame::GetFrameDelay()
{
	return _frameDelay;
}
void Frame::SetPosition(float x, float y)
{
	_x = x;
	_y = y;
}