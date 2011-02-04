local barrel
local pit
local elevId=getThingType('Elevator') -- запоминаем заранее код

function act1()
	if barrel==nil then -- ищем стальную бочку
		local id=getThingType('barrelSteel1')
		unitGetAround(
			function(x) if getThingType(x)==id then barrel=x end end
			,1500,1500,4000,4000)
	end
	if barrel==nil then return end
	if pit==nil then --ищем выход для элеватора
		local id=getThingType('elevatorPit')
		unitGetAround(
			function(x) if getThingType(x)==id then pit=x end end
			,1500,1500,4000,4000)
	end
	if pit==nil then return end
	local mayCreate=true
	local id=getThingType('elevatorPit')
	local x,y=unitPos(barrel)
	local d=50
	unitGetAround(  -- проверяем нет ли поблизости всяких предметов
		function(x) 
			if getThingType(x)==id then mayCreate=false end 
			if getThingType(x)==elevId then mayCreate=false end 
		end
		,x-d,y-d,x+d,y+d)
	if not mayCreate then 
		print 'too near'
		return end
	local t=createObject('elevator',unitPos(barrel))
	local uc=getPtrPtr(pit,0x2EC) -- пока таким ужасным образом

	local old=getPtrPtr(uc,4) 
	if old then  -- отключаем старый
		local uc2=getPtrPtr(old,0x2EC)
		setPtrInt(uc2,4,0)
		setPtrInt(uc2,12,4) --переводем элеватор в состояние "выключен"
	end
	setPtrPtr(uc,4,t) -- подключаем элеватор к дыре
	uc = getPtrPtr(t,0x2EC)
	setPtrPtr (uc,4,pit) -- и дыру к элеватору
  setTimeout(function() unitMove(barrel,x,y) end,1) -- помещаем бочку на лифт через 1 кадр, так как лифт должен создаться
end


local act2_Bone
function act2()
	if act2_Bone~=nil then
		for i=1,#act2_Bone do
			unitDecay(act2_Bone[i],1)
		end
	act2_Bone=nil
	end
	local x,y,r,t,x0,y0
	if scriptParent==nil then return end
	x0,y0=unitPos(scriptParent)
	r=80
	local boneN={'corpseskullW','corpseribcageW','corpseLeftLowerArmW','corpseLeftUpperArmW','corpseRightLowerArmW','corpseRightUpperArmW','corpsePelvisW'}
	act2_Bone={}
	for t=0,360,10 do
		x=r*math.cos(t*3.14/180)+x0
		y=r*math.sin(t*3.14/180)+y0
		obj=math.random(#boneN)
		table.insert(act2_Bone,createObject(boneN[obj],x,y))
	end
end

function act3()
	local x=math.random(2670,3000)
	local y=math.random(2880,2990)
	local butilka={crb,crw}
	local num=math.random(1,#butilka)
	num=butilka[num]
	num(x,y)
end
	

function act4()
print('act4')
end

function ExFrog()
unitMove(scriptParent,2793,2907)
end


local rand=math.random
local abs=math.abs
function makeFrog(t1,t2,t3)
	if not me then setMe() end
	local plr=me
	local frog=createObject('greenFrog',100,100)
	local a,b,c,d,e
	local x0,y0= unitPos(plr)
	local oldx,oldy
	oldx=x0;oldy=y0
	local at,bt -- для поправки баги с дергающейся жабой
	playerOnInput[plr]=function(pl,c,targ)
		if c==1 then
			if at and at<getFrameCounter() then 
				unitActionPop(frog) 
				at=nil
			end
			return false 
		end
		if c==2 then -- правой кнопкой - идем
			local x,y=playerMouse(pl)
			if (not at) or at<getFrameCounter()  then
				unitActionPop(frog) -- отменяем прошлую команду
				local t=unitSetAction(frog,7) -- идем
				at=getFrameCounter()+15
				setPtrFloat(t,4,x)
				setPtrFloat(t,8,y)
				setPtrInt(t,0x0C,0)
			end
		end
		if c==6 then  -- левой кнопкой колдуем
			if (not targ) then
				local x,y=playerMouse(pl)
				local d=1
				targ=unitPick(x,y,d) -- сканим просто
			end
		
			if (targ) and (not bt or bt < getFrameCounter() ) then 
				unitActionPop(frog) -- отменяем прошлую команду
				local t=unitSetAction(frog,18) -- колдовать спелл в цель
				bt = getFrameCounter() + 30
				setPtrInt(t,0x04,12)
				setPtrPtr(t,0x0C,targ)
			end
		end
		return true 
	end
	
	unitOnDie(frog,function () 
		print('re')
		unitOnCastList[frog]=nil 
		playerOnInput[plr]=nil
		playerOnSpell[plr]=nil
		unitMove(plr,oldx,oldy)
		end)
	c=function(s,me,to,fly,tx,ty) 
		if (fly) then -- cast
			unitFlyActivate[fly]=d
			unitOnCastList[frog]=nil
		end
	end
	d=function()
		unitMove(plr,2528,2647) -- можно положить в колонну 
		setTimeout(function() playerLook(plr,frog) end,1)
		
		playerOnSpell[plr]=e
		unitOnCastList[frog]=function(s,me,to,fly,tx,ty)
			-- пущай палит молотками
			if (fly) then
				unitFlyActivate[fly]=function(SpellType,Owner,Source,Carrier,X,Y,Power)
					spellApply(Owner,52,Carrier,plr,X,Y,3)
				end
			end
		end

	end
	e=function(plr,targ,dir,spell,tx,ty)
--		print('d:',dir,',spell:',spell)
		if (targ) then
			local t=unitSetAction(frog,18) -- колдовать спелл в цель
			setPtrInt(t,0x04,spell)
			setPtrPtr(t,0xC,targ)
		end
		return -1;
	end

	local x,y
	local dx,dy
	for i=1,20 do
		dx= rand(-46,46)
		dy= rand(-46,46)
		x= x0 + dx*3
		y= y0 + dy*3
		if not (dx+dy<10) and mapTraceRay(x,y,x0,y0,5) then
			break
		end
	end
	a=function()
		unitMove(frog,x,y)
		netPointFx(0x81,x,y)
		netPointFx(0x8A,x,y)
		netPointFx(0x89,x,y)
	    setTimeout(b,15)
    	unitOnCastList[frog]=c -- ловим каст от жабы который потом будет

	end
	b=function()
	    local t=unitSetAction(frog,18) -- колдовать спелл в цель
		setPtrInt(t,4,12)
		setPtrPtr(t,0xC,plr)
		setTimeout(d,12)
	end

	setTimeout(a,3)
end

function crb(x1,y1)
  if me==nil then setMe() end
  x=createObject('BlackPotion',x1,y1)
  local t=memAlloc(4)
  setPtrInt(t,0,1)
  setPtrPtr(x,0x2E0,t)
  local acts={
	function(t) spellApply(t,3,t) end,
	function(t) 
		spellApply(t,15,t)
		end,
	function(t) spellApply(t,3,t) end,
       }

  local fn=function (to,me)
        local x=math.random(#acts)
        x=acts[x]
        x(to)
	unitDelete(me) 
	return 1
  end
  unitOnUse(x,fn)
end

function crw(x1,y1)
  if me==nil then setMe() end
  x=createObject('WhitePotion',x1,y1)
  local t=memAlloc(4)
  setPtrInt(t,0,1)
  setPtrPtr(x,0x2E0,t)
  local fn=function (to,me)
	spellApply(to,2,to)
	unitDelete(me) 
	return 1
  end
  unitOnUse(x,fn)
end