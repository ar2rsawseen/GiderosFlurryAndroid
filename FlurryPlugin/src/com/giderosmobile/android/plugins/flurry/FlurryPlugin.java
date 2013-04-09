package com.giderosmobile.android.plugins.flurry;

import java.lang.ref.WeakReference;
import java.util.Map;

import android.app.Activity;

import com.flurry.android.*;

public class FlurryPlugin {
	//reference to main activity
	private static WeakReference<Activity> sActivity;	
	//is flurry session active
	private static boolean isActive = false;
	//store flurry ID
	private static String ID;
	
	//on create event from Gideros
	//receives reference to current activity
	public static void onCreate(Activity activity)
	{
		//reference to activity
		sActivity =  new WeakReference<Activity>(activity);	
		FlurryAgent.setLogEnabled(false);
	}
	
	//on stop event
	public static void onStop()
	{
		FlurryPlugin.endSession();
	}

	//start flurry session
	public static void startSession(String flurryID){
		//store flurry ID
		ID = flurryID;
		//check if session is active
		if(!isActive)
		{
			//if not then start flurry session
			FlurryAgent.onStartSession(sActivity.get(), flurryID);
			isActive = true;
		}
	}
	
	//log flurry event
	public static void logEvent(String eventId){
		FlurryAgent.logEvent(eventId);
	}
	
	//log flurry event with parameters
	public static void logEvent(String eventId, Object parameters){
		Map<String, String> map = (Map<String, String>)parameters;
		FlurryAgent.logEvent(eventId, map);
	}
	
	//log timed event
	public static void logEvent(String eventId, Object parameters, boolean timed){
		Map<String, String> map = (Map<String, String>)parameters;
		FlurryAgent.logEvent(eventId, map, timed);
	}
	
	//end timed event
	public static void endTimedEvent(String eventId){
		FlurryAgent.endTimedEvent(eventId);
	}
	
	//end flurry session
	public static void endSession(){
		//check if session was active
		if(isActive)
		{
			//end session
			FlurryAgent.onEndSession(sActivity.get());
			isActive = false;
		}
	}
}
