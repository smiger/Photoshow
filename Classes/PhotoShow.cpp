#include "PhotoShow.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "../NetWork/HandlerGame.h"
#include "../GamePacket/CMD_Plazz.h"

PhotoShow::PhotoShow()
	:_camera(nullptr)
	, isMove(false)
	, isTouchMoved(true)
	, count(0)
	, count1(0)
	, m_angle(180)
	, dx(-1.0f)
{
}
PhotoShow::~PhotoShow()
{
	if (_camera)
	{
		_camera = nullptr;
	}
}


bool PhotoShow::init()
{
	if (!Layer::init())
		return false;

	initView(3, 300);
	scheduleUpdate();

	return true;
}

void PhotoShow::initView(int num, float radius)
{
	m_num = num;
	m_radius = radius;

	size = Director::getInstance()->getWinSize();


	auto layer3D = Layer::create();
	addChild(layer3D, 0);
	_layer3D = layer3D;

	if (_camera == nullptr)
	{
		_camera = Camera::createPerspective(60, (GLfloat)size.width / size.height, 1, 2000);
		_camera->setCameraFlag(CameraFlag::USER1);
		_camera->setPosition3D(Vec3(size.width / 2, size.height / 2, 600.f));
		_camera->lookAt(Vec3(size.width / 2, size.height / 2, 0.f));
		_layer3D->addChild(_camera);
	}

	//添加触摸响应
	auto _listener = EventListenerTouchOneByOne::create();
	_listener->setSwallowTouches(true);
	_listener->onTouchBegan = CC_CALLBACK_2(PhotoShow::onTouchBegan, this);
	_listener->onTouchMoved = CC_CALLBACK_2(PhotoShow::onTouchMoved, this);
	_listener->onTouchEnded = CC_CALLBACK_2(PhotoShow::onTouchEnded, this);

	m_radians = CC_DEGREES_TO_RADIANS(270 / (m_num - 2));
	float radians = 0;
	float y = 10;

	Vector<SpriteFrame*> spriteFrameVec;
	auto spriteFrameCache = SpriteFrameCache::getInstance();
	spriteFrameCache->addSpriteFramesWithFile("animations/anim.plist", "animations/anim.png");
	int frameNum = 11;
	for (int i = 0; i < m_num; i++)
	{

		if (i < 3)
		{
			radians = CC_DEGREES_TO_RADIANS(45) * (i + 1);
		}
		else
			radians = CC_DEGREES_TO_RADIANS(135) + m_radians * (i - 2);

		float x = m_radius * cosf(radians);
		float z = m_radius * sinf(radians);
		//float z = -100;
		//char filename[10] = {0};
		//sprintf(filename, "bg.png", i + 1);
		bill = BillBoard::createWithSpriteFrameName("bg.png");
		bill->setPosition3D(Vec3(size.width / 2 + x, size.height / 2 + y, z));
		log("bill %f,%f,%f", size.width / 2 + x, size.height / 2 + y, z);
		bill->setScale(0.4f);
		bill->setName("bill");


		char buf[64] = { 0 };
		spriteFrameVec.clear();
		if (i == m_num - 1)
			frameNum = 4;
		for (int j = 0; j < frameNum; ++j)
		{
			sprintf(buf, "anim_%02d_%02d.png", i, j);
			SpriteFrame* pSpriteFrame = spriteFrameCache->getSpriteFrameByName(buf);
			spriteFrameVec.pushBack(pSpriteFrame);
		}
		auto animation = Animation::createWithSpriteFrames(spriteFrameVec, 0.1);
		animation->setDelayPerUnit(2.0f / (float)frameNum); //
		animation->setRestoreOriginalFrame(true); //是否回到第一帧
		animation->setLoops(-1);	//重复次数(-1:无限循环)


		sprintf(buf, "anim_%02d_00.png", i);
		auto sprite = Sprite::createWithSpriteFrameName(buf);
		sprintf(buf, "anim_%02d_text.png", i);
		auto text = Sprite::createWithSpriteFrameName(buf);
		text->setPosition(Vec2(sprite->getContentSize().width / 2, 50));
		sprite->addChild(text);
		sprite->setAnchorPoint(Vec2::ZERO);
		CCAnimate *action = CCAnimate::create(animation);
		sprite->runAction(action);
		bill->addChild(sprite);
		bill->setTag(i);

		spriteVec.pushBack(bill);
		auto sp3d = Sprite3D::create();
		sp3d->addChild(bill);
		sp3d->setTag(i);
		_layer3D->addChild(sp3d);

	}
	_eventDispatcher->addEventListenerWithSceneGraphPriority(_listener, this);

	//sortVector();
	_layer3D->setCameraMask(2);

	m_angle = 270 / (spriteVec.size() - 2);
}

