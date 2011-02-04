-- UniMod.map
dofile('maps/UniMod/second.lua')

do 

	local ti=table.insert
	local oldRec=nil
	createCell=function ()

--		print(tostring(scriptTrigger)) -- на что жали
--		print(tostring(scriptCaller))  -- чем жали
--		print(tostring(scriptParent))  -- кто в этом виноват (прикол с бомбером будет)

		if oldRec~=nil then -- убираем старую клетку
			for _,q in ipairs(oldRec) do
				if q[1]==nil then
					mapDel(q.x,q.y)
				else
					mapSet(q[1],q.x,q.y,q[4],q[5],q[6],q[7],q[8])
				end
				
			end
			oldRec=nil
			return	
		end
		oldRec={}
		local x0,y0=unitPos(scriptParent)
		x0=x0-x0%46
		y0=y0-y0%46
   		 local ras=23
		local cell={
			1,1,  0,
			-1,-1,0,
			1,-1,1,
			-1,1,1,
			0,-2,9,
			-2,0,8,
			0,2,7,
			2,0,10 }
		for i=1,#cell,3 do
			local x,y
			x=x0+ras*cell[i];y=y0+ras*cell[i+1]  -- ищем координаты
			local tile= { mapGet(x,y) } -- читаем старую стену
			tile.x=x;tile.y=y -- запоминаем ее координаты
			ti(oldRec,tile) -- запоминаем старую стену
			mapSet(12,x,y,cell[i+2],0,0,0,0,10) -- создаем клетку

			if tile[1]~=nil then -- чтобы за стены не цеплялось
				mapSet(tile[1],x,y,2,0,0,0,0,10)
			end
		end 
	end
end



--0 0 
--46 46 

function pin(player1)
	if player1==nil then setMe() player1=me end
	local ras=23
	local x0=2760;local y0=2300
	local Centr
	local Ball=createObject('gameball',x0,y0)
	local BallFlag=getPtrShort(Ball,0x10)
	setPtrShort(Ball,0x10,BallFlag+0x40)
	local BallVx,BallVy
	local DlinPalka=dist(0,0,46,46)
	local Nablud=createObject('smallblueflame',x0,y0)
	local corWall={
			x0+ras*(-3),y0+ras*(-9),
			x0+ras*(-9),y0+ras*(-3),
			x0+ras*3,y0+ras*9,
			x0+ras*9,y0+ras*3}
	local corPl={
			x0+ras*(-4),y0+ras*(-4),
			x0+ras*4,y0+ras*4}

	local ObnovlenRay=function()
		for i=1,#corWall-2,2 do
			netRayFx(0x95,corWall[i],corWall[i+1],corWall[i+2],corWall[i+3])
		end
		local i=#corWall
		netRayFx(0x95,corWall[i-1],corWall[i],corWall[1],corWall[2])
		for i=1,#corPl,2 do
			netRayFx(0x95,corPl[i]+ras,corPl[i+1]-ras,corPl[i]-ras,corPl[i+1]+ras)
			netRayFx(0x95,corPl[i]-ras,corPl[i+1]+ras,corPl[i]+ras,corPl[i+1]-ras)
		end
	end
	
	local gameNow=function()
		BallVx=math.random(-360,360);BallVy=math.random(-360,360)
		local V=math.sqrt(BallVx^2+BallVy^2)
		BallVx=10*BallVx/V
		BallVy=10*BallVy/V
	end 
	local ballGo=function()
		unitSpeed(Ball,BallVx,BallVy)
	end
	
	--________________бот
	local BotGo=function()
		local x=corPl[3];local y=corPl[4]
		print ('1',me)
		local x1,y1=unitPos(me)
		local NachaloX=x-ras;local NachaloY=y+ras 
		local KonecX=x+ras;local KonecY=y-ras
		Lx=(KonecX-NachaloX)/DlinPalka;Ly=(KonecY-NachaloY)/DlinPalka
	end
	
	Centr=function()
		ObnovlenRay()
		ballGo()
		setTimeout(function() pcall(Centr) end,1)
	end	
	
	--________плаер часть
	playerOnInput[player1]=function(pl,c,a)
		if c==2 then
			local xp,yp=playerMouse(player1)
			local y=corPl[2]
			if yp-y<0 then
				if y>(y0-ras*6)+2 then corPl[2]=corPl[2]-3;corPl[1]=corPl[1]+3 end
			else
			    if y<(y0-ras*2)-2 then corPl[2]=corPl[2]+3;corPl[1]=corPl[1]-3 end
			end
		end
	return true
	end
	--________ енд плаер часть
	playerLook(player1,Nablud)
	gameNow()
	Centr()	
	BotGo()
end

function pinPong(firstPlayer)
-- _____________________________________ определяем центральную точечку, ставим там меточку и расставляем объектики
	if firstPlayer==nil then firstPlayer=me end 
	local goPlayer  
	local x0=2760;y0=2300
	local ras=23
	createObject('smallblueflame',x0,y0)
	local Ball=createObject('barrelsteel1',x0,y0)
	local Player1=createObject('lightbench2',x0-ras*4,y0-ras*4)
	local Player2=createObject('lightbench2',x0+ras*5,y0+ras*5)
--_________________________________________енд
-- _____________________________________ строим стены для пинг понга
	local cell={
		3,-3,1, -- право середина
		2,-4,1, -- все что право и вверх
		1,-5,1,
		0,-6,1,
       -1,-7,1,
	   -2,-8,1,
       -3,-9,9, -- угол правый верхний
	   -5,-7,0, -- все что сверху и вправо
	   -4,-8,0,
	   -6,-6,0, -- вверх середина
	   -7,-5,0, -- все что сверху и влево
	   -8,-4,0,
	   -9,-3,8, -- угол левый вверхний
	   -8,-2,1, -- все что влево и вверх
	   -7,-1,1,
		-6,0,1,
		-5,1,1,
		-4,2,1,
		-3,3,1, -- лево середина 
		-2,4,1, -- все что слево и вниз
		-1,5,1,
		 0,6,1,
		 1,7,1,
		 2,8,1,
		 3,9,7, -- левый нижний угол
		 4,8,0, -- все что снизу и влево
		 5,7,0,		 
		 6,6,0, -- низ середина
		 7,5,0, -- все что снизу и влево
		 8,4,0,
	    9,3,10, -- правый нижний угол
	     8,2,1,
	     7,1,1,
	     6,0,1,
	     5,-1,1,
		 4,-2,1} 
	for i=1,#cell,3 do
		local x,y
		x=x0+ras*cell[i];y=y0+ras*cell[i+1]
		mapSet(47,x,y,cell[i+2],0,0,0,0,10)
	end
-- _____________________________________ енд строим стены для пинг понга
	playerLook(firstPlayer,Player1)
	goPlayer=function(Y,Y1,obj) -- двигаем палки плееров
		local speed=20
		Vy=(Y1-Y) 
		V=math.sqrt(Vy^2)
		Vy=speed*Vy/V
		if Vy<0 then 
			if Y>y0-ras*6 then unitSpeed(obj,5,-5) end
			else if Y<y0-ras*3 then unitSpeed(obj,-5,5) end
		end
	end
	playerOnInput[firstPlayer]=function(pl,c,a)
		if c==2 then
			print ('2',pl)
			local xp,yp=unitPos(pl)
			local xn,ym=playerMouse(firstPlayer)
			goPlayer(yp,ym,pl)
		end
	return true
	end
end

