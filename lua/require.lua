(function()
	local _ENV = getfenv();
	_ENV.require = function(modulename)
		local _G = getfenv(0);
		local sandbox_env = {};
		setmetatable(sandbox_env, {__index = _ENV});
		local _G_metatable = {};
		if _G ~= _ENV then
			setfenv(0, sandbox_env);
		end;
		local moduleloaded = _G.require(modulename);
		if _G ~= _ENV then
			setfenv(0, _G);
		end;
		return moduleloaded;
	end;
end)()