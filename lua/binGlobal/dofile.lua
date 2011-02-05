local y=loadfile
local bz2LoaderL=bz2Loader
local z=function(x)
	local a,b=bz2LoaderL(x)
	if not a then a,b=y(x) end
--[[
	local s=''
	if not a then s='bz2Loader fail'
		 a,b=y(x)
	 else
	    s='bz2Loader ok'
	 end
	setTimeout(function() print(s) end,1)
--]]
	return a,b
end
loadfile=z
dofile = function(x)
	local env=getfenv(2)
	if env and env.setTimeout then
		env.setTimeout(function() print('dofile deprecated, arg='..x) end,1)
	end
	local a,b=z(x)
	if not a then error(b) end
	setfenv(a,env)()
end
