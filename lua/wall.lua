
mapNearFlag={0,0, --для определния направления
			4,0,
			16,0,
			10,1,
			2,1,
			8,1,
			30,2,
			22,3,
			14,4,
			28,5,
			26,6,
			6,7,
			12,8,
			24,9,
			18,10}
mapCorForWall={-1,-1,1,-1,1,1,-1,1} --надо что бы легко доставать ближайшие стены


function setWall()
	local x,y=cliPlayerMouse()
	x=x-x%23
	y=y-y%23
	if (x%2==1 and y%2==1) or (x%2==0 and y%2==0) then
		local dir,flag
		flag=getNearWallFlags(x,y) --получаем что надо
		for i=1,#mapNearFlag,2 do -- ставим сначало нашу стену
			if mapNearFlag[i]==flag then dir=mapNearFlag[i+1] end
		end
		local mayWall=mapSet(1,x,y,dir,0,0,0)
		local walls=getNearWall(mayWall)
		for i=1,#walls do --теперь меняем стены ближайшие
			flag=getNearWallFlags(mapGetPos(walls[i]))
			for j=1,#mapNearFlag,2 do
				if mapNearFlag[j]==flag then setPtrByte(walls[i],0, mapNearFlag[j+1]) end
			end
		end
	end
end

function getNearWallFlags(x,y)
	local c=0 -- то на основании чего мы будем смотреть направление
	local a
	for i=1,8,2 do
		local x1=x+23*mapCorForWall[i]
		local y1=y+23*mapCorForWall[i+1]
		if mapGet(x1,y1,true)~=nil then c=c+2^((i+1)/2) end
	end
	return c
end


function getNearWall(x)
	local x,y=mapGetPos(x)
	local t={}
	for i=1,8,2 do
		local x1=x+23*mapCorForWall[i]
		local y1=y+23*mapCorForWall[i+1]
		a=mapGet(x1,y1,true)
		if a~=nil then table.insert(t,a)  end
	end
	return t
end


function mapGetPos(a)
	local x=getPtrByte(a,5)*23 --сори но ты сам говорил, что нафиг надо
	local y=getPtrByte(a,6)*23
	return x,y
end


