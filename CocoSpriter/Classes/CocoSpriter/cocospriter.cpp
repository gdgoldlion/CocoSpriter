/****************************************************************************
Copyright (c) 2013 , goldlion (gdgoldlion@gmail.com)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#include "cocospriter.h"
#include <sstream>

namespace cocospriter
{
	static const int NONE_DATA = -1;

	Folder::Folder( xml_node &folder_node )
	{
		id = folder_node.attribute("id").as_int();
		name = folder_node.attribute("name").as_string();

		for (xml_node file_node = folder_node.child("file"); file_node; file_node = file_node.next_sibling("file"))
		{
			File file(file_node);
			files.push_back(file);
		}
	}

	File::File( xml_node &file_node )
	{
		id = file_node.attribute("id").as_int();
		name = file_node.attribute("name").as_string();
		width = file_node.attribute("width").as_int();
		height = file_node.attribute("height").as_int();
	}

	Entity::Entity( xml_node &entity_node )
	{
		id = entity_node.attribute("id").as_int();
		name = entity_node.attribute("name").as_string();

		for (xml_node animation_node = entity_node.child("animation");animation_node;animation_node = animation_node.next_sibling("animation"))
		{
			Animation animation(animation_node);
			animations.push_back(animation);

			nameToAnimationMap[animation.name] = animation;
		}
	}

	Animation::Animation( xml_node &animation_node )
	{
		id = animation_node.attribute("id").as_int();
		name = animation_node.attribute("name").as_string();
		length = animation_node.attribute("length").as_int();
		looping = animation_node.attribute("looping").as_bool(true);

		xml_node mainline_node = animation_node.child("mainline");
		mainline = Mainline(mainline_node);

		for (xml_node timeline_node = animation_node.child("timeline");timeline_node;timeline_node=timeline_node.next_sibling("timeline"))
		{
			Timeline timeline(timeline_node);
			timelines.push_back(timeline);
		}
	}

	Mainline::Mainline( xml_node &mainline_node )
	{
		for (xml_node key_node = mainline_node.child("key");key_node;key_node = key_node.next_sibling("key"))
		{
			MainlineKey mainlineKey(key_node);
			mainlineKeys.push_back(mainlineKey);
		}
	}

	MainlineKey::MainlineKey( xml_node &mainlinekey_node )
	{
		id = mainlinekey_node.attribute("id").as_int();
		time = mainlinekey_node.attribute("time").as_int();

		for (xml_node object_ref_node = mainlinekey_node.child("object_ref");object_ref_node;object_ref_node = object_ref_node.next_sibling("object_ref"))
		{
			ObjectRef objectRef(object_ref_node);
			objectRefs.push_back(objectRef);
		}
	}

	ObjectRef::ObjectRef( xml_node &object_ref_node )
	{
		id = object_ref_node.attribute("id").as_int();
		timeline = object_ref_node.attribute("timeline").as_int();
		key = object_ref_node.attribute("key").as_int();
		z_index = object_ref_node.attribute("z_index").as_int();
	}

	Timeline::Timeline( xml_node &timeline_node )
	{
		id = timeline_node.attribute("id").as_int();
		time = timeline_node.attribute("time").as_int(); 		

		for (xml_node timelinekey_node = timeline_node.child("key");timelinekey_node;timelinekey_node=timelinekey_node.next_sibling("key"))
		{
			TimelineKey timelineKey(timelinekey_node);
			timelineKeys.push_back(timelineKey);
		}
	}

	TimelineKey::TimelineKey( xml_node &timelinekey_node )
	{
		id = timelinekey_node.attribute("id").as_int();
		time = timelinekey_node.attribute("time").as_int(); 	
		spin = timelinekey_node.attribute("spin").as_int();

		xml_node object_node = timelinekey_node.child("object");
		object = Object(object_node);
	}

	Object::Object( xml_node &object_node )
	{
		folder = object_node.attribute("folder").as_int();
		file = object_node.attribute("file").as_int(); 	
		x = object_node.attribute("x").as_float();
		y = object_node.attribute("y").as_float(); 	
		pivot_x = object_node.attribute("pivot_x").as_float();
		pivot_y = object_node.attribute("pivot_y").as_float(1.0f);//default value is 1
		angle = object_node.attribute("angle").as_float();
		scale_x = object_node.attribute("scale_x").as_float(1.0f);//default value is 1
		scale_y = object_node.attribute("scale_y").as_float(1.0f);//default value is 1
		a = object_node.attribute("a").as_float(1.0f);//default value is 1
	}

	CocoSpriterAnimationCache *CocoSpriterAnimationCache::instance = NULL;

	CocoSpriterAnimationCache* CocoSpriterAnimationCache::getInstance()
	{
		if (!instance)
			instance = new CocoSpriterAnimationCache();

		return instance;
	}

	CocoSpriterAnimationCache::CocoSpriterAnimationCache()
	{

	}

	CocoSpriterAnimationCache::~CocoSpriterAnimationCache()
	{

	}

	void CocoSpriterAnimationCache::addAnimation(CocoSpriterAnimation *cocospriter_animation)
	{
		CCAssert(cocospriter_animation, "CocoSpriterAnimationCache#addAnimation. cocospriter_animation must not be NULL");

		CocoSpriterAnimation *prevAnimation = nameToAnimationMap[cocospriter_animation->scml_name];
		if (prevAnimation)
			prevAnimation->release();

		cocospriter_animation->retain();
		nameToAnimationMap[cocospriter_animation->scml_name] = cocospriter_animation;
	}

	void CocoSpriterAnimationCache::removeAnimation( const char *scml_name )
	{
		CCAssert(scml_name, "CocoSpriterAnimationCache#removeAnimation. scml_name must not be NULL");

		CocoSpriterAnimation *animation = nameToAnimationMap[scml_name];
		if (animation)
			animation->release();

		nameToAnimationMap.erase(scml_name);		
	}

	CocoSpriterAnimation* CocoSpriterAnimationCache::getAnimation( const char *scml_name )
	{
		CCAssert(scml_name, "CocoSpriterAnimationCache#getAnimation. scml_name must not be NULL");

		return nameToAnimationMap[scml_name];
	}

	CocoSpriterAnimation::CocoSpriterAnimation()
	{

	}

	CocoSpriterAnimation::~CocoSpriterAnimation()
	{

	}

	CocoSpriterAnimation* CocoSpriterAnimation::create( const char *scml_name )
	{
		CocoSpriterAnimation *animation = new CocoSpriterAnimation();
		if (animation && animation->initWithFile(scml_name))
		{
			animation->autorelease();
			return animation;
		}

		CC_SAFE_DELETE(animation);
		return NULL;
	}

	bool CocoSpriterAnimation::initWithFile( const char *scml_name )
	{
		CCAssert(scml_name != NULL, "CocoSpriterAnimation#initWithFile. scml_name must not be NULL");

		this->scml_name = scml_name;
		string filepath = CCFileUtils::sharedFileUtils()->fullPathForFilename(scml_name);		

		xml_document doc;
		xml_parse_result result = doc.load_file(filepath.c_str());

		xml_node spriter_data_node = doc.child("spriter_data");

		scml_version = spriter_data_node.attribute("scml_version").as_string();
		generator = spriter_data_node.attribute("generator").as_string();
		generator_version = spriter_data_node.attribute("generator_version").as_string();

		for (xml_node folder_node = spriter_data_node.child("folder"); folder_node; folder_node = folder_node.next_sibling("folder"))
		{
			Folder folder(folder_node);
			folders.push_back(folder);

			//TODO load plist.There is not releasing code.You must be implement it by yourself with your mind.
			string plistFileName = folder.name+".plist";
			CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile(plistFileName.c_str());
		}

		for (xml_node entity_node = spriter_data_node.child("entity"); entity_node; entity_node = entity_node.next_sibling("entity"))
		{
			Entity entity(entity_node);
			entities.push_back(entity);

			nameToEntityMap[entity.name] = entity;
		}

		return true;
	}

	vector<string> CocoSpriterAnimation::getEntitieNames() 
	{
		vector<string> retval;
		for (unsigned int i = 0 ; i < entities.size() ; i++)
		{
			retval.push_back(entities[i].name);
		}

		return retval;
	}

	vector<string> CocoSpriterAnimation::getAnimationNames( const char *entity_name )
	{
		vector<string> retval;
		Entity &enitity = nameToEntityMap[entity_name];
		for (unsigned int i = 0 ; i < enitity.animations.size() ; i++)
		{
			retval.push_back(enitity.animations[i].name);
		}

		return retval;
	}

	vector<string> CocoSpriterAnimation::getPlistNames()
	{
		vector<string> retval;
		for (unsigned int i = 0 ; i < folders.size() ; i++)
		{
			retval.push_back(folders[i].name + ".plist");
		}
		
		return retval;
	}

	CocoSpriterAnimate::CocoSpriterAnimate()
	{

	}

	CocoSpriterAnimate::~CocoSpriterAnimate()
	{
		CC_SAFE_RELEASE(cocosSpriterAnimation);
	}

	CocoSpriterAnimate* CocoSpriterAnimate::create( CocoSpriterAnimation *cocospriter_animation ,const char *entity_name,const char *animation_name)
	{
		return create(NONE_DATA,cocospriter_animation,entity_name,animation_name);
	}

	CocoSpriterAnimate* CocoSpriterAnimate::create( const char *scml_name,const char *entity_name,const char *animation_name)
	{
		return create(NONE_DATA,scml_name,entity_name,animation_name);
	}

	CocoSpriterAnimate* CocoSpriterAnimate::create( float time,const char *scml_name,const char *entity_name,const char *animation_name )
	{
		CocoSpriterAnimate* animate = new CocoSpriterAnimate();
		if (animate && animate->initWithScml(time,scml_name,entity_name,animation_name))
		{
			animate->autorelease();
			return animate;
		}

		CC_SAFE_DELETE(animate);
		return NULL;
	}

	CocoSpriterAnimate* CocoSpriterAnimate::create( float time,CocoSpriterAnimation *cocospriter_animation,const char *entity_name,const char *animation_name )
	{
		CocoSpriterAnimate* animate = new CocoSpriterAnimate();
		if (animate && animate->initWithAnimation(time,cocospriter_animation,entity_name,animation_name))
		{
			animate->autorelease();
			return animate;
		}

		CC_SAFE_DELETE(animate);
		return NULL;
	}

	bool CocoSpriterAnimate::initWithScml(float time, const char *scml_name,const char *entity_name,const char *animation_name)
	{
		CCAssert(scml_name != NULL, "CocoSpriterAnimate#initWithScml. scml_name must not be NULL");
		CCAssert(entity_name != NULL, "CocoSpriterAnimate#initWithScml. entity_name must not be NULL");
		CCAssert(animation_name != NULL, "CocoSpriterAnimate#initWithScml. animation_name must not be NULL");
				
		CocoSpriterAnimationCache *cocoSpriterAnimationCache = CocoSpriterAnimationCache::getInstance();
		CocoSpriterAnimation *cocospriter_animation = cocoSpriterAnimationCache->getAnimation(scml_name);

		if (!cocospriter_animation)
		{
			cocospriter_animation = CocoSpriterAnimation::create(scml_name);
			cocoSpriterAnimationCache->addAnimation(cocospriter_animation);
		}

		return initWithAnimation(time,cocospriter_animation,entity_name,animation_name);
	}

	bool CocoSpriterAnimate::initWithAnimation(float time,CocoSpriterAnimation *cocospriter_animation,const char *entity_name,const char *animation_name)
	{
		CCAssert(cocospriter_animation != NULL, "CocoSpriterAnimate#initWithAnimation. cocospriter_animation must not be NULL");
		CCAssert(entity_name != NULL, "CocoSpriterAnimate#initWithScml. entity_name must not be NULL");
		CCAssert(animation_name != NULL, "CocoSpriterAnimate#initWithScml. animation_name must not be NULL");

		Animation &animation = cocospriter_animation->nameToEntityMap[entity_name].nameToAnimationMap[animation_name];
		this->animation_time = (time < 0) ? animation.length/1000.f : time;

		if (CCActionInterval::initWithDuration(animation_time))
		{
			CC_SAFE_RETAIN(cocospriter_animation);
			this->cocosSpriterAnimation = cocospriter_animation;
			this->animation = &animation;
			this->key_frame_time = 0;
			this->spf = 1.0f / 60;

			return true;
		}

		return false;
	}
	
	void CocoSpriterAnimate::update( float dt )
	{
		//check target's type
		CocoSpriter *spriter = dynamic_cast<CocoSpriter*>(m_pTarget);
		if (!spriter)
			return;
				
		//optimize!reduce render count
		if (getFPS() != 60)
		{
			float playTime = dt * animation_time;

			if (dt == 1.0){
				key_frame_time = 0;
			}
			else if (playTime >= key_frame_time)
			{
				key_frame_time = (floor(playTime / spf) + 1) * spf;						
			}
			else
			{				
				return;		
			}
		}

		//current time point
		//float time = dt * animation->length ;		
		dt /= animation->length / (animation_time * 1000.f); //dt in key-frame
		float time = (dt == 1 ? dt : dt - floor(dt)) * animation->length;
		
		//current key-frame total count 
		int keyFrameSize = animation->mainline.mainlineKeys.size();

		//find key-frame position
		//0...1...2...3...
		int keyFrameId = 0;		
		for (int i = keyFrameSize - 1; i >= 0 ; i-- )
		{
			int keyFrameTime = animation->mainline.mainlineKeys[i].time;
			
			//Current time must be greater-than-equal match the time of key-frame
			if ( time >= keyFrameTime )
			{
				keyFrameId = i ;
				break;
			}
		}

		MainlineKey &keyFrame = animation->mainline.mainlineKeys[keyFrameId];
		int keyFrameTime = keyFrame.time;

		//next key-frame position
		int nextKeyFrameId = keyFrameId + 1;
		if (nextKeyFrameId >= keyFrameSize)
		{
			if (animation->looping)
				//sweep.Improve the performance of animation that is loop playing 
				nextKeyFrameId = 0;
			else
				return;
		}
		
		MainlineKey &nextKeyFrame = animation->mainline.mainlineKeys[nextKeyFrameId];
		int nextKeyFrameTime = nextKeyFrame.time;
		if (nextKeyFrameId==0)//sweep.Improve the performance of animation that is loop playing 
			nextKeyFrameTime=animation->length;

		//calculus of interpolation param
		float t = (time-keyFrameTime)/(nextKeyFrameTime-keyFrameTime);//TODO In last key-frame,the t is negative ?

		//reset children node	
		int objectRefSize = keyFrame.objectRefs.size();
		vector<CCSprite*> &spritePool = spriter->spritePool;
		unsigned int index = 0;
		for (int i = 0 ; i < objectRefSize ; i++)
		{
			ObjectRef &objectRef = keyFrame.objectRefs[i];
			TimelineKey &keyRef = animation->timelines[objectRef.timeline].timelineKeys[objectRef.key];
			Object &object = keyRef.object;
			
			float x,y,px,py,angle,scale_x,scale_y,a;
			int nextObjectRefId = findObjectRefFromKeyFrame(nextKeyFrame,objectRef.timeline);			
			if (nextObjectRefId == NONE_DATA)//The next key-frame data have not include this slice
			{
				x = object.x ;
				y = object.y ;
				px = object.pivot_x ;
				py = object.pivot_y ;
				angle = object.angle;
				scale_x = object.scale_x;
				scale_y = object.scale_y;
				a = object.a;
			}
			else//The next key-frame data have include this slice
			{
				ObjectRef &nextObjectRef = nextKeyFrame.objectRefs[nextObjectRefId];
				TimelineKey &nextKeyRef = animation->timelines[nextObjectRef.timeline].timelineKeys[nextObjectRef.key];
				Object &nextObject = nextKeyRef.object;

				x = object.x + (nextObject.x - object.x) * t;
				y = object.y + (nextObject.y - object.y) * t;
				px = object.pivot_x + (nextObject.pivot_x - object.pivot_x) * t;
				py = object.pivot_y + (nextObject.pivot_y - object.pivot_y) * t;

				float dangle = nextObject.angle - object.angle;
				if (!(keyRef.spin == -1))
				{
					if (dangle < 0)
						dangle = (nextObject.angle+360)-object.angle;
				}
				else
				{
					if (dangle > 0)
						dangle = (nextObject.angle-360)-object.angle;
				}

				angle = object.angle+(dangle)*t;

				scale_x = object.scale_x + (nextObject.scale_x - object.scale_x) * t;
				scale_y = object.scale_y + (nextObject.scale_y - object.scale_y) * t;
				a = object.a + (nextObject.a - object.a) * t;
			}

			string fileName = cocosSpriterAnimation->folders[object.folder].files[object.file].name;
			CCSpriteFrame *spriteFrame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(fileName.c_str());			
			if (!spriteFrame)
			{
				string fileNameWithoutFolder = fileName.substr(fileName.find("/")+1,fileName.length());
				spriteFrame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(fileNameWithoutFolder.c_str());
			}

			CCSprite *slice = NULL;			
			if (index<spritePool.size())
			{
				slice = spritePool[index];
				slice->setVisible(true);
			}
			else
			{
				slice = CCSprite::create();
				spriter->addChild(slice);
				spritePool.push_back(slice);
			}

			index++;

			slice->setPosition(ccp(x,y));
			slice->setRotation(-angle);			
			slice->setAnchorPoint(ccp(px,py));

			if (!slice->isFrameDisplayed(spriteFrame))
				slice->setDisplayFrame(spriteFrame);
			
			slice->setScaleX(scale_x);
			slice->setScaleY(scale_y);
			slice->setOpacity(floor(a*255));
		}

		//hide other nodes that are not will be used
		for (unsigned int i=index;i<spritePool.size();i++)
		{
			spritePool[i]->setVisible(false);
		}
	
	}

	CCActionInterval* CocoSpriterAnimate::reverse( void )
	{
		CCAssert(false, "CocoSpriterAnimate::reverse is not implements.");
		return NULL;
	}

	int CocoSpriterAnimate::findObjectRefFromKeyFrame( MainlineKey &keyFrame,int timeline )
	{
		//TODO can optimize it?
		for (unsigned int i = 0;i<keyFrame.objectRefs.size();i++)
		{
			if (keyFrame.objectRefs[i].timeline == timeline)
			{
				return i;
			}
		}

		return NONE_DATA;
	}

	void CocoSpriterAnimate::setFPS( int FPS )
	{
		spf = 1.0f/FPS;
	}

	int CocoSpriterAnimate::getFPS()
	{
		return ceil(1.0f / spf);
	}

	CocoSpriter::CocoSpriter()
	{

	}

	CocoSpriter::~CocoSpriter()
	{

	}

	CocoSpriter* CocoSpriter::create()
	{
		CocoSpriter *spriter = new CocoSpriter();
		if (spriter)
		{
			spriter->autorelease();
			return spriter;
		}

		CC_SAFE_DELETE(spriter);
		return NULL;
	}

}