bool PhotoShow::onTouchBegan(Touch* touch, Event* event)
{
	if (!(this->isVisible()))
		return false;
	if (isTouchMoved || isMove)
		return false;
	//auto location2 = touch->getLocation();

	auto location = touch->getLocationInView();
	for (int i = 0; i < spriteVec.size(); i++)
	{
		auto target = spriteVec.at(i);
		auto posZ = spriteVec.at(i)->getPositionZ();
		//log("posz = %f",posZ);
		auto pos = transformPoint(location, posZ);
		Rect rect = target->getBoundingBox();
		if (rect.containsPoint(pos))
		{
			//log("sprite3d began... x = %f, y = %f", touch->getLocation().x, touch->getLocation().y);
			//target->setOpacity(100);
			target->setColor(Color3B::WHITE);
			clickTarget = target;
			return true;
		}
	}

	return false;
}

void PhotoShow::onTouchMoved(Touch* touch, Event* event)
{
	dx = touch->getDelta().x;
	log("movedx = %f", dx);
	if (abs(dx) < 5)
		return;
	m_angle = 270 / (spriteVec.size() - 2);
	if (dx > 0)
	{
		if (spriteVec.at(1)->getPositionX() > spriteVec.at(2)->getPositionX())
		{
			spriteVec.swap(spriteVec.at(1), spriteVec.at(2));
		}
	}

	isTouchMoved = true;
}

void PhotoShow::onTouchEnded(Touch* touch, Event* event)
{
	//auto target = static_cast<Sprite*>(event->getCurrentTarget());
	log("sprite3d onTouchesEnded.. ");
	//target->setOpacity(255);
	for (int i = 0; i < spriteVec.size(); i++)
	{
		spriteVec.at(i)->setColor(Color3B::GRAY);
	}

	if (!isTouchMoved)
	{
		clickTarget->setColor(Color3B::WHITE);
		auto angle = Vec2(clickTarget->getPositionZ(), clickTarget->getPositionX() - size.width / 2).getAngle();
		dx = -CC_RADIANS_TO_DEGREES(angle);
		m_angle = 270 / (spriteVec.size() - 2);

		log("enddx %f", dx);
		if (abs(dx) > 2)
		{
			if (dx > 0)
			{
				if (spriteVec.at(1)->getPositionX() > spriteVec.at(2)->getPositionX())
				{
					spriteVec.swap(spriteVec.at(1), spriteVec.at(2));
				}
			}
			isMove = true;
		}
		else
		{
			log("enter.....%d", clickTarget->getTag());
			WORD KindID;
			switch (clickTarget->getTag())
			{
			case 0:
				//李逵捕鱼
				KindID = KIND_ID_LKBY;
				break;

			case 1:
				//欢乐30秒
				KindID = KIND_ID_BAIJIALE;
				break;

			default:
				break;
			}
			HandlerGame::getInstance(NET_PLAZZ)->sendData(MDM_MB_SERVER_LIST, SUB_MB_LIST_SERVER, &KindID, sizeof(KindID));
		}
	}
}

