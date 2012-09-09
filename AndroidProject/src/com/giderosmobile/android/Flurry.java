package com.giderosmobile.android;

import java.util.Map;

import android.app.Activity;

import com.flurry.android.*;
public class Flurry{
	//reference to current activity
	static Activity currentActivity;
	//is flurry session active
	static boolean isActive = false;
	//store flurry ID
	static String ID;
	
	//on create event from Gideros
	//receives reference to current activity
	public static void onCreate(Activity activity)
	{
		currentActivity = activity;
	}

	//on application resume start session
	//if ID was provided
	public static void onResume()
	{
		if(ID != null)
		{
			Flurry.startSession(ID);
		}
	}
	
	//on pause event end flurry session
	public static void onPause()
	{
		Flurry.endSession();
	}

	//start flurry session
	public static void startSession(String flurryID){
		try
		{
			//store flurry ID
			ID = flurryID;
			//check if session is active
			if(!isActive)
			{
				//if not then start flurry session
				FlurryAgent.onStartSession(currentActivity, flurryID);
				isActive = true;
			}
		}
		catch(Exception e){}
	}
	
	//log flurry event
	public static void logEvent(String eventId){
		try
		{
			FlurryAgent.logEvent(eventId);
		}
		catch(Exception e){}
	}
	
	//log flurry event with parameters
	public static void logEvent(String eventId, Object parameters){
		try
		{
			Map<String, String> map = (Map<String, String>)parameters;
			FlurryAgent.logEvent(eventId, map);
		}
		catch(Exception e){}
	}
	
	//log timed event
	public static void logEvent(String eventId, Object parameters, boolean timed){
		try
		{
			Map<String, String> map = (Map<String, String>)parameters;
			FlurryAgent.logEvent(eventId, map, timed);
		}
		catch(Exception e){}
	}
	
	//end timed event
	public static void endTimedEvent(String eventId){
		try
		{
			FlurryAgent.endTimedEvent(eventId);
		}
		catch(Exception e){}
	}
	
	//end flurry session
	public static void endSession(){
		try
		{
			//check if session was active
			if(isActive)
			{
				//end session
				FlurryAgent.onEndSession(currentActivity);
				isActive = false;
			}
		}
		catch(Exception e){}
	}
}
