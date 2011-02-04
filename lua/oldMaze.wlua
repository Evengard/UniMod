require( "iuplua" )
function loadTemplates()
	return [[
{
'OO###OO',
'#.....O',
'O.....O',
'O.....O',
'O.....#',
'O.....O',
'OO###OO',
name='/'
};
	]]
end

local size=11
local map
local tilesCount
function init()
	map={}
	tilesCount=0
	for i=1,size do
		local row={}
		for j=1,size do
			row[j]=' '
		end
		map[i]=row
	end
end
function writeOut()
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
function attachTile(x0,y0,t,ignore)
	local x,y
	local toItem=function (s,i)
		local t=string.sub(s,i,i)
		if type(t)~='string' or #t~=1 then
			print ('!'..x..','..y..' i='..i..' '..tostring(s)..'|'..type(t))
		end
		return t
	end
	local ok=true
	local back={}
	for dy=1,#t do
		y=y0+dy-1
		if y>size then break end
		if y>0 then
			local row=t[dy]
			back[dy]={}
			for dx=1,#row do
				x=x0+dx-1
				if x>size then break end
				if x>0 then
					local l=map[y][x]
					local r=toItem(row,dx)
					if l~=r and l~=' ' and not ignore then
						y=10000
						ok=false
						break
					end
					map[y][x]=r
					back[dy][dx]=l
				end
			end
		end
		if not ok then break end
	end
	--- если у нас была ошибка - откручиваем все назад
	if not ok then
		print ('not ok')
		for dy=1,#back do
			y=y0+dy-1
			if y>size then break end
			if y>0 then
				local row=back[dy]
				for dx=1,#row do
					x=x0+dx-1
					if x>size then break end
					if x>0 then
						map[y][x]=row[dx]
					end
				end
			end
		end
	end
end
function printPt(t,x,y)
	print(string.format('(%d,%d)=%s',x,y,t))
