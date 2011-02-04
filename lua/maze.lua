
mazeSize=15
mazeX,mazeY=104,110
mazeTile=47
mazeTile2=66

mazeTemplates=[[
{
'_OwwwO_',
'OO...OO',
'w.AAA.w',
'w.AAA.w',
'w.AAA.w',
'OO...OO',
'_OwwwO_',
objects={
	{type= 'Meat';x=2;y=4},
	{type= 'Meat';x=4;y=2},
	{type= 'Meat';x=6;y=4},
	{type= 'torch';x=4;y=6},
	};
walls={
	O={tile=0};
	w={tile=0};
	A={tile=36};
	},
complete=true; --пускай ставится только если целиком
},
{ -- кусок корридора
'wwwww',
'w. .w',
'w   w',
'w. .w',
'wwwww',
walls={
	w={tile=mazeTile};
	},
objects={ {type='redpotion';x=2;y=2},};
};
]]
--[[
	Символы элементов в буквах (для шаблонов)
	(пробел) - уже посещенная пустая территория
	. - еще не посещенная территория
	_ - не изменяемая территория т.е. как-бы прозрачная. нужна для скруглений и т.п.
	w - (любая маленькая буква) стена которую можно убрать
	W - (любая большая буква) стена которую убрать нельзя

	Символы в цифрах
	='.'	- еще не посещенная территория
	=0		- уже были, пустое место
	<0		- стену нельзя убрать
	>0		- стену можно убрать
]]--

--[[
параметры шаблона
objects={
	{
		type='Urchin';
		have={ {type='redpotion'}, и что из него еще падает}
		x=4;y=4
	},
	 и так далее };
walls={
		буква = {параметры стены для создания}
	},
complete=true; --не может ставится если выходит за край карты
nomirror=true; --не переворачивается/отражается
]]--

local map -- сдесь будет лежать сама карта
local tiles -- сюда надо класть отображения
local tilePlus,tileMinus=2, (-2) -- свободный номер для отображений
local walkEvents={} -- таблица для событий при ходьбе 2й фазы по карте
local mzObjects -- сюда кладут объекты из шаблонов
local ti=table.insert

local mzGet=function(x,y) if y<1 or x<1 or x>mazeSize or y>mazeSize then return -1 end return map[y][x] end
local mzToGlobal=function(x,y) -- переходит в мировые координаты
	return 23*(mazeX+x+y),23*(mazeY+y-x)
end
local function placeTemplates(tiles)
	if 0==#tiles then return end
