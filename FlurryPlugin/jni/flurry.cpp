#include "gideros.h"
#include "lua.h"
#include "lauxlib.h"
#define LUA_LIB
#include <jni.h>
//this is for debugginh purpose
//and should be commented out before deployment
//you can log using
//__android_log_print(ANDROID_LOG_DEBUG, "tag", "Output String");
//#include <android/log.h>

//Store our main class, what we will use as plugin
static jclass cls;
//store map object for our use
static jclass clsTreeMap;

//start Flurry session
static int startSession(lua_State *L)
{
	JNIEnv *env = g_getJNIEnv();
	
	//get flurry ID as string from lua
	const char *flurryId = lua_tostring(L, 1);

	//call java method and pass flurry ID
	jstring jId = env->NewStringUTF(flurryId);
	env->CallStaticVoidMethod(cls, env->GetStaticMethodID(cls, "startSession", "(Ljava/lang/String;)V"), jId);
	env->DeleteLocalRef(jId);
	
	return 0;
}

//check if plugin is available
static int isAvailable(lua_State *L)
{
	//simply returning true to Lua
	lua_pushboolean(L, 1);
	return 1;
}

//method to log event
static int logEvent(lua_State *L)
{
	JNIEnv *env = g_getJNIEnv();
	
	//get event ID as string from Lua
	const char *eventId = lua_tostring (L, 1);
	jstring jeventId = env->NewStringUTF(eventId);
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
		
		//create Java Tree map object
		jobject jmapobj = env->NewObject(clsTreeMap, env->GetMethodID(clsTreeMap, "<init>", "()V"));
		
		//table is our second argument in function
		int t = 2;
		lua_pushnil(L);  // first key
		while (lua_next(L, t) != 0) {
			// uses 'key' (at index -2) and 'value' (at index -1)
			//putting key and value in the java map object
			jstring jKey = env->NewStringUTF(lua_tostring(L, -2));
			jstring jVal = env->NewStringUTF(lua_tostring(L, -1));
			env->CallObjectMethod(jmapobj, env->GetMethodID(clsTreeMap, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;"), jKey, jVal);
			env->DeleteLocalRef(jKey);
			env->DeleteLocalRef(jVal);
			
			// removes 'value'; keeps 'key' for next iteration
			lua_pop(L, 1);
		}
		//if we had bool argument
		//it means it's timed event
		if(b)
		{
			//call java method for timed event
			env->CallStaticVoidMethod(cls, env->GetStaticMethodID(cls, "logEvent", "(Ljava/lang/String;Ljava/lang/Object;Z)V"), jeventId, jmapobj, b);
		}
		//else it's not a timed event
		else
		{
			//call java method for event with parameters
			env->CallStaticVoidMethod(cls, env->GetStaticMethodID(cls, "logEvent", "(Ljava/lang/String;Ljava/lang/Object;)V"), jeventId, jmapobj);
		}
		env->DeleteLocalRef(jmapobj);
	}
	//we don't have any parameters
	else
	{
		//and call this java method
		env->CallStaticVoidMethod(cls, env->GetStaticMethodID(cls, "logEvent", "(Ljava/lang/String;)V"), jeventId);
	}
	env->DeleteLocalRef(jeventId);
	return 0;
}

//method to end timed events
static int endTimedEvent(lua_State *L)
{
	JNIEnv *env = g_getJNIEnv();
	
	//get event ID from Lua
	const char *eventId = lua_tostring(L, 1);
	jstring jeventId = env->NewStringUTF(eventId);
	
	//call java method to ent timed event
	env->CallStaticVoidMethod(cls, env->GetStaticMethodID(cls, "endTimedEvent", "(Ljava/lang/String;)V"), jeventId);
	env->DeleteLocalRef(jeventId);
	
	return 0;
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
  luaL_register(L, "flurry", funcs);
  return 1;
}

//here we do all our stuff needs to be done on initialization
static void g_initializePlugin(lua_State *L)
{
	//get java environment reference
	JNIEnv *env = g_getJNIEnv();
	
	jclass class_local = env->FindClass("com/giderosmobile/android/plugins/flurry/FlurryPlugin");
	cls = static_cast<jclass>(env->NewGlobalRef(class_local));
	env->DeleteLocalRef(class_local);
	
	jclass class_tree = env->FindClass("java/util/HashMap");
	clsTreeMap = static_cast<jclass>(env->NewGlobalRef(class_tree));
	env->DeleteLocalRef(class_tree);
	
	//get global package object
	lua_getglobal(L, "package");
	lua_getfield(L, -1, "preload");
	
	//put our plugin name inside with a callback to
	//registering C functions
	lua_pushcfunction(L, luaopen_plugin);
	lua_setfield(L, -2, "flurry");

	lua_pop(L, 2);
}

//and here we free everything we need to free
static void g_deinitializePlugin(lua_State *L)
{
	JNIEnv *env = g_getJNIEnv();
	env->DeleteGlobalRef(cls);
	env->DeleteGlobalRef(clsTreeMap);
}

//register our plugin with Gideros lib
REGISTER_PLUGIN("flurry", "1.0")