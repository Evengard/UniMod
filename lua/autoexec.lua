function onFirstFrame()
	print('loaded Autoexec.lua')
end

function kick(s) if not s then return end conExec('kick "'..s..'"') end
function ban(s) if not s then return end conExec('ban "'..s..'"') end

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

function del(x)
 if x==nil then x = lastObj end
 if x==nil then return end
 if getThingType(x)==0x2c9 then return end
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

gtt=getThingType
stt=setThingType

function ph(x)
	print(printf('0x%X',x) )
end

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



function getUnitClass(w)
	return getPtrInt(w,0x8)
end
function getUnitSubclass(w)
	return getPtrInt(w,0xC)
end
function setUnitClass(w,x)
	return setPtrInt(w,0x8,x)
end
function setUnitSubclass(w)
	return setPtrInt(w,0xC,x)
end


setTimeout(onFirstFrame,1)

function makeGold(amount)
  local x=cr('Gold')
  local p=getPtrPtr(x,0x2B4)
  setPtrInt(p,0,amount)
end


function follow(x)
  if x==nil then return end
  setMe()
  x=cr(x)
  setTimeout(function()
     setFollow(x,me)
     becomePet(x,me)
  end,
  15)
end



function angryCrown()
 local x=cr 'Crown'
 local time=0
 local dt=3
 local obj=gtt('SmallFlame')
 fireCnt=0
 fn=function(me,him,x,y)
    if him==nil then print('him==nil') return end
    if gtt(him)==obj then return end
    if getFrameCounter() < time then return end
    time=getFrameCounter()+dt
    dt=dt+2
    setTimeout(function(x) if x > time then dt=3 end end,20)
    local p=unitPos(me)
    local f=createObject('SmallFlame',p[1]-x,p[2]-y)
    unitDecay(f,30)
    fireCnt=fireCnt+1
   end
 unitOnCollide(x,fn)
end

function crSpell(spellN)
 local x=cr 'SpellBook'
 local t=getPtrPtr(x,0x2E0)
 setPtrInt(t,0,getPtrInt(t,0)+spellN)
 return x
end

onPlayerJoin=function(pl)
print('Hi!')
end

netTT={}
netByCode={}
function clientOnUnitHover(tt,netCode)
	if (netByCode[netCode]~=nil) then return netByCode[netCode] end
	if (netTT[tt]~=nil) then return netTT[tt] end
end

ser=serialize

lineColor=1
lineDraw=true
lineColors={0x8C,0x095}
function line(x1,y1,x2,y2)
	local t
	local c=lineColor
	local d=120
	t=function ()
		if d<0 then return end
		d=d-1
		if lineColors[c]==nil then return end
		if lineDraw then setTimeout(t,1) end
		netRayFx(lineColors[c],x1,y1,x2,y2)
	end
	t()
end

function circle(x0,y0)
	local r=math.random(50,100)
	local t={}
	local x,y
	for i=0,360,36 do
		x=r*math.cos(i*3.14/180)+x0
		y=r*math.sin(i*3.14/180)+y0
		table.insert(t,x)
		table.insert(t,y)
	end
	for i=3,#t,2 do
		line(t[i-2],t[i-1],t[i],t[i+1])
	end
	line(t[#t-1],t[#t],t[1],t[2])
end

function setSentrySpeed(obj,angl)
	if obj==nil or getThingName(getThingType(obj))~="SentryGlobe" then return end
	local uc=getPtrPtr(obj,0x2ec) -- юнитконтроллер
	if angl==nil then angl=180 end
	angl=angl*(3.14/180)
	setPtrFloat(uc,8,angl)
end



