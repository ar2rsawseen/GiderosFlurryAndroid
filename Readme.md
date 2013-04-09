PLUGIN INSTALLATION

Download Flurry SDK and include it in your project. To do that, right click on project and select properties, from there select "Java Build Path" and click on "Add External JARs..." and navigate to downloaded SDK jar file.
Then go to Order and Export tab and check added Flurry SDK jar file.

Put libflurry.so files to according directories of your exported Gideros project (libs/armeabi and libs/armeabi-v7a)

Put Flurry.java inside "src/com/giderosmobile/android"

Open your main activity in "src/com/giderosmobile/android"

Add "System.loadLibrary("flurry");"

Add Flurry class inside externalClasses using "com.giderosmobile.android.Flurry" string.

And thats it! Now run your application and you can check for "tag:Flurry" inside you LogCat to debug your events from your application.


PLUGIN USAGE
Use it same way as described in Gideros documentation for IOS Flurry plugin