cocos2d::Vec2 PhotoShow::transformPoint(Vec2 vec, float z)
{
	Vec3 nearP(vec.x, vec.y, -1.0f), farP(vec.x, vec.y, 1.0f);

	auto size = Director::getInstance()->getWinSize();
	_camera->unproject(size, &nearP, &nearP);
	_camera->unproject(size, &farP, &farP);
	Vec3 dir(farP - nearP);
	float dist = 0.0f;
	float ndd = Vec3::dot(Vec3(0, 0, 1), dir);
	if (ndd == 0)
		dist = 0.0f;
	float ndo = Vec3::dot(Vec3(0, 0, 1), nearP);
	dist = (0 - ndo + z) / ndd;
	Vec3 p = nearP + dist *  dir;

	// if( p.x > 100)
	//	 p.x = 100;
	// if( p.x < -100)
	//	 p.x = -100;
	//if( p.z > 100)
	//	 p.z = 100;
	//if( p.z < -100)
	//	 p.z = -100;

	_targetPos = p;
	//log("target=%f,%f,%f",_targetPos.x,_targetPos.y,_targetPos.z);
	return Vec2(_targetPos.x, _targetPos.y);
}

void PhotoShow::sortVector()
{
	for (int i = 0; i < spriteVec.size() - 1; i++)
	{
		for (int j = 1; j < spriteVec.size() - i; j++)
		{
			if (spriteVec.at(j - 1)->getPositionZ() < spriteVec.at(j)->getPositionZ())
			{
				spriteVec.swap(spriteVec.at(j - 1), spriteVec.at(j));
			}
		}
	}
	for (int i = 0; i < spriteVec.size(); i++)
	{
		spriteVec.at(i)->setColor(Color3B::GRAY);
	}
	spriteVec.at(0)->setColor(Color3B::WHITE);
	if (spriteVec.at(1)->getPositionX() < spriteVec.at(2)->getPositionX())
	{
		spriteVec.swap(spriteVec.at(1), spriteVec.at(2));
	}
	//auto angle = Vec2(spriteVec.at(0)->getPositionZ(), spriteVec.at(0)->getPositionX() - size.width/2).getAngle();
	//dx = -CC_RADIANS_TO_DEGREES(angle);
	//m_angle = abs(dx);
	//log("sort %f",dx);
	//if(m_angle > 2)
	//	isMove = true;

}

void PhotoShow::nextScene()
{
	//	auto frameScene=FrameScene::createScene();
	//	Director::getInstance()->replaceScene(frameScene);
}

void PhotoShow::update(float delta)
{
	if (!isTouchMoved && !isMove)
	{
		return;
	}
	if (count >= 45 && count1 >= m_angle)
	{
		count = 0;
		count1 = 0;
		isTouchMoved = false;
		isMove = false;
		sortVector();
		return;
	}


	static float accAngle = 0.f;
	float time = 40.f;

	int inc = 45 / time;
	int inc1 = m_angle / time;
	if (inc1 < 1)
		inc1 = 1;

	dx < 0 ? inc = inc : inc = -inc;
	dx < 0 ? inc1 = inc1 : inc1 = -inc1;
	//log("inc =%d, %d, %f", inc, inc1, m_angle);
	for (int i = 0; i < 2; i++)
	{
		if (count >= 45)
			break;
		auto sprite = spriteVec.at(i);
		auto angle = Vec2(sprite->getPositionX() - size.width / 2, sprite->getPositionZ()).getAngle();
		auto degree = CC_RADIANS_TO_DEGREES(angle);
		//log("%f,%f,%f",sp3d->getPositionX(),sp3d->getPositionZ(),degree);
		accAngle = CC_DEGREES_TO_RADIANS(degree + inc);
		float x = cosf(accAngle) * m_radius, z = sinf(accAngle) * m_radius;
		sprite->setPositionX(size.width / 2 + x);
		sprite->setPositionZ(z);
	}
	for (int i = 2; i < spriteVec.size(); i++)
	{
		if (count1 >= m_angle)
			break;
		auto sprite = spriteVec.at(i);
		auto angle = Vec2(sprite->getPositionX() - size.width / 2, sprite->getPositionZ()).getAngle();
		auto degree = CC_RADIANS_TO_DEGREES(angle);
		//log("%f,%f,%f",sp3d->getPositionX(),sp3d->getPositionZ(),degree);
		accAngle = CC_DEGREES_TO_RADIANS(degree + inc1);
		float x = cosf(accAngle) * m_radius, z = sinf(accAngle) * m_radius;
		sprite->setPositionX(size.width / 2 + x);
		sprite->setPositionZ(z);
	}
	count += abs(inc);
	count1 += abs(inc1);
}
