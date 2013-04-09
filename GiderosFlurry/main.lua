--require flurry
require("flurry")


if(flurry.isAvailable()) then
	--start flurry session with your flurry ID
	flurry.startSession("STF9NBS8JQ4539TM8T3T")

	--log simple flurry event
	flurry.logEvent("simpleEvent")

	--log flurry event with parameters
	flurry.logEvent("EventWithParameters", {key="val", anotherkey="anotherval"})

	--log flurry timed event
	flurry.logEvent("TimedEvent", {key="val"}, true)
	
	Timer.delayedCall(1000, function()
		flurry.endTimedEvent("TimedEvent")
	end)
	local text = TextField.new(nil, "Testing Flurry")
	text:setPosition(100, 100);
	stage:addChild(text)
end