--require flurry
require("flurry")

--start flurry session with your flurry ID
flurry.startSession("your flurry ID")

--log simple flurry event
flurry.logEvent("simpleEvent")

--log flurry event with parameters
flurry.logEvent("EventWithParameters", {key="val", anotherkey="anotherval"})

--log flurry timed event
flurry.logEvent("TimedEvent", {key="val"}, true)
