local xwis = function(persistent) -- Used to set if it will persist on map change
	local setTimeoutF = function(what, when)
		local origSetTimeout = setTimeout or cliSetTimeout;
		return origSetTimeout(what, when, nil, persistent);
	end;
	
	local noxsockets = require('noxsockets');
	
	local private = {};
	
	local xwis = {};
	
	private.link = noxsockets(persistent);
	
	private.status = "disconnected";
	private.login = nil;
	private.apgar = nil;
	
	private.callback = nil;
	
	private.readycallback = nil;
	
	private.logintimedout = function()
		if private.status == "notloggedin" then
			print("ERROR: couldn't authenticate in due time! Timeout.");
			xwis.close();
		end;
	end;
	
	private.loginprocedure = function()
		xwis.talk("CVERS 11015 9472");
		xwis.talk("PASS supersecret");
		xwis.talk("NICK "..private.login);
		xwis.talk("apgar "..private.apgar.." 0");
		xwis.talk("USER UserName HostName irc.westwood.com :RealName");
		setTimeoutF(private.logintimedout, 900);
	end;
	
	private.dataReceiver = function(data)
		if private.status == "notloggedin" and string.starts(data, ": 375 u :- Welcome to XWIS!") == true then
			private.status = "loggedin";
			print("INFO: logged into the xwis server!");
			if private.readycallback ~= nil and type(private.readycallback) == "function" then
				private.readycallback();
			end;
		elseif string.starts(data, "PING") == true then
			private.link.send("PONG\r\n");
		elseif private.callback ~= nil and type(private.callback) == "function" then
			private.callback(data);
		end;
	end;
	
	private.onReady = function()
		if private.link.getStatus() ~= "disconnected" and private.status == "disconnected" then
			private.status = "notloggedin";
			private.loginprocedure();
		elseif private.link.getStatus() == "disconnected" and private.status == "disconnected" then
			print("INFO: ready to reconnect!");
		end;
	end;
	
	private.onClose = function()
		private.status = "disconnected";
		print("ERROR: lost connection to XWIS");
	end;
	
	private.link.setReceiveCallback(private.dataReceiver);
	private.link.setCloseCallback(private.onClose);
	private.link.setReadyCallback(private.onReady);
	
	private.link.debug = true;
	
	xwis.connect = function(login, apgar)
		print('XWIS preparing connection');
		private.login = login;
		private.apgar = apgar;
		private.status = "disconnected";
		private.link.connect("xwis.net", 4000);
	end;
	
	xwis.setReadyCallback = function(func)
		if func ~= nil and type(func) == "function" then
			private.readycallback = func;
		end;
	end;
	
	xwis.getStatus = function()
		return private.status;
	end;
	
	xwis.talk = function(data, func)
		if func == nil or type(func) == "function" then
			private.callback = func;
		end;
		private.link.send(data.."\r\n");
	end;
	
	xwis.close = function()
		private.link.close();
	end;
	
	xwis.decrypt = function(data)
		local length = data:len();
		local bittester = 0;
		local bitcount = 0;
		local bitloc = 0;
		
		for i = 1, length - 9, 1 do
			local accumulator = 0;
			for j = 0, 7, 1 do
				if bitcount == 7 then
					bitcount = 0;
					bitloc = bitloc + 1;
				end;
				if bitloc == 8 then
					data = data:bin_set(bittester+1, 0);
					bittester = bittester + 1;
					bitloc = 0;
				end;
				local val6;
				if bittester >= length then
					val6 = 0;
				else
					val6 = data:bin_get(bittester+1);
				end;
				local val5 = bit.lshift(1, bitloc);
				local val4 = bit.band(val6, val5);
				local val3 = bit.rshift(val4, bitloc);
				local val2 = bit.lshift(val3, j);
				local val1 = bit.band(val2, 0xFF);
				accumulator = bit.bxor(accumulator, val1);
				
				bitloc = bitloc + 1;
				bitcount = bitcount + 1;
			end;
			data = data:bin_set(i, bit.band(accumulator, 0xFF));
		end;
		return data;
	end;
	
	xwis.encrypt = function(data)
		local length = data:len();
		local bittester = 0;
		local bitcount = 0;
		local bitloc = 0;
		local bitloc2 = 0;
		local accumulator = 0;
		local output = data;
		
		for i=1, length - 9, 1 do
			for j=0, 7, 1 do
				if bitloc == 8 then
					bittester = bittester + 1;
					bitloc = 0;
				end;
				
				if bitloc2 == 7 then
					local temp = bit.lshift(1, 7);
					accumulator = bit.bxor(accumulator, temp);
					output = output:bin_set(bitcount+1, accumulator);
					bitcount = bitcount + 1;
					accumulator = 0;
					bitloc2 = 0;
				end;
				
				local val6;
				if bittester >= length then
					val6 = 0;
				else
					val6 = data:bin_get(bittester+1);
				end;
				local val5 = bit.lshift(1, bitloc);
				local val4 = bit.band(val6, val5);
				local val3 = bit.rshift(val4, bitloc);
				local val2 = bit.lshift(val3, bitloc2);
				local val1 = bit.band(val2, 0xFF);
				accumulator = bit.bxor(accumulator, val1);
				
				bitloc = bitloc + 1;
				bitloc2 = bitloc2 + 1;
			end;
		end;
		return output;
	end;
	
	xwis.getLastError = function()
		return private.link.getLastError();
	end;
	
	xwis.parse = function(binary)
		local parsed = {};
		
		-- PARSING 1 byte accesscode
		parsed.accesscode = binary:bin_get(1);
		local serveraccess = bit.rshift(bit.band(parsed.accesscode, 0xF0), 4);
		if serveraccess == 0 then
			parsed.serveraccess = "open";
		elseif serveraccess == 1 then
			parsed.serveraccess = "closed";
		elseif serveraccess == 2 then
			parsed.serveraccess = "private";
		else
			parsed.serveraccess = "unknown";
		end;
		local classaccess = bit.band(parsed.accesscode, 0x0F);
		parsed.disallowedclasses = {};
		if bit.band(classaccess, 1) ~= 0 then
			table.insert(parsed.disallowedclasses, "WAR");
		end;
		if bit.band(classaccess, 2) ~= 0 then
			table.insert(parsed.disallowedclasses, "WIZ");
		end;
		if bit.band(classaccess, 4) ~= 0 then
			table.insert(parsed.disallowedclasses, "CON");
		end;
		
		-- PARSING 1 byte unk1
		parsed.unk1 = binary:bin_get(2);
		
		-- PARSING 1 byte resolutioncode
		parsed.resolutioncode = binary:bin_get(3);
		if bit.band(parsed.resolutioncode, bit.lshift(8, 4)) > 0 then
			parsed.limitresolution = true;
		else
			parsed.limitresolution = false;
		end;
		local resolution = bit.band(parsed.resolutioncode, 0xF);
		if resolution == 0 then
			parsed.resolution = "640x480";
		elseif resolution == 1 then
			parsed.resolution = "800x600";
		elseif resolution == 2 then
			parsed.resolution = "1024x768";
		elseif resolution == 3 then
			parsed.resolution = "1280x1024";
		else
			parsed.resolution = "unknown";
		end;
		
		-- PARSING 1 byte players
		parsed.players = binary:bin_get(4);
		
		-- PARSING 1 byte maxplayers
		parsed.maxplayers = binary:bin_get(5);
		
		-- PARSING 2 bytes minping
		parsed.minping = bin_ops.to_number(binary, 2, 6);
		
		-- PARSING 2 bytes maxping
		parsed.maxping = bin_ops.to_number(binary, 2, 8);
		
		-- PARSING 2 bytes unk2
		parsed.unk2 = bin_ops.to_number(binary, 2, 10);
		
		-- PARSING 9 bytes mapname
		parsed.mapname = binary:sub(12, 20);
		
		-- PARSING 15 bytes gamename
		parsed.gamename = binary:sub(21, 35);
		
		-- PARSING 28 bytes unk3
		parsed.unk3 = binary:sub(36, 63);
		
		-- PARSING 2 bytes gameflags
		parsed.gameflags = bin_ops.to_number(binary, 2, 64);
		local maptype = bit.band(parsed.gameflags, 0x1FF0);
		if bit.band(maptype, 0x0010) > 0 then
			parsed.maptype = "kotr";
		elseif bit.band(maptype, 0x0020) > 0 then
			parsed.maptype = "ctf";
		elseif bit.band(maptype, 0x0040) > 0 then
			parsed.maptype = "flagball";
		elseif bit.band(maptype, 0x0080) > 0 then
			parsed.maptype = "chat";
		elseif bit.band(maptype, 0x0100) > 0 then
			parsed.maptype = "arena";
		elseif bit.band(maptype, 0x0400) > 0 then
			parsed.maptype = "elimination";
		elseif bit.band(maptype, 0x0A00) > 0 then
			parsed.maptype = "coop";
		elseif bit.band(maptype, 0x1000) > 0 then
			parsed.maptype = "quest";
		else
			parsed.maptype = "unknown";
		end;
		
		-- PARSING 2 bytes fraglimit
		parsed.fraglimit = bin_ops.to_number(binary, 2, 66);
		
		-- PARSING 2 bytes timelimit
		parsed.timelimit = bin_ops.to_number(binary, 2, 68);
		
		-- PARSING everything else
		parsed.ending = binary:sub(70);
		
		return parsed;
	end;
	
	xwis.encode = function(parsed)
		local binary = "";
		
		-- ENCODING 1 byte accesscode
		local serveraccess = 0;
		if parsed.serveraccess == "open" then
			serveraccess = 0;
		elseif parsed.serveraccess == "closed" then
			serveraccess = 1;
		elseif parsed.serveraccess == "private" then
			serveraccess = 2;
		end;
		parsed.accesscode = bit.lshift(serveraccess, 4);
		local classaccess = 0;
		for key,class in pairs(parsed.disallowedclasses) do
			if class == "WAR" then
				classaccess = bit.bor(classaccess, 1);
			elseif class == "WIZ" then
				classaccess = bit.bor(classaccess, 2);
			elseif class == "CON" then
				classaccess = bit.bor(classaccess, 4);
			end;
		end;
		parsed.accesscode = bit.bor(parsed.accesscode, classaccess);
		binary = binary..string.char(parsed.accesscode);
		
		-- ENCODING 1 byte unk1
		parsed.unk1 = 255; -- Value as seen on XWIS
		binary = binary..string.char(parsed.unk1);
		
		-- ENCODING 1 byte resolutioncode
		parsed.resolutioncode = 0;
		if parsed.limitresolution == true then
			parsed.resolutioncode = bit.lshift(8, 4);
		end;
		local resolution = 0;
		if parsed.resolution == "640x480" then
			resolution = 0;
		elseif parsed.resolution == "800x600" then
			resolution = 1;
		elseif parsed.resolution == "1024x768" then
			resolution = 2;
		elseif parsed.resolution == "1280x1024" then
			resolution = 3;
		end;
		parsed.resolutioncode = bit.bor(parsed.resolutioncode, resolution);
		binary = binary..string.char(parsed.resolutioncode);
		
		-- ENCODING 1 byte players
		binary = binary..string.char(parsed.players);
		
		-- ENCODING 1 byte maxplayers
		binary = binary..string.char(parsed.maxplayers);
		
		-- ENCODING 2 bytes minping
		binary = binary..bin_ops.to_string(parsed.minping, 2);
		
		-- ENCODING 2 bytes maxping
		binary = binary..bin_ops.to_string(parsed.maxping, 2);
		
		-- ENCODING 2 bytes unk2
		parsed.unk2 = 18590 -- Value as seen on XWIS
		binary = binary..bin_ops.to_string(parsed.unk2, 2);
		
		-- ENCODING 9 bytes mapname
		local mapname = parsed.mapname:sub(1, 9);
		if mapname:len() < 9 then
			for i = mapname:len()+1, 9, 1 do
				mapname = mapname..string.char(0x00);
			end;
		end;
		binary = binary..mapname;
		
		-- ENCODING 15 bytes gamename
		local gamename = parsed.gamename:sub(1, 15);
		if gamename:len() < 15 then
			for i = gamename:len()+1, 15, 1 do
				gamename = gamename..string.char(0x00);
			end;
		end;
		binary = binary..gamename;
		
		-- ENCODING 28 bytes unk3
		parsed.unk3 = ""; 
		for unk3byte = 1, 28, 1 do
			parsed.unk3 = parsed.unk3..string.char(255); -- Value as seen on XWIS
		end;
		binary = binary..parsed.unk3;
		
		-- ENCODING 2 bytes gameflags
		binary = binary..bin_ops.to_string(parsed.gameflags, 2); -- Because we have "gameFlags()" function, no need to craft by hand
		
		-- ENCODING 2 bytes fraglimit
		binary = binary..bin_ops.to_string(parsed.fraglimit, 2);
		
		-- ENCODING 2 bytes timelimit
		binary = binary..bin_ops.to_string(parsed.timelimit, 2);
		
		-- ENCODING 9 bytes ending
		parsed.ending = ""; 
		for endingbyte = 1, 9, 1 do
			parsed.ending = parsed.ending..string.char(0); -- Value as seen on XWIS
		end;
		binary = binary..parsed.ending;
		
		return binary;
	end;
	
	xwis.preparecurrentinfo = function(servername)
		local parsed = {};
		parsed.serveraccess = "open";
		parsed.disallowedclasses = {};
		parsed.limitresolution = false;
		parsed.resolution = "640x480";
		local playerlist = playerList() or {};
		parsed.players = #playerlist - 1; -- As with "serveronly" flag
		parsed.maxplayers = 31; -- As with "serveronly" flag
		parsed.minping = -1; -- Means "disabled"
		parsed.maxping = -1; -- Means "disabled"
		parsed.mapname = mapGetName();
		parsed.gamename = servername; -- Can be different from actual server name
		parsed.gameflags = gameFlags();
		parsed.fraglimit = scoreFragLimit();
		parsed.timelimit = scoreTimeLimit();
		
		return parsed;
	end;
	
	xwis.registergame = function(parsed)
		private.status = "registering";
		local packet = "JOINGAME #"..private.login.."'s_game 1 "..parsed.maxplayers.." 37 3 1 1 41681088\r\n";
		xwis.talk(packet, function()
			private.status = "registered";
			xwis.updategame(parsed);
		end)
	end;
	
	xwis.updategame = function(parsed)
		if private.status == "registered" then
			local encoded = xwis.encode(parsed)
			local encrypted = xwis.encrypt(encoded);
			local topicchunk = string.char(0x47, 0x31, 0x50, 0x33, 0x9a, 0x03, 0x01);
			local packet = "TOPIC #"..private.login.."'s_game :"..topicchunk..encrypted;
			xwis.talk(packet);
		else
			error("ERROR: The server isn't registered yet!");
		end;
	end;
	
	xwis.getavailableservers = function(func)
		if func == nil or type(func) ~= "function" then
			error("you should provide a function as argument!");
		end;
		local results = {};
		xwis.talk("LIST 37 37", function(data)
			if string.starts(data, ": 326 u #") == true then
				local dataChunks = data:split(" ");
				local encrypted = dataChunks[#(dataChunks)]:sub(13);
				local decrypted = xwis.decrypt(encrypted);
				local parsed = xwis.parse(decrypted);
				table.insert(results, parsed);
			elseif string.starts(data, ": 323 u:") == true then
				func(results);
			end;
		end);
	end;
	
	return xwis;
end;

return xwis;

--[[
gamenamecr = nil;
crashreenc = nil;
crashreencindex = nil;
crashenc = nil;
reparse = {};
unkpack = "";
xwis();
xwis.connect("nwserver1", "4WGnaWGn");
xwis.setReadyCallback(function()
	xwis.talk("LIST 37 37", function(data)
		if string.starts(data, ": 326 u #") == true then
			local dataChunks = data:split(" ");
			local encrypted = dataChunks[#(dataChunks)]:sub(13);
			local decrypted = xwis.decrypt(encrypted);
			print("decrypted!")
			local parsed = xwis.parse(decrypted);
			print("parsed!")
			for k,v in pairs(parsed) do
				if k == "disallowedclasses" then
					local disallowedclasses = "|";
					for k2,v2 in pairs(v) do
						disallowedclasses = disallowedclasses..v2.."|";
					end;
					print("disallowedclasses = "..disallowedclasses);
				else
					print(k.." = "..tostring(v));
				end;
			end;
			if parsed.mapname:starts("Duel") == true then
				gamenamecr = parsed.gamename;
			end;
			local reencoded = xwis.encode(parsed);
			print("reencoded!")
			
			if decrypted ~= reencoded then
				crashreenc = reencoded;
				crashenc = decrypted;
				print("reencoded MISMATCH");
			end;
			
			local reencrypted = xwis.encrypt(reencoded);
			if reencrypted == encrypted then
				print("Reencrypted OK")
			else
				print("Reencrypted mismatch!!!")
				local mismatch_count = 0;
				for i = 1, reencrypted:len(), 1 do
					print(reencrypted:bin_get(i).."|"..encrypted:bin_get(i));
					if reencrypted:bin_get(i) ~= encrypted:bin_get(i) then
						print("MISMATCH");
						mismatch_count = mismatch_count + 1;
					end;
				end;
				print("END");
				print("Mismatch count: "..mismatch_count);
			end;
			reparse = parsed;
			
			local topicchunk = dataChunks[#(dataChunks)]:sub(6, 12);
			print(topicchunk);
			print(topicchunk:len());
			for i = 1, topicchunk:len(), 1 do
				print(bit.tohex(topicchunk:bin_get(i)));
			end;
			local topichunk2 = string.char(0x47, 0x31, 0x50, 0x33, 0x9a, 0x03, 0x01);
			if topicchunk == topichunk2 then
				print ("topicchunk ok");
			else
				for i = 1, topicchunk:len(), 1 do
					print(topicchunk:bin_get(i).."|"..topichunk2:bin_get(i));
					if topicchunk:bin_get(i) ~= topichunk2:bin_get(i) then
						print("MISMATCH");
						
					end;
				end;
			end;
		else
			print("unknown packet?");
			unkpack = data;
		end;
	end);
end)
]]
