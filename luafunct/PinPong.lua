function Pin(player1)
	local restart=false
	local corToNew
	local corToStar
	corToNew=function(xStar,yStar)
		xStar=xStar*0.707
		yStar=yStar*0.707
		local xNew=xStar+yStar;local yNew=xStar-yStar
		return xNew, yNew
	end
	corToStar=function(xStar,yStar)
		xStar=xStar*0.707
		yStar=yStar*0.707
		local xNew=xStar+yStar;local yNew=xStar-yStar
		return xNew, yNew
	end


	setMe()
	if player1==nil then player1=me end
	local ras=23
	local x0=2760;local y0=2300
	local GameBall=createObject('GameBall',x0,y0)
	local BallFlag=getPtrShort(GameBall,0x10)
	setPtrShort(GameBall,0x10,BallFlag+0x40)
	local Nablud=createObject('smallblueflame',x0,y0)
	local x0New,y0New=corToNew(x0,y0)
	local Centr,BallVx,BallVy,botGo,ballGo,gameNow,ballCollideYesNo, ballCollide
	local corWall={
		x0New+ras*(-10),y0New+ras*(5),
		x0New+ras*(-10),y0New+ras*(-5),
		x0New+ras*(10),y0New+ras*(-5),
		x0New+ras*(10),y0New+ras*(5)}
	local corPl={
		x0New+ras*(-8),y0New+ras*(0),
		x0New+ras*(8),y0New+ras*(0)}

	local TableOfDeth={
		x0New+ras*(10),y0New+ras*(5)+10,1,0,
		x0New+ras*(10),y0New+ras*(-5)+20,1,0,
		x0New+ras*(10)-15,y0New+ras*(5),0,1,
		x0New+ras*(-10)-8,y0New+ras*(5),0,1}


	local ObnovlenRay=function()
		for i=1,#corWall-2,2 do
			local x,y=corToStar(corWall[i],corWall[i+1]); local x1,y1=corToStar(corWall[i+2],corWall[i+3])
			netRayFx(0x95,x,y,x1,y1)
		end
		local i=#corWall
		local x,y=corToStar(corWall[i-1],corWall[i]); local x1,y1=corToStar(corWall[1],corWall[2])
		netRayFx(0x95,x,y,x1,y1)
		for i=1,#corPl,2 do
			local x,y=corToStar(corPl[i],corPl[i+1])
			netRayFx(0x95,x+ras,y-ras,x-ras,y+ras)
			netRayFx(0x95,x-ras,y+ras,x+ras,y-ras)
		end
	end


	gameNow=function()
		BallVx=math.random(-360,360);BallVy=math.random(-360,360)
		local V=math.sqrt(BallVx^2+BallVy^2)
		BallVx=6*BallVx/V
		BallVy=6*BallVy/V
		unitSpeed(GameBall,BallVx,BallVy)
	end

	Centr=function()
		ObnovlenRay()
		botGo()
		ballGo()
		if restart then gameNow() restart=false end
		setTimeout(function() a,b=pcall(Centr) if not a then print(b) end end,1)
	end


	playerOnInput[player1]=function(pl,c,a)
		if c==2 then
			local xp,yp=corToNew(playerMouse(player1))
			local y=corPl[2]
			if yp-y-20<0 then
				if y>(y0New-ras*(4))+13 then corPl[2]=corPl[2]-3 end
			else
				if y<(y0New-ras*(-4))-13 then corPl[2]=corPl[2]+3 end
			end
		end
	return true
	end

	botGo=function()
		local xp,yp=corToNew(unitPos(GameBall))
		local y=corPl[4]
		if yp-y<0 then
			if y>(y0New-ras*(4))+13 then corPl[4]=corPl[4]-3 end
		else
			if y<(y0New-ras*(-4))-13 then corPl[4]=corPl[4]+3 end
		end
	end

	local checkWall=function(x0,y0,Bx,By,BallX,BallY)
			if (BallVx==0) and (BallVy==0) then return end
			local x,y=BallX,BallY
			x=x-x0
			y=y-y0
			local t1=x*Bx+y*By
			local px,py
			px=x+BallVx
			py=y+BallVy
			local t2=px*Bx+py*By
			px=px-t2*Bx
			py=py-t2*By
			x=x-t1*Bx
			y=y-t1*By
			if px*x+py*y>0 then return end
			x,y=BallX,BallY
			local a=BallVx*BallVx+BallVy*BallVy
			local t=BallVx*Bx+BallVy*By
			local nx=BallVx-t*Bx
			local ny=BallVy-t*By
			BallVx=(BallVx-2*nx)
			BallVy=(BallVy-2*ny)
			 --print(string.format('%3.3f - %3.3f',a,BallVx*BallVx+BallVy*BallVy))
	end
	local ox,oy
	ballGo=function()
		BallVx,BallVy=corToNew(unitSpeed(GameBall))
		if BallVx~=ox or BallVy~=oy then
			ox,oy=BallVx,BallVy
		end
		local BallX,BallY=corToNew(unitPos(GameBall))
		for i=1,#TableOfDeth,4 do
			checkWall( TableOfDeth[i],TableOfDeth[i+1],
				TableOfDeth[i+2],TableOfDeth[i+3],
				BallX,BallY)
		end
		BallVx,BallVy=corToStar(BallVx,BallVy)
		local V=1.010367 -- незначительное ускорение
		BallVx=BallVx*V
		BallVy=BallVy*V
		unitSpeed(GameBall,BallVx,BallVy)
	end




	playerLook(player1,Nablud)
	Centr()
	restart=true
end