--	attachTile(2,2,tiles[2],true) return
--[[ простой вариант случайных элементов
	for i=1,5 do
		local t=math.random(1,#tiles)
		t=tiles[t]
		local tx,ty
		if t.complete then
			tx,ty=math.random(0, math.floor((mazeSize - #(t[1]) )*0.5)),
				math.random(0,math.floor( ( mazeSize-#t)*0.5 ) )
		else
			tx,ty=math.random(2-#(t[1]) ,math.floor( (mazeSize - 1 )*0.5)),
				math.random(2-#(t[1]), math.floor( (mazeSize-1)*0.5 ) )
		end
		local ok=attachTile(1+2*tx,1+2*ty,t,true)
	end
	]]--
	local sorted={}
	local index,prev=1,0

	for _,t in ipairs(tiles) do -- строим список шаблонов по размеру
		local tsize=#(tiles[1]) -- определяем размер шаблона так
		t.size=tsize
		while prev<tsize do -- едем вверх от текущего по увеличению
			if index==1 then break end
			prev=sorted[index].size
			index=index-1
		end

		ti(sorted,t,index)
	end

end
local function init()
	map={}
	mzObjects={}

	tiles={
		[1]={tile=mazeTile},
		[-1]={tile=mazeTile2},
		}
	for i=1,mazeSize do
		local row={}
		for j=1,mazeSize do
			row[j]=0
		end
		map[i]=row
	end
	-- 1. заполняем клетками
	for j=2,#map-1,2 do
		local row=map[j]
		for i=1,#row,2 do
			row[i]=1
		end
	end
	for j=1,#map,2 do
		local row=map[j]
		for i=2,#row-1,2 do
			row[i]=1
		end
	end
	for j=1,#map,2 do
		local row=map[j]
		for i=1,#row,2 do
			row[i]=-1
		end
	end
	for j=2,#map-1,2 do
		local row=map[j]
		for i=2,#row-1,2 do
			row[i]='.'
		end
	end
end


local function writeOut()
	local s=' '
	for j=1,#map[1] do
		s=s..'-'
	end
	s=s..' \n'
	for i,row in ipairs(map) do
		s=s..'|'
		for _,j in ipairs(row) do
			s=s..j
		end
		s=s..'|\n'
	end
	s=s..' '
	for j=1,#map[1] do
		s=s..'-'
	end
	s=s..' \n'
	print(s)
end
local function writeToMap(x0,y0)
	local tile
	local calc={
		[" # #"]=0,
		["   #"]=0,
		[" #  "]=0,
		["# # "]=1,
		["  # "]=1,
		["#   "]=1,
		["    "]=1,
		["####"]=2,
		["## #"]=3,
		["### "]=4,
		[" ###"]=5,
		["# ##"]=6,
		["##  "]=7,
		[" ## "]=8,
		["  ##"]=9,
		["#  #"]=10,
		}
	local calcWall=function(c,t,r,b,l)
		if c==0 or c=='.' then return -1 end

		if t=='.' or t==0 then t=' ' else t='#' end
		if r=='.' or r==0 then r=' ' else r='#' end
		if b=='.' or b==0 then b=' ' else b='#' end
		if l=='.' or l==0 then l=' ' else l='#' end

		t=t..r..b..l
		c=calc[t]
		if c~=nil then return c end
		print('wall lost '..t)
		return 2
	end
	x0=mazeX*23
	y0=mazeY*23
	local x,y
	for j=1,#map do
		local row=map[j]
		x=x0+23*j
		y=y0+23*j
		for i=1,#row do
			local t=calcWall(row[i],mzGet(i,j-1),mzGet(i+1,j),mzGet(i,j+1),mzGet(i-1,j))
			if t>=0 then
				local v=tiles[ row[i] ]
				if v~=nil and v.tile then --загружаем шаблонную стену
					tile=v.tile
				else
					tile=mazeTile
				end

				if mapSet(tile,x,y,t)==nil then
					print ('fail',x,' ',y,' ',t)
				end
			else
				mapDel(x,y)
			end
			x=x+23
			y=y-23
		end
	end
	for _,i in ipairs(mzObjects) do
		local x,y=i[1],i[2]
		local o=createObject(i[3],mzToGlobal(x,y))
		if type(i[4])=='table' then
			for _,j in pairs(v.have) do
				if j.type then
					local k=createObjectIn(j.type,o)
					--- TODO: добавить новых свойств для объекта в чемодане
				end
			end
		end
	end
end
local function attachTile(x0,y0,t,ignore)
	local x,y
	local walls={}
	local toItem=function (s,i)
		local c=string.sub(s,i,i)
		local r
		if type(c)~='string' or #c~=1 then
			r= -1
		else
			if c=='.' then -- путь для лабиринта
				return c
			elseif c==' ' then -- пространство
				return 0
			elseif c=='_' then -- ничего не делаем
				return nil
			elseif nil==walls[c] then -- запасаем новую величину
				local v=t.walls[c]
				if (type(v)~='table') then
					error('wrong template: '..c)
				end
				local k
				if c>='a' and c<='z' then
					k=tilePlus
					tilePlus=tilePlus+1
				elseif c>='A' and c<='Z' then
					k=tileMinus
					tileMinus=tileMinus-1
				end
				tiles[k]=v
				walls[c]=k
			end
			r=walls[c]
		end
		return r
	end
	local ok=true
	local back={}
	for dy=1,#t do
		y=y0+dy-1
		if y>mazeSize then break end
		if y>0 then
			local row=t[dy]
			back[dy]={}
			for dx=1,#row do
				x=x0+dx-1
				if x>mazeSize then break end
				if x>0 then
					local l=map[y][x]
					local r=toItem(row,dx)
					if r~=nil then
						if (l~=r and l~='.' and not ignore) or  -- ща всегда игнор
							(type(l)=='number' and (l<-1 or l>1) ) -- другой шаблон
							then
								y=10000
								ok=false
								break
						end
						map[y][x]=r
					end
					back[dy][dx]=l
				end
			end
		end
		if not ok then break end
	end

	if ok then
		if type(t.objects)=='table' then -- создадим  предметы из шаблона
			for q,v in pairs(t.objects) do

				ti(mzObjects,{x0+tonumber(v.x),y0+tonumber(v.y),v.type,v.have})
			end
		end
	else --- если у нас была ошибка - откручиваем все назад
		for dy=1,#back do
			y=y0+dy-1
			if y>mazeSize then break end
			if y>0 then
				local row=back[dy]
				for dx=1,#row do
					x=x0+dx-1
					if x>mazeSize then break end
					if x>0 then
						map[y][x]=row[dx]
					end
				end
			end
		end
	end
	return ok
end
function makeMaze(x,y)
--[[
	используем алгоритм DFS
   1. Start at a particular cell and call it the "exit."
   2. Mark the current cell as visited, and get a list of its neighbors. For each neighbor, starting with a randomly selected neighbor:
         1. If that neighbor hasn't been visited, remove the wall between this cell and that neighbor, and then recurse with that neighbor as the current cell.
]]--
	-- 0. коррекция координат, инициализация
	if math.mod(x,2)~=0 then x=x+1 end
	if math.mod(y,2)~=0 then y=y+1 end
	local ti=table.insert
	local tr=table.remove
	local cmp=function(a,b) return (a[1]==b[1]) and (a[2]==b[2]) end

	-- 3. начинаем перебирать стены
	local wallList={}
	local markRoom=function(x,y)
		-- отмечает комнату добавляет новые стены и
		-- возвращает список стен, которые необходимо удалить
		local walls={}
		local remove={}
		local roomSize=0 -- размер территории комнаты
		local blockedList={} -- сюда падают стены, ведущие в соседнюю комнату,
			-- которые алгоритм не собирается ломать
		local x0,y0=x,y
		local points={ {x,y} }
		while #points>0 do --ищем всех соседей
			local t=points[1]
			roomSize=roomSize+1
			tr(points,1)
			x,y=t[1],t[2]
			t=mzGet(x,y)
			if t=='.' then
				map[y][x]=0
				y=y-1
				t=mzGet(x,y)
				if t=='.' then
					ti(points,{x,y})
				elseif t>0 then
					local v=mzGet(x,y-1)
					if v=='.' then
						ti(walls,{x,y,3})
					else
						if v==0 then ti(blockedList,{x,y,3}) end
						ti(remove,{x,y,3})
					end
				end
				y=y+2
				t=mzGet(x,y)
				if t=='.' then
					ti(points,{x,y})
				elseif t>0 then
					local v=mzGet(x,y+1)
					if v=='.' then
						ti(walls,{x,y,1})
					else
						if v==0 then ti(blockedList,{x,y,1}) end
						ti(remove,{x,y,1})
					end
				end
				y=y-1 x=x-1
				t=mzGet(x,y)
				if t=='.' then
					ti(points,{x,y})
				elseif t>0 then
					local v=mzGet(x-1,y)
					if v=='.' then
						ti(walls,{x,y,2})
					else
						if v==0 then ti(blockedList,{x,y,2}) end
						ti(remove,{x,y,2})
					end
				end
				x=x+2
				t=mzGet(x,y)
				if t=='.' then
					ti(points,{x,y})
				elseif t>0 then
					local v=mzGet(x+1,y)
					if v=='.' then
						ti(walls,{x,y,4})
					else
						if v==0 then ti(blockedList,{x,y,4}) end
						ti(remove,{x,y,4})
					end
				end
			end
		end
		for i,j in ipairs(remove) do
			for q,v in ipairs(wallList) do --удаляем исчезнувшие стены
				if cmp(v,j) then
					remove[i]=q
				end
			end
		end
		for _,v in ipairs(walls) do
			ti(wallList,v)
		end
		return remove
	end
	--------------
	markRoom(x,y)
	while #wallList>0 do
		local r=math.random(#wallList)
		local t=wallList[r]
		local d=t[3]
		tr(wallList,r)
		x,y=t[1],t[2]
		local tx,ty=x,y
		if d==1 then --в зависимости от направления удаляем цель
			y=y+1
		elseif d==2 then
			x=x-1
		elseif d==3 then
			y=y-1
		elseif d==4 then
			x=x+1
		end
		local tile=mzGet(x,y)
		if tile=='.' then -- при правильной работе алгоритма эта проверка не нужна
			local remove=markRoom(x,y)
			for _,q in ipairs(remove) do
				if type(q)=='number' then -- вычистим ошметки
					tr(wallList,q)
				end
			end
			map[ty][tx]=0 -- удаляем стену
		end
	end

end
function mazeGenerate(tiles)
	-- 1. Набиваем пустых комнат
	init()
	local x,y=2,2
	-- 2. Помещаем шаблоны
	placeTemplates(tiles)
	-- 3. Запускаем генератор
	makeMaze(x,y)
	-- 4.

	map[mazeSize][math.floor(mazeSize/2)]=0
	map[1][math.floor(mazeSize/2)]=0
	map[math.floor(mazeSize/2)][1]=0
	map[math.floor(mazeSize/2)][mazeSize]=0
	writeToMap()
	--writeOut()
end
do
	local a,b=loadstring('return '..mazeTemplates)()
	if not a then print(b) end
end
maze=function ()	mazeGenerate( loadstring('return {'..mazeTemplates..'}')() ) end
