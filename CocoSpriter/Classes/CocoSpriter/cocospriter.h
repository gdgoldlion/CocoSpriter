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

#ifndef _COCOSPRITER_H_
#define _COCOSPRITER_H_

#include <vector>
#include <map>
#include <string>
#include "pugixml.hpp"
#include "cocos2d.h"

namespace cocospriter
{
	using namespace std;
	using namespace cocos2d;
	using namespace pugi;

	struct File
	{
		File(){}
		File(xml_node &file_node);

		int id;
		string name;
		int width;
		int height;
	};

	struct Folder
	{
		Folder(){}
		Folder(xml_node &folder_node);

		int id;
		string name;

		vector<File> files;
	};

	struct Object
	{
		Object(){};
		Object(xml_node &object_node);

		int folder;
		int file;
		float x;
		float y;
		float pivot_x;
		float pivot_y;
		float angle;
		float scale_x;
		float scale_y;
		float a;//opacity.But it's name is a(alpha?) in scml file.XD
	};

	struct TimelineKey
	{
		TimelineKey(){}
		TimelineKey(xml_node &timelinekey_node);

		int id;
		int time;
		int spin;

		Object object;
	};

	struct Timeline
	{
		Timeline(){}
		Timeline(xml_node &timeline_node);

		int id;
		int time;
		vector<TimelineKey> timelineKeys;
	};

	struct ObjectRef
	{
		ObjectRef();
		ObjectRef(xml_node &object_ref_node);

		int id;
		int timeline;
		int key;
		int z_index;
	};

	struct MainlineKey
	{
		MainlineKey(){}
		MainlineKey(xml_node &mainlinekey_node);

		int id;
		int time;
		vector<ObjectRef> objectRefs;
	};

	struct Mainline
	{
		Mainline(){}
		Mainline(xml_node &mainline_node);

		vector<MainlineKey> mainlineKeys;
	};

	struct Animation
	{
		Animation(){}
		Animation(xml_node &animation_node);
		int id;
		string name;
		int length;
		bool looping;

		Mainline mainline;
		vector<Timeline> timelines;
	};

	struct Entity
	{
		Entity(){}
		Entity(xml_node &entity_node);
		int id;
		string name;

		vector<Animation> animations;

		map<string,Animation> nameToAnimationMap;
	};	

	class CocoSpriterAnimation : public CCObject
	{
		friend class CocoSpriterAnimate;
		friend class CocoSpriterAnimationCache;
	public:
		CocoSpriterAnimation();
		~CocoSpriterAnimation();

		static CocoSpriterAnimation* create(const char *scml_name);	
	public:
		string get_scml_name() { return scml_name; }
		string get_scml_version() { return scml_version; }
		string get_generator() { return generator; }
		string get_generator_version() { return generator_version; }

		vector<string> getPlistNames();
		vector<string> getEntitieNames() ;
		vector<string> getAnimationNames(const char *entity_name) ;
	protected:
		bool initWithFile(const char *scml_name);

		string scml_name;
		string scml_version;
		string generator;
		string generator_version;

		vector<Entity> entities;
		vector<Folder> folders;

		map<string,Entity> nameToEntityMap;
	};

	class CocoSpriterAnimationCache : public CCObject
	{
	public:
		CocoSpriterAnimationCache();
		~CocoSpriterAnimationCache();
		static CocoSpriterAnimationCache* getInstance();
	public:
		//If the animation is existed it will be covered.
		void addAnimation(CocoSpriterAnimation *cocospriter_animation);
		//Remove CocoSpriterAnimation* from Cache and release it.Note:it do not contains releasing of texture.
		void removeAnimation(const char *scml_name);

		//If the name is not found it will return NULL.
		CocoSpriterAnimation* getAnimation(const char *scml_name);

	private:
		map<string,CocoSpriterAnimation*> nameToAnimationMap;
		static CocoSpriterAnimationCache *instance;
	};

	class CocoSpriterAnimate : public CCActionInterval
	{
	public:
		CocoSpriterAnimate();
		virtual ~CocoSpriterAnimate();	

		static CocoSpriterAnimate* create(float time,const char *scml_name,const char *entity_name,const char *animation_name);
		static CocoSpriterAnimate* create(const char *scml_name,const char *entity_name,const char *animation_name);

		static CocoSpriterAnimate* create(float time,CocoSpriterAnimation *cocospriter_animation,const char *entity_name,const char *animation_name);
		static CocoSpriterAnimate* create(CocoSpriterAnimation *cocospriter_animation,const char *entity_name,const char *animation_name);
	public:
		virtual void update(float dt);
		//No implements.
		virtual CCActionInterval* reverse(void);

		void setFPS(int FPS);//it's optimize by 24 FPS etc.
		int getFPS();
	protected:
		bool initWithScml(float time,const char *scml_name,const char *entity_name,const char *animation_name);		
		bool initWithAnimation(float time,CocoSpriterAnimation *cocospriter_animation,const char *entity_name,const char *animation_name);

		//Find the objectRef in keyFrame by timeline.If haven't,it will return -1.
		int findObjectRefFromKeyFrame(MainlineKey &keyFrame,int timeline);

		CocoSpriterAnimation *cocosSpriterAnimation;

		Animation *animation;
		float animation_time;
		
		float spf; //seconds per frame
		float key_frame_time;
	}; 

	class CocoSpriter : public CCNode
	{
		friend class CocoSpriterAnimate;
	public:
		CocoSpriter();
		virtual ~CocoSpriter();

		static CocoSpriter* create();		
	protected:
		vector<CCSprite*> spritePool;
	};
}

#endif //_COCOSPRITER_H_