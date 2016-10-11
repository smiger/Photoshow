#pragma once 

#include "cocos2d.h"

USING_NS_CC;

class PhotoShow : public Layer
{
public:
	CREATE_FUNC(PhotoShow);
	virtual bool init();
	PhotoShow();
	~PhotoShow();

	bool onTouchBegan(Touch* touch, Event* event);
	void onTouchMoved(Touch* touch, Event* event);
	void onTouchEnded(Touch* touch, Event* event);

	Vec2 transformPoint(Vec2 vec, float z);
	void sortVector();

	void update(float delta) override;
private:
	Size size;
	Layer* _layer3D;
	Camera* _camera;
	Vec3 _targetPos;
	Sprite* bill;
	Vector<Sprite*> spriteVec;
	Sprite* clickTarget;

	float dx;
	bool isMove;
	bool isTouchMoved;
	bool isClicked;
	float m_angle;
	float count;
};