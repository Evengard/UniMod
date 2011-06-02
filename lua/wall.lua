
local mapNearFlag={0,0, --дл€ определни€ направлени€
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
local mapCorForWall={-1,-1,1,-1,1,1,-1,1} --надо что бы легко доставать ближайшие стены
local dlg
local wallP={}
wallP.tile=mapTileByName'rockDark' --"decidiousWallGreen"
wallP.var=-1 -- -1 значит рандом
wallP.flag=0
wallP.hp=0

function setWall()
	local x,y=cliPlayerMouse()
	x=x-x%23
	y=y-y%23
	if (x%2==1 and y%2==1) or (x%2==0 and y%2==0) then
		local dir,flag,var,t
		flag=getNearWallFlags(x,y) --получаем что надо
		for i=1,#mapNearFlag,2 do -- ставим сначало нашу стену
			if mapNearFlag[i]==flag then dir=mapNearFlag[i+1] end
		end
		if wallP.var==-1 then var=math.random(0,mapMaxVari(wallP.tile,dir)-1) else var=wallP.var end
		local mayWall=mapSet(wallP.tile,x,y,dir,wallP.flag,var,wallP.hp)
		local walls=getNearWall(mayWall)
		for i=1,#walls do --теперь мен€ем стены ближайшие
			flag=getNearWallFlags(mapPos(walls[i]))
			for j=1,#mapNearFlag,2 do
				if mapNearFlag[j]==flag then
					setPtrByte(walls[i],0, mapNearFlag[j+1])
					t=mapInfo(walls[i])
					print(t.vari)
					if t.vari>mapMaxVari(t.tile,t.dir)-1 then setPtrByte(walls[i],2,0) end
				end
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
		if mapGet(x1,y1)~=nil then c=c+2^((i+1)/2) end
	end
	return c
end


function getNearWall(x)
	local x,y=mapPos(x)
	local t={}
	for i=1,8,2 do
		local x1=x+23*mapCorForWall[i]
		local y1=y+23*mapCorForWall[i+1]
		a=mapGet(x1,y1)
		if a~=nil then table.insert(t,a)  end
	end
	return t
end


function mapPos(a)
	local t=mapInfo(a)
	local x=t.x * 23
	local y=t.y * 23
	return x,y
end

function wallDel()
	local x,y=cliPlayerMouse()
	x=x-x%23
	y=y-y%23
	mapDel(x,y)
end

function wallDlg()
	if dlg~=nil then return end
	local t
	local ti=table.insert
	local width=200
	local height=400
	dlg={x=0,y=0,w=width,h=height,bgcolor='#0'}
	t={ --«акрываем окно
		type="PUSHBUTTON",
		status="ENABLED+IMAGE",
		style="MOUSETRACK",
		x=width-55,y=height-35,w=45,h=25,
		offsetX=0,offsetY=-1,
		selectedImage="UIButtonSmLit";
		hiliteImage="UIButtonSmLit";
		image="UIButtonSm";
		disabledImage="UIButtonSmDis";
		textColor='#E6A541';
		text="«акрыть",
		onClick=function(a,b) wndClose(b) dlg=nil end
	}
	--t={ -- лист бокс



	ti(dlg,t)
	wndCreate(dlg)
end
