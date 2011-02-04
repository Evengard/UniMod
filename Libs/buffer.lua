--[[
	Sim,2010
--]]

local ti=table.insert

textBuf={}
function textBuf.new(me,s)
	local ret={}
	setmetatable(ret,{__index=me})
	if type(s)=='string' then
		
	end
	return ret
end

function textBuf.addLine(me,s,pos)
	if pos~=nil then
		if pos>#me then
			for i=#me,pos do
				me[i]=''
			end
			me[pos]=s
		else
			ti(me,pos,s)
		end
	else
		ti(me,s)
	end
	return #me
end
function textBuf.join(me)
	local s=''
	for _,v in pairs(me) do
		s=s..v
	end
	return s
end