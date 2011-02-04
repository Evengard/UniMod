local dlg
local tileButton
local textCoords
local textTile
local buttVari,textA,textB
local listVari
local listOthers -- список всех тайлов
local listOthersSel=-1 -- выбраная строчка
local listStore -- список сохраненных
local tileStoreSel=0 -- выбранный элемент списка
local tileStore={}
local tileMode
--[[
1 - режим выбора одного тайла
2 - режим рисования с бордюром


--]]
local list

local format=string.format
local parseR

function tileSel()
	if me==nil then setMe() end
	local a,b=playerMouse(me)
	local r=tileGet(a,b)
	if dlg~=nil then
		wndSetText(textCoords,format('(%d,%d)',math.floor(a/23),math.floor(b/23)))
		if #r>0 then
			listOthersSel=0
		else
			listOthersSel=-1
		end
		parseR(r)
	end
end

parseR=function (r)
	wndLbClear(listOthers)
	if r~=nil and r[1]~=nil then
		dlg.data=r -- задаем параметры
		wndSetText(textTile,tileGetName(r[1])..'('..r[1]..')')
		if r[2]==-1 then
			wndButtonSetText(buttVari,'RND')
		else
			wndButtonSetText(buttVari,r[2])
		end
		wndSetText(textA,r[3])
		wndSetText(textB,r[4])

		local i=1
		while r[i]~=nil do
			local s=format('%d %d (%d,%d)',r[i],r[i+1],r[i+2],r[i+3])
			wndLbAddText(listOthers,s)
			i=i+4
		end
	else
		listOthersSel=-1
		wndSetText(textTile,' ')
		wndButtonSetText(buttVari,'RND')
		wndSetText(textA,'0')
		wndSetText(textB,'0')
		dlg.data=nil
	end
end
local function tilePaint(dst)
	if me==nil then setMe() end
	local x,y=playerMouse(me)
	-- надо какое-то уточнение координат
	if dst==nil then tileSet(x,y,{}) return end -- просто удаляем
	local src=tileGet(x,y)
	local srcLen=#src
	local dstLen=#dst
	local t={dst[1],dst[2],0,0}
	if  t[2]==-1 then
		t[2] = rnd(0,tileMaxVari(t[1])-1 )
	end

	if src==nil or src[1]~=dst[1] or srcLen~=dstLen then
		tileSet(x,y,t)
	end

	local tr=table.remove
	local ti=table.insert
--[[
как выглядят варианты "B"

0  1  2  3  4
5			6
7			8
9			10
11 12 13 14 15


удаляем все ТАКИЕ ЖЕ дургого цвета И добавляем своих если еще не надо
]]
	local items=
	{
		{ 0,-2,0,	{0}, },			--верхний левый
		{ 1,-1,1,	{1,2,3}, },		--верхний
		{ 2,0,4,	{4}, },			--верхний правый
		{ -1,-1,5,	{5,7,9}, },		--левый
		{ 1,1,6,	{6,8,10}, },	--правый
		{ -2,0,11,	{11}, },		--нижний левый
		{ -1,1,12,	{12,13,14}, },	--нижний
		{ 0,2,15,	{15}, },		--нижний правый

	}
	local px,py=x,y
	local dt=1
	for _,k in ipairs(items) do -- проверяем соседей
		x,y=px+23*k[1],py+23*k[2]
		if x>=0 and x<=255*23 and y>=0 and y<=255*23 then
			local src=tileGet(x,y) -- не строим объединение с пустотой
			if src~=nil and src[1]~=dst[1] then -- с одинаковыми не работаем
				local srcLen=#src
				local i=1
				local needAdd=true
				while src[i]~=nil do
					if src[i+2]>0 then --есть смешение
						if src[i+3]==k[3] then -- удаляем ТАКОЙ ЖЕ УГОЛ
							tr(src,i);tr(src,i);tr(src,i);tr(src,i);tr(src,i);
							i=i-4
							--print ('removed at:',k[3])
						else
							for _,v in ipairs(k[4]) do
								if src[i]==dst[1] and src[i+3]==v then -- такой же тайл, заменяемый угол
									needAdd=false
									break;
								end
							end
						end
					end
					i=i+4
				end
				if needAdd then
					ti(src,dst[1])
					if dst[2]==-1 then
						ti(src,math.random(0,tileMaxVari(dst[1])-1))
					else
						ti(src,dst[2])
					end
					ti(src,dst[3]) -- тип тайла
					ti(src,k[3])
				end
				local a,b=x,y
				setTimeout(function () tileSet(a,b,src) end,dt)
				dt=dt+1
			end
		end
	end
