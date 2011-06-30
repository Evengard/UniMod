function drawP()
	local x,y=screenGetPos()
	local a,b=screenGetSize()
	local c=stringGetSize('P')*2
	x=x+a/2
	y=y+b/2
	x,y=posIsom(x,y)
	x=x-x%46;y=y-y%46
	x=x-46*4;y=y-46*4
	local x1,y1
	for i=0,8 do
		for j=0,8 do
			x1=x+j*46;y1=y+i*46
			x1,y1=posWorldToScreen(posNorm(x1-23,y1+23))
			stringDraw('P',x1-c,y1,'#FFFFFF')
			x1=x+j*46;y1=y+i*46
			x1,y1=posWorldToScreen(posNorm(x1,y1))
			stringDraw('P',x1-c,y1,'#FF0000')
		end
	end
	snapToGrid()
	cliSetTimeout(drawP,1)
end

function posIsom(x,y)
	local x1,y1
	x1=x+y;y1=x-y
	return x1,y1
end

function posNorm(x,y)
	local x1,y1
	x1=x/2+y/2;y1=x/2-y/2
	return x1,y1
end



function snapToGridC()
	local x,y=posIsom(cliPlayerMouse())
	if (x+23)%46>(x)%46 then x=x-46 end
	if (y+46)%46<y%46 then y=y+23 end
	x=(x-x%46)+23;y=(y-y%46)+23 -- центр
	xGrid,yGrid=x,y
	x,y=posWorldToScreen(posNorm(x,y))
	stringDraw('P',x+11,y,'#0000FF')
	cliSetTimeout(snapToGridC,1)
end

function snapToGrid()
	local x,y=posIsom(cliPlayerMouse())
	if (y+23)%46<y%46 then y=y+46 end
	x=(x-x%46);y=(y-y%46)
	xGrid,yGrid=posNorm(x,y)
	x,y=posWorldToScreen(xGrid,yGrid)
	stringDraw('P',x+11,y,'#0000FF')
	cliSetTimeout(snapToGrid,1)
end
