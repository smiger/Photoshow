#include "PhotoShow.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"

PhotoShow::PhotoShow()
	:_camera(nullptr)
	,isMove(false)
	,isTouchMoved(true)
	,count(0)
	,m_angle(90)
	,dx(-1.0f)
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
	if(!Layer::init())
		return false;
	size = Director::getInstance()->getWinSize();

	auto layer3D = Layer::create();
	addChild(layer3D, 0);
	_layer3D = layer3D;

	if(_camera == nullptr)
	{
		_camera = Camera::createPerspective(60, (GLfloat)size.width / size.height, 1, 2000);
		_camera->setCameraFlag(CameraFlag::USER1);
		_camera->setPosition3D(Vec3(size.width/2, size.height/2, 500.f));
		_camera->lookAt(Vec3(size.width/2, size.height/2, 0.f));
		_layer3D->addChild(_camera);
	}



	

	//Ìí¼Ó´¥ÃþÏìÓ¦
	auto _listener = EventListenerTouchOneByOne::create();
	_listener->setSwallowTouches(true);
	_listener->onTouchBegan = CC_CALLBACK_2(PhotoShow::onTouchBegan, this);
	_listener->onTouchMoved = CC_CALLBACK_2(PhotoShow::onTouchMoved, this);
	_listener->onTouchEnded = CC_CALLBACK_2(PhotoShow::onTouchEnded, this);

	float r = 200;
	int num = 5;
	float degree = CC_DEGREES_TO_RADIANS(360 / num);
	float angle = 0;
	float y = 10;

	for(int i = 0; i < num; i++)
	{
		angle = degree * i;
		float x = r * cosf(angle);
		float z = r * sinf(angle);
		//float z = -100;
		char filename[10] = {0};
		sprintf(filename, "%d.png", i + 1);
		bill = BillBoard::create(filename);
		bill->setPosition3D(Vec3(size.width/2 + x, size.height/2 + y, z));
		log("bill %f,%f,%f",size.width/2 + x, size.height/2 + y, z);
		bill->setScale(0.4f);
		bill->setName("bill");
		spriteVec.pushBack(bill);
		auto sp3d = Sprite3D::create();
		sp3d->addChild(bill);
		sp3d->setTag(i + 1);
		_layer3D->addChild(sp3d);
		
	}
	_eventDispatcher->addEventListenerWithSceneGraphPriority(_listener, this);
	auto background = Sprite::create("bk.jpg");
	background->setPosition3D(Vec3(size.width/2, size.height/2, -250));
	background->setScale(1.2f);
	auto sp3d = Sprite3D::create();
	sp3d->addChild(background);
	_layer3D->addChild(sp3d);
	//sortVector();
	_layer3D->setCameraMask(2);

	scheduleUpdate();

	return true;
}

bool PhotoShow::onTouchBegan(Touch* touch, Event* event)
{
	 if(isTouchMoved || isMove)
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
	log("movedx = %f",dx);
	if(abs(dx) < 5)
		return;
	m_angle = 360 / spriteVec.size();
	isTouchMoved = true;
}

void PhotoShow::onTouchEnded(Touch* touch, Event* event)
{
	//auto target = static_cast<Sprite*>(event->getCurrentTarget());
	log("sprite3d onTouchesEnded.. ");
	//target->setOpacity(255);
	for(int i = 0; i < spriteVec.size(); i++)
	{
		spriteVec.at(i)->setColor(Color3B::GRAY);
	}
	
	if(!isTouchMoved)
	{
		clickTarget->setColor(Color3B::WHITE);
		auto angle = Vec2(clickTarget->getPositionZ(), clickTarget->getPositionX() - size.width/2).getAngle();
		dx = -CC_RADIANS_TO_DEGREES(angle);
		m_angle = abs(dx);
		log("enddx %f",dx);
		if(m_angle > 2)
			isMove = true;
	}
}

cocos2d::Vec2 PhotoShow::transformPoint(Vec2 vec, float z)
{
	Vec3 nearP(vec.x, vec.y, -1.0f), farP(vec.x, vec.y, 1.0f);

	auto size = Director::getInstance()->getWinSize();
	_camera->unproject(size, &nearP, &nearP);
	_camera->unproject(size, &farP, &farP);
	Vec3 dir(farP - nearP);
	float dist=0.0f;
	float ndd = Vec3::dot(Vec3(0,0,1),dir);
	if(ndd == 0)
		dist=0.0f;
	float ndo = Vec3::dot(Vec3(0,0,1),nearP);
	dist= (0 - ndo + z) / ndd;
	Vec3 p =   nearP + dist *  dir;

	// if( p.x > 100)
	//	 p.x = 100;
	// if( p.x < -100)
	//	 p.x = -100;
	//if( p.z > 100)
	//	 p.z = 100;
	//if( p.z < -100)
	//	 p.z = -100;

	_targetPos=p;
	//log("target=%f,%f,%f",_targetPos.x,_targetPos.y,_targetPos.z);
	return Vec2(_targetPos.x,_targetPos.y);
}

void PhotoShow::sortVector()
{
	for(int i = 0; i < spriteVec.size() - 1; i++)
	{
		for(int j = 1; j < spriteVec.size() - i; j++)
		{
			if(spriteVec.at(j - 1)->getPositionZ() < spriteVec.at(j)->getPositionZ())
			{
				spriteVec.swap(spriteVec.at(j - 1),spriteVec.at(j));
			}
		}
	}
	for(int i = 0; i < spriteVec.size(); i++)
	{
		spriteVec.at(i)->setColor(Color3B::GRAY);
	}
	spriteVec.at(0)->setColor(Color3B::WHITE);
	auto angle = Vec2(spriteVec.at(0)->getPositionZ(), spriteVec.at(0)->getPositionX() - size.width/2).getAngle();
	dx = -CC_RADIANS_TO_DEGREES(angle);
	m_angle = abs(dx);
	log("sort %f",dx);
	if(m_angle > 2)
		isMove = true;

}

void PhotoShow::update(float delta)
{
	if(!isTouchMoved && !isMove)
	{
		return;
	}
	if(count >= m_angle)
	{
		count = 0;
		isTouchMoved = false;
		isMove = false;
		sortVector();
		return;
	}


	static float accAngle = 0.f;
	float radius = 200.f;
	float time = 60.f;

	int inc = m_angle / time;
	//log("inc =%d",inc);
	dx < 0 ? inc = 2 : inc = -2;
	for(int i = 0; i < spriteVec.size(); i++)
	{
		auto sprite = spriteVec.at(i);
		auto angle = Vec2(sprite->getPositionX() - size.width/2,sprite->getPositionZ()).getAngle();
		auto degree = CC_RADIANS_TO_DEGREES(angle);
		//log("%f,%f,%f",sp3d->getPositionX(),sp3d->getPositionZ(),degree);
		accAngle =  CC_DEGREES_TO_RADIANS(degree + inc);
		float x = cosf(accAngle) * radius, z = sinf(accAngle) * radius;
		sprite->setPositionX(size.width/2 + x);
		sprite->setPositionZ(z);
	}
	count += abs(inc);
}
