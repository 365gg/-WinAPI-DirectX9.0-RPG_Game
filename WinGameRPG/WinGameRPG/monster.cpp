#include"Monster.h"
Monster::Monster(LPCWSTR name, LPCWSTR scriptName, LPCWSTR pngName) : Character(name, scriptName, pngName)
{
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
		Component* findEnemy = NULL;
		{
			component->GetType() == eComponentType::CT_NPC or eComponentType::CT_PLAYER
				findEnemy = component;
			break;
		}
		if (NULL != findEnemy)
		{
			//�߰�
			eDirection direction = eDirection::NONE;
			MoveStart(direction);
		}
		else
		{
			Character::UpdateAI();
		}
	}
}