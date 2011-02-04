   Bx=0.707
   By=0.707
   x0=23*120
   y0=23*130
do
local fn
function ball(a)
	if a~=nil then Bx=math.cos(a) By=math.sin(a) end
	setMe()
	local ball=createObject('GameBall',23*128,23*128)
	if fn==nil then
		fn=function()
			setTimeout(fn,1)
			netRayFx(0x95,x0,y0,x0+160*Bx,y0+160*By)
			local BallVx,BallVy=unitSpeed(ball)
			if (BallVx==0) and (BallVy==0) then return end
			local x,y=unitPos(ball)
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
			if 0<px*x+py*y then return end
			x,y=unitPos(ball)
			local t=BallVx*Bx+BallVy*By
			local nx=BallVx-t*Bx
			local ny=BallVy-t*By
			line(x,y,x+nx*10,y+ny*10)
			BallVx=BallVx-2*nx
			BallVy=BallVy-2*ny
			unitSpeed(ball,BallVx,BallVy)
		end
	end
	fn()
end
end
