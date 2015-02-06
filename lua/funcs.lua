mapMode=function()
        local gf=gameFlags()
        gf=bitAnd(gf, 0x00001FF0)
        if bitAnd(gf, 0x100)>0 then
                return "arena"
        elseif bitAnd(gf, 0x400)>0 then
                return "elimination"
        elseif bitAnd(gf, 0x20)>0 then
                return "ctf"
        elseif bitAnd(gf,0x80)>0 then
                return "chat"
        elseif bitAnd(gf, 0x10)>0 then
                return "kotr"
        elseif bitAnd(gf, 0x40)>0 then
                return "flagball"
        end
        return "unknown"
end

getPlayerList=function()
        local result={}
        for q,v in pairs(playerList())
        do
                table.insert(result,playerInfo(v))
        end
        return result
end

getServerInfo=function()
        return {map=mapGetName(), mode=mapMode(), playerList=getPlayerList()}
end

function getPlayerCountInGame()
        local c=getPlayerList()
        local ingame=0
        for i,q in pairs(c) do
                if q.isObserver==0 then
                        ingame=ingame+1
                end
        end
        return ingame
end

function math.logb(number, base)
	return math.log10(number) / math.log10(base);
end;

function string.starts(String,Start)
	return string.sub(String,1,string.len(Start))==Start
end;

function string.bin_get(str, index)
	return str:byte(index);
end;

function string.bin_set(str, index, value)
	local before = "";
	if index > 1 then
		before = str:sub(1, index-1);
	end;
	
	local after = "";
	if index < str:len() then
		after = str:sub(index+1, str:len());
	end;
	
	str = before..string.char(value)..after
	return str;
end;

bin_ops = {};

function bin_ops.number_memsize(number)
	local size = 0;
	local trim = math.floor;
	if number < 0 then
		trim = math.ceil;
	end;
	while number ~= 0 do
		size = size + 1;
		number = trim(number / 256);
	end;
	return size;
end;

function bin_ops.to_number(bin_str, size, offset)
	offset = offset or 1;
	local maxsize = bin_str:len();
	local warn_truncate = false;
	if maxsize > 7 then
		maxsize = 7; -- Lua allows only up to 7 bytes integers
		warn_truncate = true;
	end;
	
	size = size or maxsize;
	
	if size > maxsize then
		if warn_truncate then
			print("Data truncated due to overflow!");
		end;
		size = maxsize;
	end;
	
	local number = 0;
	local shiftmod = 0;
	for i=offset, offset+size-1, 1 do
		local value = bin_str:bin_get(i);
		value = value * 2 ^ (8 * shiftmod);
		number = number + value;
		shiftmod = shiftmod + 1;
	end;
	return number;
end;

function bin_ops.to_string(number, bytes)
	local stored_bytes = bin_ops.number_memsize(number);
	bytes = bytes or stored_bytes;
	if bytes > 7 then
		bytes = stored_bytes;
	end;
	local trim = math.floor;
	local is_negative = false;
	if number < 0 then
		trim = math.ceil;
		is_negative = true;
	end;
	local bin_str = "";
	local divider = 1;
	for i = 0, bytes-1, 1 do
		number = trim(number / divider);
		local byte_data = number % 256;
		if byte_data == 0 and is_negative == true then
			byte_data = 0xFF;
		end;
		bin_str = bin_str..string.char(byte_data);
		divider = 256;
	end;
	return bin_str;
end;

function string:split(sep)
	local sep, fields = sep or ":", {}
	local pattern = string.format("([^%s]+)", sep)
	self:gsub(pattern, function(c) fields[#fields+1] = c end)
	return fields
end