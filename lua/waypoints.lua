--¬ообще можешь включать а можешь нет - все равно уже. (ѕока до конца не доделанно, нет соединений) а так с схранением мапы пойдет)


function getWp(a)
	local x,y,id2,flag,name=getWaypoint(a,true)
	print('X: ',x,' Y: ',y,' Id: ',id2,' Id2: ',flag,' name: ',name)
end

local doShowWp
function showWp()
	local t={}
	local x,y
	local fn
	if doShowWp then 
		doShowWp=false
		return 
	end
	doShowWp=true
	fn=function()
		if not doShowWp then return end
	t=waypointList()
	for i=1,#t,1 do
		x,y=getWaypoint(t[i],true)
		netPointFx(0x82,x,y)
	end

	setTimeout(fn,15)

	end
	fn()
end

function crWp(a,b)
	if me==nil then setMe() end
	local x,y = playerMouse(me)
	local obj = createWaypoint(x,y,a)
	return obj
end