end
function makeMaze(x,y)
--[[
	используем алгоритм DFS
   1. Start at a particular cell and call it the "exit."
   2. Mark the current cell as visited, and get a list of its neighbors. For each neighbor, starting with a randomly selected neighbor:
         1. If that neighbor hasn't been visited, remove the wall between this cell and that neighbor, and then recurse with that neighbor as the current cell.
]]--
--[[
	—имволы элементов:
	(пробел) - уже посещенна€ пуста€ территори€
	. - еще не посещенна€ территори€
	# - стена которую можно убрать
	O - стена которую убрать нельз€

]]--
	-- 0. коррекци€ координат, инициализаци€
	if math.mod(x,2)~=0 then x=x+1 end
	if math.mod(y,2)~=0 then y=y+1 end
	local ti=table.insert
	local tr=table.remove
	local cmp=function(a,b) return (a[1]==b[1]) and (a[2]==b[2]) end
	local mapGet=function(x,y) if y<1 or x<1 or x>size or y>size then return '!!' end return map[y][x] end
	-- 1. заполн€ем клетками
	for j=2,#map-1,2 do
		local row=map[j]
		for i=1,#row,2 do
			row[i]='#'
		end
	end
	for j=1,#map,2 do
		local row=map[j]
		for i=2,#row-1,2 do
			row[i]='#'
		end
	end
	for j=1,#map,2 do
		local row=map[j]
		for i=1,#row,2 do
			row[i]='O'
		end
	end
	for j=2,#map-1,2 do
		local row=map[j]
		for i=2,#row-1,2 do
			row[i]='.'
		end
	end
	-- 2. ѕомещаем шаблон
	attachTile(3,3,loadstring('return '..loadTemplates())(),true )
	-- 3. начинаем перебирать стены
	local wallList={}
	local cnt=1
	local markRoom=function(x,y) 
		-- отмечает комнату добавл€ет новые стены и
		-- возвращает список стен, которые необходимо удалить
		local walls={}
		local remove={}
		local points={ {x,y} }
		print ('--- '..cnt) cnt=cnt+1
		while #points>0 do --ищем всех соседей
			local t=points[1]
			tr(points,1)
			x,y=t[1],t[2]
			t=mapGet(x,y)
			if t=='.' then 
				print ('='..t)
				map[y][x]=' '
				y=y-1
				t=mapGet(x,y)
				if t=='#' then
					if mapGet(x,y-1)=='.' then
						ti(walls,{x,y,3})
					else
						ti(remove,{x,y,3})
					end
				elseif t=='.' then
					ti(points,{x,y})
				end
				y=y+2
				t=mapGet(x,y)
				if t=='#' then
					if mapGet(x,y+1)=='.' then
						ti(walls,{x,y,1})
					else
						ti(remove,{x,y,1})
					end
				elseif t=='.' then
					ti(points,{x,y})
				end
				y=y-1 x=x-1
				t=mapGet(x,y)
				if t=='#' then
					if mapGet(x-1,y)=='.' then
						ti(walls,{x,y,2})
					else
						ti(remove,{x,y,2})
					end
				elseif t=='.' then
					ti(points,{x,y})
				end
				x=x+2
				t=mapGet(x,y)
				if t=='#' then
					if mapGet(x+1,y)=='.' then
						ti(walls,{x,y,4})
					else
						ti(remove,{x,y,4})
					end
				elseif t=='.' then
					ti(points,{x,y})
				end
			end
		end
		for i,j in ipairs(remove) do
			for q,v in ipairs(wallList) do --удал€ем исчезнувшие стены
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
	map[y][x]=' '
	if mapGet(x,y-2)=='.' then -- стартовые стены
		ti(wallList,{x,y-1,3})
	end
	if  mapGet(x,y+2)=='.' then
		ti(wallList,{x,y+1,1})
	end
	if  mapGet(x-2,y)=='.' then
		ti(wallList,{x,y-1,2})
	end
	if  mapGet(x+2,y)=='.' then
		ti(wallList,{x+1,y,4})
	end
	while #wallList>0 do
		local r=math.random(#wallList)
		local t=wallList[r]
		local d=t[3]
		tr(wallList,r)
		x,y=t[1],t[2]
		local tx,ty=x,y
		if d==1 then --в зависимости от направлени€ удал€ем цель
			y=y+1
		elseif d==2 then
			x=x-1
		elseif d==3 then
			y=y-1
		elseif d==4 then
			x=x+1
		end
		local tile=mapGet(x,y)
		if tile=='.' then -- при правильной работе алгоритма эта проверка не нужна
			local remove=markRoom(x,y)
			for _,q in ipairs(remove) do
				if type(q)=='number' then -- вычистим ошметки
					tr(wallList,q)
				end
			end
			map[ty][tx]=' ' -- удал€ем стену
		end
--		coroutine.yield()
	end

end
local rout
function generate(tiles)
	math.randomseed( os.time() )
	init()
	local x,y=1,1
--	attachTile(7,8,tiles[1])
--	attachTile(6,5,tiles[1])
	makeMaze(x,y)
	writeOut()
end

generate( loadstring('return {'..loadTemplates()..'}') )


do
local canvas=iup.multiline{expand='yes';font='Fixedsys'}
local button=iup.button{title="Generate"}
local text=iup.multiline{expand='yes';value=loadTemplates();font='Fixedsys'}

function button:action ()
    local p=print
    local v=''
    print=function(x) v=v..tostring(x)..'\n' end
	local a,b=loadstring('return {'..text.value..'}')
	if not a then
		print(b)
	else
		local tiles=a()
		generate(tiles)
	end
    print=p
    canvas.value=v
    return iup.DEFAULT
end

local dlg = iup.dialog{
		iup.hbox{
--			size="200x200";
			EXPANDCHILDREN='BOTH';
			EXPAND='YES';
--			NORMALIZESIZE ='BOTH';
			canvas,
			iup.vbox{
				EXPANDCHILDREN='BOTH';

				text,
				button;
				},
		};
	EXPAND='BOTH';
	title="Testing",size="HALFxHALF"}

dlg:show()

iup.MainLoop()

end
