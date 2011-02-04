local onFirstFrame=function ()
	print('loaded defaults') 
end

defPoint={2900,2900}

function atMouse()
 if me==nil then return end
 local x,y= playerMouse(me)
 local obj=nil
 local fn=function(p) obj=p end
 local d=1
 unitGetAround(fn,x-d,y-d,x+d,y+d)
return obj
end

function pick()
 lastObj=atMouse()
 if not lastObj then setTimeout(pick,15) return end
 print ('picked '..getThingName(lastObj) )
end

function atMouse(x,y)
 if me==nil then return end
 if x==nil or y==nil then x,y = playerMouse(me) end 
 local obj=nil
 local fn=function(p) obj=p end
 local d=5
 unitGetAround(fn,x-d,y-d,x+d,y+d)
return obj
end

function del(x)
 if x==nil then x = lastObj end
 if x==nil then return end
 unitDelete(x)
end

function cr(x)
 local r
 if(me==nil) then setMe() end
 if me==nil then return end
 r=createObject(x,playerMouse(me))
 if r==nil then print('wrong Obj') end
 return r
end


function ph(x)
	print(printf('0x%X',x) )
end

lastObj=nil


function setMe(x) 
 if x==nil or x==0 then x=1 end
 me=nil
 local pl = playerList()
 me = pl[x]
 if me~=nil then
   defPoint=unitPos(me)
 end
end


function getInv(w) 
 --[[



   достает объекты инвентаря 
   пример использования - перебор всех элементов инвентаря:

    setMe()
    for o in getInv(me) do
       print(getThingName(o))
    end
   другой пример использования - получение первого предмета
   _,obj = getInv(me)



 ]]--
   return function (t,x)
        if x==nil then return t end
        return getPtrPtr(x,0x1F0) 
      end, getPtrPtr(w,0x1F8),nil
end

function inform()
	noxOnCreateAt=function(o,p,x,y) 
		print(printf('0x%X %f,%f',getThingType(o),x,y) )
		lastObj=o 
	end
	-- теперь после каждого создания можно достать последний созданый объект из lastObj
end

function makeTrap()
   if not me then setMe() end
   local t
   local x,y=unitPos(me)
   t=createObject('Pit',playerMouse(me))
   if (t==nil) then print 'unable' else print('ok') end
   t=getPtrPtr(t,0x2BC)
   setPtrInt(t,0x8,x)
   setPtrInt(t,0xc,y)
end

setTimeout(onFirstFrame,1)

function makeGold(amount)
	local x=cr('Gold')
	local p=getPtrPtr(x,0x2B4)
	if (type(amount)=='number') then
		setPtrInt(p,0,amount)
	end
end


function ufl(x) -- собирает всю инфу
  print(printf('Flags:0x%08x Class:0x%08x SubClass:0x%08x',
     getPtrInt(x,16),getPtrInt(x,8),getPtrInt(x,12) ))
end

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
		if me==nil then setMe() end
		 obj=me 
	end 
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

function spell(targ,type)
 local x=cr('Magic')
 local uc=getPtrPtr(x,0x2EC)
 if (targ~=nil) then setPtrPtr(uc,4,targ) end
 if type~=nil then
   setPtrPtr(uc,   0,x) -- владелец (который от зеркала)
   setPtrPtr(uc,   8,x) -- создатель (кстати возможно они наоборот)
 -- тут наглое жульничество -получается что тебя заколдовало само колодовство
   setPtrInt(uc, 0xC,type)
   setPtrInt(uc,0x10,3) -- spell power
 end
end
