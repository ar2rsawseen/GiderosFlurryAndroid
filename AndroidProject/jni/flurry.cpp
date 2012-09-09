#include "gideros.h"
#include "lua.h"
#include "lauxlib.h"
#define LUA_LIB
#include <jni.h>
//this is for debugginh purpose
//and should be commented out before deployment
//you can log using
//__android_log_print(ANDROID_LOG_DEBUG, "tag", "Output String");
#include <android/log.h>

//some configurations of our plugin
static const char* pluginName = "flurry";
static const char* pluginVersion = "1.0";
static const char* javaClassName = "com/giderosmobile/android/Flurry";

//Store Java Environment reference
static JNIEnv *ENV;
//Store our main class, what we will use as plugin
static jclass cls;
//store map object for our use
static jclass clsTreeMap;
//store startSession method ID
static jmethodID jStartSession;
//store logEvent method ID
static jmethodID jLogEvent;
//store logEvent with parameter method ID
static jmethodID jLogEventParam;
//store logEvent timed method ID
static jmethodID jLogEventTimed;
//store endTimedEvent method ID
static jmethodID jEndEvent;
//store method to create a map
static jmethodID createMap;
//store method to put value in map
static jmethodID putMap;

//start Flurry session
static int startSession(lua_State *L)
{
	//if no Java Env, exit
	if(ENV == NULL) return 0;
	
	//if no class, try to retrieve it
	if(cls == NULL)
	{
		cls = ENV->FindClass(javaClassName);
		if(!cls) return 0;
	}
	
	//if no methode, try to retrieve it
	if(jStartSession == NULL)
	{
		jStartSession = ENV->GetStaticMethodID(cls, "startSession", "(Ljava/lang/String;)V");
		if(!jStartSession) return 0;
	}
	
	//get flurry ID as string from lua
	const char *flurryId = lua_tostring(L, 1);

	//call java method and pass flurry ID
	ENV->CallStaticVoidMethod(cls, jStartSession, ENV->NewStringUTF(flurryId));
	
	return 1;
}

//check if plugin is available
static int isAvailable(lua_State *L)
{
	//simply returning true to Lua
	lua_pushboolean(L, true);
	return 1;
}

//method to log event
static int logEvent(lua_State *L)
{
	//if no Java Env, exit
	if(ENV == NULL) return 0;
	
	//if no class, try to retrieve it
	if(cls == NULL)
	{
		cls = ENV->FindClass(javaClassName);
		if(!cls) return 0;
	}
	
	//get event ID as string from Lua
	const char *eventId = lua_tostring (L, 1);
	
	//check if second argument is table
	if(lua_istable(L, 2))
	{
		//if is it, let's check for whird argument bool
		//and store it in variable b
		bool b = false;
		if(lua_isboolean(L, 3))
		{
			b = lua_toboolean(L, 3);
		}
		
		//get Map class to store parameters
		if(clsTreeMap == NULL)
		{
			const char* treemap_class_name = "java/util/TreeMap";
			clsTreeMap = ENV->FindClass(treemap_class_name);
			if(!clsTreeMap) return 0;
		}
		
		//get create map method
		if(createMap == NULL)
		{
			createMap = ENV->GetMethodID(clsTreeMap, "<init>", "()V");
		}
		
		//create Java Tree map object
		jobject jmapobj = ENV->NewObject(clsTreeMap, createMap);
		
		//get method to put values in map
		if(putMap == NULL)
		{
			putMap = ENV->GetMethodID(clsTreeMap, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
		}
		
		//table is our second argument in function
		int t = 2;
		lua_pushnil(L);  // first key
		while (lua_next(L, t) != 0) {
			// uses 'key' (at index -2) and 'value' (at index -1)
			//putting key and value in the java map object
			ENV->CallObjectMethod(jmapobj, putMap, ENV->NewStringUTF(lua_tostring(L, -2)), ENV->NewStringUTF(lua_tostring(L, -1)));
			// removes 'value'; keeps 'key' for next iteration
			lua_pop(L, 1);
		}
		//if we had bool argument
		//it means it's timed event
		if(b)
		{
			//if we don't have method for timed events
			//retrieve it
			if(jLogEventTimed == NULL)
			{
				jLogEventTimed = ENV->GetStaticMethodID(cls, "logEvent", "(Ljava/lang/String;Ljava/lang/Object;Z)V");
				if(!jLogEventTimed) return 0;
			}
			//call java method for timed event
			ENV->CallStaticVoidMethod(cls, jLogEventTimed, ENV->NewStringUTF(eventId), jmapobj, b);
		}
		//else it's not a timed event
		else
		{
			//try to retrieve event with parameters
			//if one does not exists yet
			if(jLogEventParam == NULL)
			{
				jLogEventParam = ENV->GetStaticMethodID(cls, "logEvent", "(Ljava/lang/String;Ljava/lang/Object;)V");
				if(!jLogEventParam) return 0;
			}
			//call java method for event with parameters
			ENV->CallStaticVoidMethod(cls, jLogEventParam, ENV->NewStringUTF(eventId), jmapobj);
		}
	}
	//we don't have any parameters
	else
	{
		//simple get event method if we don't have it yet
		if(jLogEvent == NULL)
		{
			jLogEvent = ENV->GetStaticMethodID(cls, "logEvent", "(Ljava/lang/String;)V");
			if(!jLogEvent) return 0;
		}
		//and call this java method
		ENV->CallStaticVoidMethod(cls, jLogEvent, ENV->NewStringUTF(eventId));
	}
	return 1;
}

//method to end timed events
static int endTimedEvent(lua_State *L)
{
	//if no Java Env, exit
	if(ENV == NULL) return 0;
	
	//if no class, try to retrieve it
	if(cls == NULL)
	{
		cls = ENV->FindClass(javaClassName);
		if(!cls) return 0;
	}
	
	//check method for ending timed events
	//and retrieve it if we don't have it
	if(jEndEvent == NULL)
	{
		jEndEvent = ENV->GetStaticMethodID(cls, "endTimedEvent", "(Ljava/lang/String;)V");
		if(!jEndEvent) return 0;
	}
	
	//get event ID from Lua
	const char *eventId = lua_tostring(L, 1);
	
	//call java method to ent timed event
	ENV->CallStaticVoidMethod(cls, jEndEvent, ENV->NewStringUTF(eventId));
	
	return 1;
}

//here we register all functions we could call from lua
//lua function name as key and C function as value
static const struct luaL_Reg funcs[] = {
  { "startSession",	startSession },
  { "logEvent",	logEvent },
  { "endTimedEvent",	endTimedEvent },
  { "isAvailable",	isAvailable },
  { NULL, NULL }//don't forget nulls at the end
};

//here we register all the C functions for lua
//so lua engine would know they exists
LUALIB_API int luaopen_plugin(lua_State *L)
{
  luaL_register(L, pluginName, funcs);
  return 1;
}

//here we do all our stuff needs to be done on initialization
static void g_initializePlugin(lua_State *L)
{
	//get java environment reference
	ENV = g_getJNIEnv();
	
	//get global package object
	lua_getglobal(L, "package");
	lua_getfield(L, -1, "preload");
	
	//put our plugin name inside with a callback to
	//registering C functions
	lua_pushcfunction(L, luaopen_plugin);
	lua_setfield(L, -2, pluginName);

	lua_pop(L, 2);
}

//and here we free everything we need to free
static void g_deinitializePlugin(lua_State *L)
{

}

//register our plugin with Gideros lib
REGISTER_PLUGIN(pluginName, pluginVersion)