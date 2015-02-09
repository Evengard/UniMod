local noxsockets = function(persistent) -- Used to set if it will persist on map change, else the main loop will stop (warning, socket won't be closed!)
	local setTimeoutF = function(what, when)
		local origSetTimeout = setTimeout or cliSetTimeout;
		return origSetTimeout(what, when, nil, persistent);
	end;
	
	local socket = require('socket');
	
	local public = {};
	local private = {};
	
	-- Class declaration
	public.debug = false; -- Set to true to show debug prints
	
	-- Private values
	
	private.receivedCallback = nil;
	private.receiveQuantity = nil;
	private.protocol = 'tcp';
	
	private.readyCallback = nil;
	private.closeCallback = nil;
	
	private.link = nil;
	private.ready = true;
	private.status = "disconnected";
	private.checkingInterval = 10;
	private.requestDisconnect = false;
	
	private.checkingCounter = 0;
	
	private.receivedData = {};
	private.dataToSend = {};
	private.errors = {};
	
	-- Table for functions which will be called only asynchronously
	private.async = {};
	
	-- Table for async tcp processing
	private.async.tcp = {};
	
	-- Table for tcp implementation specifics
	private.tcp = {};
	private.tcp.receivedData = {};
	
	-- Table for udp implementation specifics
	private.udp = {};
	private.udp.receivedData = {};
	
	-- Table for async udp processing
	private.async.udp = {};
	
	-- Private methods
	
	-- Debug method, output only if debug flag set to true
	private.dbg = function(msg)
		if public.debug == true then
			print(msg);
		end;
	end;
	
	private.markAsReady = function()
		private.ready = true;
		if private.readyCallback ~= nil and type(private.readyCallback) == "function" then
			private.readyCallback();
		end;
	end;
	
	private.closingConnection = function()
		private.link:close();
		private.status = "disconnected";
		if private.closeCallback ~= nil and type(private.closeCallback) == "function" then
			private.closeCallback();
		end;
		private.markAsReady();
	end;
	
	private.isIp = function(addr)
		if addr == nil or type(addr) ~= "string" then
			return false;
		end
		
		local chunks = {addr:match("(%d+)%.(%d+)%.(%d+)%.(%d+)")}
		if (#chunks == 4) then
			for _,v in pairs(chunks) do
				if (tonumber(v) < 0 or tonumber(v) > 255) then
					return false;
				end;
			end;
			return true;
		else
			return false;
		end
	end;
	
	private.async.tcp.connect = function(addr, port)
		private.dbg("INFO: connecting...");
		private.link = socket.tcp();
		private.link:settimeout(0); -- Non blocking so NoX doesn't hang when doing something with sockets
		status, err = private.link:connect(addr, port);
		if status == nil and err ~= "timeout" then
			private.dbg("ERROR: Couldn't connect!");
			table.insert(private.errors, {err = "connect error!", socket_err = err, status = private.status});
			private.closingConnection();
			return;
		end;
		private.status = "connecting";
		private.checkingCounter = private.checkingInterval;
		private.markAsReady();
		-- Starting main data loop
		private.dbg("INFO: starting main data loop...");
		setTimeoutF(function() private.async.dataloop() end, 1);
	end;
	
	private.async.udp.connect = function(addr, port)
		private.dbg("INFO: setting up udp...");
		private.link = socket.udp();
		private.link:settimeout(0); -- Non blocking so NoX doesn't hang when doing something with sockets
		private.link:setpeername(addr, port);
		private.status = "connecting";
		private.checkingCounter = private.checkingInterval;
		private.markAsReady();
		-- Starting main data loop
		private.dbg("INFO: starting main data loop...");
		setTimeoutF(function() private.async.dataloop() end, 1);
	end;
	
	-- UDP specific methods
	private.udp.getquantity = function()
		return nil; -- We will be getting the full udp socket and then breaking it into parts in getdata
	end;
	
	private.udp.handledata = function(data, err, partial)
		local state = "error"; -- We assume we're in an error state, but if everything is OK we will be overriding that
		if err == "timeout" then
			state = "timeout";
		elseif err == nil and data ~= nil then
			state = "data";
			table.insert(private.udp.receivedData, data);
			private.dbg("RECV: "..data);
		elseif err == "Message too long" and partial ~= nil then
			state = "partialdata";
			table.insert(private.udp.receivedData, partial);
			private.dbg("RECV PART: "..partial);
		end;
		return state;
	end;
	
	private.udp.getdata = function()
		if #(private.udp.receivedData) > 0 and private.receivedCallback ~= nil and type(private.receivedCallback) == "function" then
			local data = private.udp.receivedData[1];
			if private.receiveQuantity == nil or data:len() <= private.receiveQuantity then
				table.remove(private.udp.receivedData, 1);
				private.receiveQuantity = private.receivedCallback(data);
			else
				local datatoparse = string.sub(data, 1, private.receiveQuantity);
				local datatostore = string.sub(data, private.receiveQuantity + 1);
				private.udp.receivedData[1] = datatostore;
				private.receiveQuantity = private.receivedCallback(datatoparse);
			end;
		end;
	end;
	
	
	-- TCP specific methods
	private.tcp.getquantity = function()
		return private.receiveQuantity;
	end;
	
	private.tcp.handledata = function(data, err, partial)
		local state = "error"; -- We assume we're in an error state, but if everything is OK we will be overriding that
		if err == "timeout" or err == "Socket is not connected" then
			state = "timeout";
		elseif err == nil and data ~= nil then
			state = "data";
			table.insert(private.tcp.receivedData, data);
			private.dbg("RECV: "..data);
		end;
		return state;
	end;
	
	private.tcp.getdata = function()
		if #(private.tcp.receivedData) > 0 and private.receivedCallback ~= nil and type(private.receivedCallback) == "function" then
			local data = private.tcp.receivedData[1];
			table.remove(private.tcp.receivedData, 1);
			private.receiveQuantity = private.receivedCallback(data);
		end;
	end;
	
	private.async.dataloop = function()
		private[private.protocol].getdata();
		if #(private.dataToSend) > 0 then
			private.checkingCounter = private.checkingInterval; -- We are going to receive after we send data because we could have some immediate answer
			private.link:send(private.dataToSend[1]);
			private.dbg("SEND: "..private.dataToSend[1]);
			table.remove(private.dataToSend, 1);
		elseif private.checkingCounter < private.checkingInterval then
			private.checkingCounter = private.checkingCounter + 1;
		else
			local quantity = private[private.protocol].getquantity();
			local data, err, partial = private.link:receive(quantity);
			local state = private[private.protocol].handledata(data, err, partial);
			if state ~= "error" then
				private.status = "connected";
			else
				private.closingConnection();
				private.dbg("ERROR: socket error! Closing connection");
				table.insert(private.errors, {err = "socket error!", socket_err = err, status = private.status});
			end;
			if state == "timeout" then
				private.checkingCounter = 0; -- Resetting the counter only if we got a timeout which means we didn't received data on this tick
			end;
		end;
		if private.requestDisconnect == true then
			private.dbg("INFO: closing connection...");
			private.closingConnection();
			private.requestDisconnect = false;
		elseif private.status ~= "disconnected" then
			setTimeoutF(function() private.async.dataloop() end, 1);
		else
			private.dbg("WARN: main dataloop terminated!");
		end;
	end;
	
	-- Public methods
	public.initialize = function(receiveCallback, receiveCheckingInterval, initialReceiveQuantity)
		if receiveCallback ~= nil and type(receiveCallback) == "function" then
			private.receivedCallback = receiveCallback;
		else
			private.receivedCallback = nil;
		end;
		private.readyCallback = nil;
		private.closeCallback = nil;
		private.checkingInterval = receiveCheckingInterval or 10;
		private.receiveQuantity = initialReceiveQuantity;
		return true;
	end;
	
	public.connect = function(addr, port, protocol)
		if private.ready == true then
			private.ready = false;
			
			private.dbg("INFO: preparing connect...");
			-- Reinitialize
			private.status = "disconnected";
			private.checkingCounter = 0;
			if private.link ~= nil then
				private.link:close();
				private.link = nil;
			end;
			private.receivedData = {};
			private.errors = {};
			
			protocol = protocol or "tcp";
			local addrorig = addr;
			if private.isIp(addr) == false then
				addr = socket.dns.toip(addr); -- Seems like LuaSockets is a little bugged, it doesn't convert hostnames to IPs on connect automatically.
				if addr == nil then
					addr = addrorig;
				end;
			end;
			if protocol == "tcp" then
				setTimeoutF(function() private.async.tcp.connect(addr, port) end, 1);
				private.protocol = "tcp";
			elseif protocol == "udp" then
				setTimeoutF(function() private.async.udp.connect(addr, port) end, 1);
				private.protocol = "udp";
			else
				error("ERROR: not implemented");
			end;
			return true;
		else
			return false;
		end;
	end;
	
	public.send = function(data)
		if private.ready == true then
			if private.status == "disconnected" then
				error("ERROR: the socket isn't connected, can't send data!");
			end;
			table.insert(private.dataToSend, data);
			return true;
		else
			return false;
		end;
	end;
	
	public.close = function()
		if private.ready == true and private.status ~= "disconnected" then
			private.ready = false;
			private.requestDisconnect = true;
			return true;
		else
			return false;
		end;
	end;
	
	public.setReceiveCallback = function(func)
		if func == nil or type(func) == "function" then
			private.receivedCallback = func;
		end;
	end;
	
	public.setReadyCallback = function(func)
		if func == nil or type(func) == "function" then
			private.readyCallback = func;
			if private.ready == true then -- If we are ready we should execute the callback immediately!
				private.readyCallback();
			end;
		end;
	end;
	
	public.setCloseCallback = function(func)
		if func == nil or type(func) == "function" then
			private.closeCallback = func;
		end;
	end;
	
	public.getStatus = function()
		return private.status;
	end;
	
	public.getLastError = function()
		return private.errors[#(private.errors)];
	end;
	
	public.getAllErrors = function()
		return private.errors;
	end;
	
	public.resetAllErrors = function()
		private.errors = {};
	end;
	-- /Class declaration
	
	return public;
end;

return noxsockets;
