#pragma once 

#include "cocos2d.h"
USING_NS_CC;

#define KIND_ID_BAIJIALE	122
#define KIND_ID_LKBY		3010

class PhotoShow : public Layer
{
public:
	CREATE_FUNC(PhotoShow);
	virtual bool init();
	PhotoShow();
	~PhotoShow();


	void initView(int num, float radius);

	bool onTouchBegan(Touch* touch, Event* event);
	void onTouchMoved(Touch* touch, Event* event);
	void onTouchEnded(Touch* touch, Event* event);

	Vec2 transformPoint(Vec2 vec, float z);
	void sortVector();
	void nextScene();

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
	float m_radians;
	int count;
	int count1;

	int m_num;
	float m_radius;
};