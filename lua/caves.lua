--- «десь начинаютс€ описание объектов
caves={
	{
		x=14,y=15;
		R=8; --- радиус в пикселах
		Dev=20; -- отклонение в % от радиуса
		caveSize=25;
	};
	{
		x=8,y=28;
		R=7,Dev=45;
		caveSize=5;
	};
	}

function makeCaves(map,boxW,boxH)
	for _,q in pairs(caves) do
		q.caveWall={}
		q.pointx={}
		q.pointy={}
		local t=2*math.pi/q.caveSize
		for i=1,q.caveSize do
			--q.caveWall[i]=q.R
			q.caveWall[i]=q.R*0.01*(100+math.random(-q.Dev,q.Dev))
			--q.caveWall[i]=q.R*(0.7+0.3*math.sin(0.60*i) )

			local x=math.cos(t*i)
			local y=math.sin(t*i)
			q.pointx[i]=x
			q.pointy[i]=y
		end
		local i=q.caveSize + 1
		q.pointx[i]=q.pointx[1]
		q.pointy[i]=q.pointy[1]
		--q.caveWall[i]=q.caveWall[1]

	end
	for y=0,boxH do
	for x=0,boxW do
		map[y][x]='w'
		for _,q in pairs(caves) do
			local x0=x-q.x+0.5
			local y0=y-q.y+0.5
			local r=q.R+q.Dev
			if (x0>-r and x0< r) and (y0>-r) and y0<r then
			-- возле окружности, считаем точнее
				local idx=0
				if q.candidate==nil then
					for i=1,q.caveSize do
						local x1,y1=q.pointx[i],q.pointy[i]
						local x2,y2=q.pointx[i+1],q.pointy[i+1]

						if y0*x1>=x0*y1 and y0*x2<x0*y2 then
							idx=i
							break
						end
					end
					-- q.candidate=idx
				else
					-- что-то лень мне с кэшем разбиратс€, не буду его делать
				end
				if idx>0  then
		--[[ вариант с интерпол€цией пр€мых
					local x1,y1=q.pointx[idx],q.pointy[idx]
					local x2,y2=q.pointx[idx+1],q.pointy[idx+1]
					x0=x0-x1
					y0=y0-y1
					x2=x2-x1
					y2=y2-y1
					if x2*y0>x0*y2 then
						boxes[i]=0
					end
		--]]
		-- вариант с отсечкой дугами
					r= q.caveWall[idx]
					if x0*x0+y0*y0<r*r then
							map[y][x]='.'
					end
		--]]
				end
			end
		end
	end
	end
end