end
local function updateStore()
	if dlg==nil then return end
	wndLbClear(listStore)
	for q,v in ipairs(tileStore) do
		local s=''
		local i=1
		if v[i]~=nil then
			s=s..format("%3d",v[1])
			i=i+4
		end
		while v[i]~=nil do
			s=s..format(" - %3d",v[i])
			i=i+4
		end
		if s~='' then
			wndLbAddText(listStore,s)
		end
	end
end
function tileDlg()
	if dlg~=nil then return end
	local width=175
	local y=10
	local ti=table.insert
	local t
	dlg={
		x=200,y=200,w=width,h=100,
		bgcolor='#303030';
		}
	t={
			type="PUSHBUTTON",
			status="ENABLED+IMAGE",
			style="MOUSETRACK",
			x=(10),y=(y),w=45,h=25,
			offsetX=0,offsetY=-1,
			selectedImage="UIButtonSmLit";
			hiliteImage="UIButtonSmLit";
			image="UIButtonSm";
			disabledImage="UIButtonSmDis";
			textColor='#E6A541';
			text="Apply";
			tooltip="Применить";
			onClick=function(a,b)
				local s=wndGetText(textCoords)
				local diag=true
				local i,_,x1,y1,x2,y2=string.find(s,'%((%d+)%s*[,;]?%s*(%d+)%s*%:%s*(%d+)%s*[,;]?%s*(%d+)%)')
				if i==nil then
					diag=false
					i,_,x1,y1,x2,y2=string.find(s,'%((%d+)%s*[,;]?%s*(%d+)%)%s*%-%s*%((%d+)%s*[,;]?%s*(%d+)%)')
				end
				if i==nil then
					i,_,x1,y1=string.find(s,'(%d+)%s*[,;]?%s*(%d+)')
					x2=x1;y2=y1
				end
				if x1==nil or y1==nil or x2==nil or y2==nil then print 'invalid coords' return end
				x1=tonumber(x1);x2=tonumber(x2);y1=tonumber(y1);y2=tonumber(y2)
				if x1<=0 or y1<=0 or x1>256 or y1>256 or
					x2<0 or y2<0 or x2>256 or y2>256 then
					print (format('(%d,%d) is not valid coords',x,y))
					return
				end

				if b.data==nil then
					b.data={}
				end
				local i=1
				local r=b.data
				local t={}
				local rnd=math.random
				local transpose
				if diag then
					transpose=function(x,y)
						return (x1+x+y)*23,(y1+x-y)*23
					end
				else
					if x2<x1 then x1,x2=x2,x1 end
					if y2<y1 then y1,y2=y2,y1 end
					x2=x2-x1
					y2=y2-y1
					transpose=function(x,y)
						return (x1+x)*23,(y1+y)*23
					end
				end
				for y=0,y2 do
					for x=0,x2 do
						local a,b=transpose(x,y)
						setTimeout(function()
							local i=1
							while r[i]~=nil do
								t[i]=r[i]
								if  r[i+1]==-1 then
									t[i+1] = rnd(0,tileMaxVari(t[i])-1 )
								else
									t[i+1] =r[i+1]
								end
								t[i+2]=r[i+2]
								t[i+3]=r[i+3]
								
								i=i+4
							end
							t[i]=nil
							--print (,table.concat(t,' '))
							tileSet(a,b,t)
							end,i)
						i=i+1
					end
				end
			end,
		}
	ti(dlg,t)
	t={
			type="PUSHBUTTON",
			status="ENABLED+IMAGE",
			style="MOUSETRACK",
			x=(10+45+10),y=(y),w=45,h=25,
			offsetX=0,offsetY=-1,
			selectedImage="UIButtonSmLit";
			hiliteImage="UIButtonSmLit";
			image="UIButtonSm";
			disabledImage="UIButtonSmDis";
			textColor='#E6A541';
			text="Save";
			tooltip="Сохранить изменения";
			onClick=function(a,b)
				local s='_test'
				mapSave(s)
				print('map saved as '..s)
			end
		}
	ti(dlg,t)
	t= {
			type="PUSHBUTTON",
			status="ENABLED+IMAGE",
			style="MOUSETRACK",
			x=(10+45+10+45+10),y=(y),w=45,h=25,
			offsetX=0,offsetY=-1,
			selectedImage="UIButtonSmLit";
			hiliteImage="UIButtonSmLit";
			image="UIButtonSm";
			disabledImage="UIButtonSmDis";
			textColor='#E6A541';
			text="Close";
			tooltip="Закрыть окно нафиг";
			onClick=function(a,b) wndClose(b) dlg=nil end,
		}
	y=y + t.h +10
	ti(dlg,t)
	textCoords={
			type="ENTRYFIELD",
			status="ENABLED+ONE_LINE",
			style="MOUSETRACK",
			x=10,y=(y),w=130,h=15,
			textColor="#007F7F";
			hiliteColor="#007F7F";
			selectedColor="#3F007F";
			text="pos:";
			maxLen=30;
			tooltip="координаты; (x1,y1) - (x2,y2) для прямоугольника";
			onFocus=function(a,b,c)
				if c==0 then
					local valid=false
					while 1 do
						local s=wndGetText(textCoords)
						local i,_,x1,y1,x2,y2=string.find(s,'%((%d+)%s*[,;]?%s*(%d+)%)%s*%-%s*%((%d+)%s*[,;]?%s*(%d+)%)')
						if i==nil then
							i,_,x1,y1=string.find(s,'(%d+)%s*[,;]?%s*(%d+)')
						end
						x2=x1;y2=y1
						if x1==nil or y1==nil or x2==nil or y2==nil then break end
						x1=tonumber(x1);x2=tonumber(x2);y1=tonumber(y1);y2=tonumber(y2)
						if x1<=0 or y1<=0 or x1>=256 or y1>256 or
							x2<=0 or y2<=0 or x2>=256 or y2>256 then
							break
						end
						valid=true
						break
					end
					if valid then
						wndSetAttr(a,'textColor',a.textColor)
					else
						wndSetAttr(a,'textColor','#7F003F')
					end
				end
			end,

		}
	ti(dlg,textCoords)
	t={
			type="PUSHBUTTON",
			status="ENABLED",
			style="MOUSETRACK",
			x=(10+130+5),y=(y+3),w=15,h=15,
			offsetX=0,offsetY=-1,
			selectedColor="#303030";
			hiliteColor="#104040";
			bgcolor="#101010";
			textColor='#E6A541';
			textColor='#E6A541';
			text="P";
			tooltip="Выбрать точку";
			onClick=function(me,parent)
				tileMode=1
				wndGrabMouse(parent)
			end,
		}
	ti(dlg,t)

	t={
			type="PUSHBUTTON",
			status="ENABLED",
			style="MOUSETRACK",
			x=(10+130+5+15+5),y=(y+3),w=15,h=15,
			offsetX=0,offsetY=-1,
			selectedColor="#303030";
			hiliteColor="#104040";
			bgcolor="#101010";
			textColor='#E6A541';
			text="D";
			tooltip="Рисовать";
			onClick=function(me,parent)
				if tileMode==2 then
					wndUnGrabMouse(parent)
					tileMode=nil
					return
				end
				local r=parent.data
				if #r>4 then
					print "No more than one tile should be selected"
				elseif r[3]==0 then
					print "Blending parameter A should not be zero"
				else
					tileMode=2
					wndGrabMouse(parent)
				end
			end,
		}
	ti(dlg,t)

	y=y+t.h+10+10
	textTile={
			type="ENTRYFIELD",
			status="ENABLED",
			style="MOUSETRACK",
			x=10,y=(y),w=(width-20),h=15,
			textColor="#007F7F";
			hiliteColor="#007F7F";
			selectedColor="#3F007F";
			text='tile:';
			maxLen=40;
			onFocus=function(a,b,c)
				if c==0 then
					local r=b.data
					if r==nil or listOthersSel==-1 then return end
					local i=listOthersSel*4 + 1
					local out=tonumber(wndGetText(a))
					if out==nil then
						print ('use Tile number please')
					else
						r[i]=out
						local s=tileGetName(out)..'('..out..')'
						wndSetText(a,s )
						parseR(dlg.data)
					end
				end
			end;

		}
	y=y + textTile.h + 10
	ti(dlg,textTile)
	buttVari= {
			type="PUSHBUTTON",
			status="ENABLED+IMAGE+BELOW",
			style="MOUSETRACK",
			x=(10),y=(y),w=45,h=25,
			offsetX=0,offsetY=-1,
			selectedImage="UIButtonSmLit";
			hiliteImage="UIButtonSmLit";
			image="UIButtonSm";
			disabledImage="UIButtonSmDis";
			textColor='#E6A541';
			text="RND";
			tooltip="Вариация тайла";
			onClick=function(me,b)
				wndLbClear(listVari)
				wndLbAddText(listVari,'RND')
				if b.data~=nil and b.data[1]~=nil then
					for i=0,tileMaxVari(b.data[1])-1 do
						wndLbAddText(listVari,i)
					end
				end
				wndShow(listVari,true)
				wndShow(me,false)
			end;
		}
	ti(dlg,buttVari)
	listVari=
		{
			type="SCROLLLISTBOX",
			status="ENABLED+HIDDEN+ABOVE",
			style="MOUSETRACK+NOFOCUS",
			x=10,y=(y),w=50,h=95,
			bgcolor="#101010";
			hiliteColor="TRANSPARENT";
			disabledColor="TRANSPARENT";
			enabledColor="TRANSPARENT";
			textColor="#007F7F";
			selectedColor="#000096";
			maxLines=20;
			-- data4 = 1 - может значит автозакрытие?
			onSelChange=function (me,b,idx)
				if idx<1 then
					idx=-1
					wndButtonSetText(buttVari,'RND')
				else
					idx = idx - 1
					wndButtonSetText(buttVari, idx )
				end

				wndShow(me,false)
				wndShow(buttVari,true)
				local r=b.data
				if r==nil or listOthersSel==-1 then return end
				local i=listOthersSel*4 + 1
				if r[i]~=nil then
					r[i+1]=idx
				end
				parseR(dlg.data)
			end
		}
	ti(dlg,listVari)
	textA={
			type="ENTRYFIELD",
			status="ENABLED+ONE_LINE",
			style="MOUSETRACK",
			x=(10+45+10),y=(y),w=45,h=15,
			textColor='#E6A541';
			textColor='#E6A541';
			selectedColor="#3F007F";
			text="A:";
			maxLen=4;
			onlyNumbers=true;
			onFocus=function(a,b,c)
				if c==0 then
					local r=b.data
					if r==nil or listOthersSel==-1 then return end
					local i=listOthersSel*4 + 1
					local out=tonumber(wndGetText(a))
					if out~=nil then
						r[i+2]=out
					end
					parseR(dlg.data)
				end
			end;
		}
	ti(dlg,textA)
	textB={
			type="ENTRYFIELD",
			status="ENABLED+ONE_LINE",
			style="MOUSETRACK",
			x=(10+45+10+45+10),y=(y),w=45,h=15,
			textColor='#E6A541';
			textColor='#E6A541';
			selectedColor="#3F007F";
			text="B:";
			maxLen=4;
			onlyNumbers=true;
			onFocus=function(a,b,c)
				if c==0 then
					local r=b.data
					if r==nil or listOthersSel==-1 then return end
					local i=listOthersSel*4 + 1
					local out=tonumber(wndGetText(a))
					if out~=nil then
						r[i+3]=out
					end
					parseR(dlg.data)
				end
			end;
		}
	ti(dlg,textB)
	y=y+textB.h+10
	t={
			type="PUSHBUTTON",
			status="ENABLED",
			style="MOUSETRACK",
			x=(10),y=(y+5),w=15,h=15,
			offsetX=0,offsetY=-1,
			selectedColor="#303030";
			hiliteColor="#104040";
			bgcolor="#101010";
			textColor='#E6A541';
			text="+";
			tooltip="Добавить строчку";
			onClick=function(me,parent)
				local r=parent.data
				if r==nil then
					parent.data={0,0,0,0}
				else
					ti(r,0)
					ti(r,0)
					ti(r,0)
					ti(r,0)
				end
				if listOthersSel==-1 then listOthersSel=0 end
				wndLbAddText(listOthers,'0 0 (0,0)')
			end,
		}
	ti(dlg,t)
	y=y+t.h+10+10
	listOthers={
			type="SCROLLLISTBOX",
			status="ENABLED+ONE_LINE",
			style="MOUSETRACK",
			x=10,y=(y),w=(width-20),h=40,
			hiliteColor="TRANSPARENT";
			disabledColor="TRANSPARENT";
			enabledColor="TRANSPARENT";
			textColor="#007F7F";
			selectedColor="#004F3F";
			maxLines=10;
			onSelChange=function (me,parent,idx)
				local r=parent.data
				local i=idx*4+1
				listOthersSel=idx
				if r==nil or idx==-1 or r[i]==nil then
					wndSetText(textTile,"")
					wndButtonSetText(buttVari,"RND")
				else
					wndSetText(textTile,tileGetName(r[i])..'('..r[i]..')')
					wndButtonSetText(buttVari,r[i+1])
					wndSetText(textA,r[i+2])
					wndSetText(textB,r[i+3])
				end
			end
		}
	ti(dlg,listOthers)
	y=y+listOthers.h+10
	t={
			type="PUSHBUTTON",
			status="ENABLED+IMAGE",
			style="MOUSETRACK",
			x=(10),y=(y),w=45,h=25,
			offsetX=0,offsetY=-1,
			selectedImage="UIButtonSmLit";
			hiliteImage="UIButtonSmLit";
			image="UIButtonSm";
			disabledImage="UIButtonSmDis";
			textColor='#E6A541';
			text="Store";
			tooltip="Записать в лист";
			onClick=function(a,b)
				print(tostring(b.data),' ',b.data[1],json.encode(tileStore))
				if b.data==nil or b.data[1]==nil then return end
				ti(tileStore,b.data)
				updateStore()
			end,
		}
	ti(dlg,t)
	t={
			type="PUSHBUTTON",
			status="ENABLED+IMAGE",
			style="MOUSETRACK",
			x=(10+45+10),y=(y),w=45,h=25,
			offsetX=0,offsetY=-1,
			selectedImage="UIButtonSmLit";
			hiliteImage="UIButtonSmLit";
			image="UIButtonSm";
			disabledImage="UIButtonSmDis";
			textColor='#E6A541';
			text="Remove";
			tooltip="Удалить из списка";
			onClick=function(a,b)
				if tileStoreSel<=0 then return end
				table.remove(tileStore,tileStoreSel)
				tileStoreSel=0
				updateStore()
			end,
		}
	ti(dlg,t)
	y=y + t.h + 10
	listStore=
		{
			type="SCROLLLISTBOX",
			status="ENABLED+ONE_LINE",
			style="MOUSETRACK",
			x=10,y=(y),w=(width-20),h=65,
			hiliteColor="TRANSPARENT";
			disabledColor="TRANSPARENT";
			enabledColor="TRANSPARENT";
			textColor="#007F7F";
			selectedColor="#004F3F";
			maxLines=10;
			onSelChange=function (me,parent,idx)
				tileStoreSel=idx+1
				if tileStoreSel>0 then
					local r=tileStore[tileStoreSel]
					if #r>0 then
						listOthersSel=0
					else
						listOthersSel=-1
					end
					parseR(r)
				end
			end;
		}
	y=y + listStore.h + 10
	ti(dlg,listStore)
	dlg.h=y;
	dlg.onGrabMouse=function (me)
		if tileMode~=nil and tileMode==2 then
			local a,b=pcall(tilePaint,me.data)
			if not a then print(b) 
			else
				wndGrabMouse(me)
			end
		else
			tileMode=nil
			local a,b=pcall(tileSel)-- если кто-то куда-то кликнул
			if not a then print(b) end
		end
	end

	wndCreate(dlg)
	updateStore()
end
local resizeButton
local recreate
resizeButton=function ()
	local x,y=wndScreenSize()
	if uniPanel==nil then
		if recreate==true then return end
		recreate=true
		uniPanel=
		{
			bgcolor='#303030';
			y=0,w=20,h=40,
			{
				type="PUSHBUTTON",
				x=0,y=0,w=20,h=20,
				bgcolor='#301030';
				status="ENABLED";
				onClick=function(me,b,x,y)
					tileDlg()
				end
			};
			{
				type="PUSHBUTTON",
				x=0,y=20,w=20,h=20,
				bgcolor='#103030';
				status="ENABLED";
				onClick=function(me,b,x,y)
					formGame('_test')
				end
			};
		}
		uniPanel.x= x - uniPanel.w
		wndCreate(uniPanel)
	end
	setTimeout(resizeButton,150)
end
setTimeout(resizeButton,1)
