{
	luaL_dostring(L," \
local y=loadfile \
local z=function(x) \
	local a,b=bz2Loader(x) \
	if not a then a,b=y(x) end \
	return a,b \
end \
loadfile=z \
dofile = function(x) \
	local env=getfenv(2) \
	if env and env.setTimeout then \
		env.setTimeout(function() print('dofile deprecated, arg='..x) end,1) \
	end \
	local a,b=z(x) \
	if not a then error(b) end \
	setfenv(a,env)() \
end \
");
}