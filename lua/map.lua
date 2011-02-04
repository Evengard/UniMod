function wscan()
	if me==nil then setMe() end
	if wscan==nil then return end
	local x1,y1=playerMouse(me)
	local Wall=mapGet(x1,y1,true)
	local tile,x,y,face,flag,vari,unk3,HP=mapGet(x1,y1)
	local GlobalId
	if Wall~=nil then GlobalId=getPtrShort(Wall,0xA) end
	if tile~=nil then
		print('Name:',mapGetNameByTile(tile),' Tile:',tile,' X:',x*23,' Y:',y*23,' Face:',face,' Flag:',flag,' Vari:',vari,' unk3:',unk3,' HP:',HP,' Id:',GlobalId)
	end
setTimeout(wscan,60)
end
function mkw(tile,Face,Flag,Vari,unk3,Hp)
	if me==nil then setMe() end
	if tile==nil then tile=12 end
	if Face==nil then Face=0 end
	if Flag==nil then Flag=0 end
	if Vari==nil then Vari=0 end
	if unk3==nil then unk3=0 end
	if Hp==nil then Hp=1 end
	local x,y=playerMouse(me)
	if not((x%2==0 and y%2==0) or (x%2~=0 and y~=0)) then print('wrong angle') return end
   mapSet(tile,x,y,Face,Flag,Vari,unk3,Hp)
    return mapGet(x,y,true)
end
function atMouseW()
if me==nil then setMe() end
local x,y=playerMouse(me)
local wall=mapGet(x,y,true)
if wall==nil then print('wrong args') return end
return wall
end
function mapChangeTile(Tile,Wall)
if me==nil then setMe() end
if Wall==nil then Wall=atMouseW() end
if Tile==nil or Tile<0 then Tile=0 end
setPtrByte(Wall,1,Tile)
end


--[[
Флаги стен
0 - неразрушимая
8 - разрушимая
60 - невидимая ни на карте ни на радаре
132 - секретная(неработает)
192 - с окном
200 - с окном разрушимая
]]--

function cageMe(obj)
if obj==nil then
if me==nil then setMe() end obj=me end
local x,y = unitPos(obj)
x=x-x%46
y=y-y%46
unitMove(obj,x+11.5,y+11.5)
local Ras = 23
mapSet(12,x+Ras,y+Ras,0,0,0,0,1) -- /
mapSet(12,x-Ras,y-Ras,0,0,0,0,1) --   /
mapSet(12,x+Ras,y-Ras,1,0,0,0,1) -- \
mapSet(12,x-Ras,y+Ras,1,0,0,0,1) --  \
mapSet(12,x,y-(Ras*2),9,0,0,0,1) -- угол правый верхний
mapSet(12,x-(Ras*2),y,8,0,0,0,1) -- угол левый верхний
mapSet(12,x,y+(Ras*2),7,0,0,0,1) -- угол левый нижний
mapSet(12,x+(Ras*2),y,10,0,0,0,1) -- угол правый нижний
end

WallModeTile=61
WallModeFace=0
WallModeFlag=0
WallModeVari=0
WallModeUnk3=0
WallModeHp=0
WallModeCopyTile=0
WallModeCopyFace=0
WallModeCopyFlag=0
WallModeCopyVari=0
WallModeCopyUnk3=0
WallModeCopyHp=0

function wallMode()
conExec('bind f3 wallModeSetWall()')
conExec('bind f4 wallModeNil()')
conExec('bind f5 wallModeNilAll()')
conExec('bind f6 wallModeCopy()')
conExec('bind f7 wallModeSetCopy()')
end

function wallModeSetWall()
mkw1(WallModeTile,WallModeFace,WallModeFlag,WallModeVari,WallModeUnk3,WallModeHp)
WallModeFace=WallModeFace+1
end

function wallModeNil()
WallModeFace=0
end

function wallModeNilAll()
WallModeFace=0
WallModeFlag=0
WallModeVari=0
WallModeUnk3=0
WallModeHp=0
end

function wallModeCopy()
local a,b -- что бы было
WallModeCopyTile,a,b,WallModeCopyFace,WallModeCopyFlag,WallModeCopyVari,WallModeCopyUnk3,WallModeCopyHp=mapGet(playerMouse(me))
end

function wallModeSetCopy()
mkw1(WallModeCopyTile,WallModeCopyFace,WallModeCopyFlag,WallModeCopyVari,WallModeCopyUnk3,WallModeCopyHp)
end
