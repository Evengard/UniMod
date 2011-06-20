
local mapNearFlag={0,0, --для определния направления
			4,0,
			16,0,
			20,0,
			10,1,
			2,1,
			8,1,
			30,2,
			22,3,
			14,4,
			28,5,
			26,6,
			6,7,
			12,8,
			24,9,
			18,10}
local mapCorForWall={-1,-1,1,-1,1,1,-1,1} --надо что бы легко доставать ближайшие стены
local dlg
local wallP={}
wallP.tile=0
wallP.var=-1 -- -1 значит рандом
wallP.flag=0
wallP.hp=0
wallP.mod=0
wallP.mod1=0 -- 0 автомат 1 полуавтомат 2 ручной
wallP.dir=0


fileLoad('UniEdit/img/wallBut.tar.bz2') --загружаем картинки
imgLoad('UniEditWall0.png','UniEditWall0')
imgLoad('UniEditWall1.png','UniEditWall1')
imgLoad('UniEditWall2.png','UniEditWall2')
imgLoad('UniEditWall3.png','UniEditWall3')
imgLoad('UniEditWall4.png','UniEditWall4')
imgLoad('UniEditWall5.png','UniEditWall5')
imgLoad('UniEditWall6.png','UniEditWall6')
imgLoad('UniEditWall7.png','UniEditWall7')
imgLoad('UniEditWall8.png','UniEditWall8')
imgLoad('UniEditWall9.png','UniEditWall9')
imgLoad('UniEditWall10.png','UniEditWall10')

imgLoad('UniEditWall0Dis.png','UniEditWall0Dis')
imgLoad('UniEditWall1Dis.png','UniEditWall1Dis')
imgLoad('UniEditWall2Dis.png','UniEditWall2Dis')
imgLoad('UniEditWall3Dis.png','UniEditWall3Dis')
imgLoad('UniEditWall4Dis.png','UniEditWall4Dis')
imgLoad('UniEditWall5Dis.png','UniEditWall5Dis')
imgLoad('UniEditWall6Dis.png','UniEditWall6Dis')
imgLoad('UniEditWall7Dis.png','UniEditWall7Dis')
imgLoad('UniEditWall8Dis.png','UniEditWall8Dis')
imgLoad('UniEditWall9Dis.png','UniEditWall9Dis')
imgLoad('UniEditWall10Dis.png','UniEditWall10Dis')



function setWall()
	local x,y=cliPlayerMouse()
	x=x-x%23
	y=y-y%23
	if (x%2==1 and y%2==1) or (x%2==0 and y%2==0) then
		local dir,flag,var,t,mod1
		mod1=wallP.mod1
		if wallP.mod1~=2 then
			flag=getNearWallFlags(x,y) --получаем что надо
			for i=1,#mapNearFlag,2 do -- ставим сначало нашу стену
				if mapNearFlag[i]==flag then dir=mapNearFlag[i+1] end
			end
		else
			dir=wallP.dir
		end
		if wallP.flag==0 then
			if wallP.var==-1 then
					var=math.random(0,mapMaxVari(wallP.tile,dir)-1)
				else
					var=wallP.var
			end
		end
		local mayWall=mapSet(wallP.tile,x,y,dir,wallP.flag,var,0,wallP.hp)
		if mod1==0 then
			local walls=getNearWall(mayWall)
			for i=1,#walls do --теперь меняем стены ближайшие
				flag=getNearWallFlags(mapPos(walls[i]))
				for j=1,#mapNearFlag,2 do
					if mapNearFlag[j]==flag then
						setPtrByte(walls[i],0, mapNearFlag[j+1])
						t=mapInfo(walls[i])
						if t.vari>mapMaxVari(t.tile,t.dir)-1 then setPtrByte(walls[i],2,0) end
					end
				end
			end
		end
	end
end

function getNearWallFlags(x,y)
	local c=0 -- то на основании чего мы будем смотреть направление
	local a
	for i=1,8,2 do
		local x1=x+23*mapCorForWall[i]
		local y1=y+23*mapCorForWall[i+1]
		if mapGet(x1,y1)~=nil then c=c+2^((i+1)/2) end
	end
	return c
end


function getNearWall(x)
	local x,y=mapPos(x)
	local t={}
	for i=1,8,2 do
		local x1=x+23*mapCorForWall[i]
		local y1=y+23*mapCorForWall[i+1]
		a=mapGet(x1,y1)
		if a~=nil then table.insert(t,a)  end
	end
	return t
end


function mapPos(a)
	local t=mapInfo(a)
	local x=t.x * 23
	local y=t.y * 23
	return x,y
end

function wallDel()
	local x,y=cliPlayerMouse()
	x=x-x%23
	y=y-y%23
	mapDel(x,y)
end

function wallDlg()
	if dlg~=nil then return end
	local t,listWall,textTile,setBut,delBut,avtomBut,poluAvtomBut,RuchBut,razrurBut
	butDir={}
	local butDir={}
	local ti=table.insert
	local width=410
	local height=310
	dlg={x=0,y=0,w=width,h=height,bgcolor='#0'}
	t={ --Закрываем окно
		type="PUSHBUTTON",
		status="ENABLED+IMAGE",
		style="MOUSETRACK",
		x=width-50,y=height-25,w=45,h=25,
		offsetX=0,offsetY=-1,
		selectedImage="UIButtonSmLit";
		hiliteImage="UIButtonSmLit";
		image="UIButtonSm";
		disabledImage="UIButtonSmDis";
		textColor='#E6A541';
		text="Закр.",
		onClick=function(a,b)
			wndClose(b) dlg=nil
			wallP.mod=0
			wallP.mod1=0
			wallP.hp=0
			wallP.flag=0
			wallP.dir=0
		end
	}
	ti(dlg,t)
	listWall={ -- лист бокс
		type="SCROLLLISTBOX",
		status="ENABLED",
		style="MOUSETRACK+BORDER",
		x=10,y=10,w=160,h=280,
		hiliteColor="#0",
		disabledColor="#0",
		enabledColor="#0",
		textColor="#007F7F",
		selectedColor="#000096",
		maxLines=74,
		onSelChange=function (me,parent,idx)
			wallP.tile=idx
			wndSetText(textTile,mapNameByTile(idx) ..'('..idx..')')
		end
	}
	ti(dlg,listWall)
	textTile={
		type="ENTRYFIELD",
		status="ENABLED+ONE_LINE",
		style="MOUSETRACK",
		x=175,y=10,w=155,h=15,
		textColor='#E6A541';
		textColor='#E6A541';
		selectedColor="#3F007F";
		maxLen=100
		}
	ti(dlg,textTile)
	setBut={
		type="PUSHBUTTON",
		status="ENABLED+IMAGE",
		style="MOUSETRACK",
		x=width-110,y=height-25,w=45,h=25,
		offsetX=0,offsetY=-1,
		selectedImage="UIButtonSmLit",
		hiliteImage="UIButtonSmLit",
		image="UIButtonSm",
		disabledImage="UIButtonSmDis",
		textColor='#E6A541',
		text="Пост.",
		tooltip='Поставить стену',
		onClick=function(a,b)
			if wallP.mod==1 then
				wallP.mod=0
				wndUnGrabMouse(b)
				wndSetAttr(a,'textColor',"#E6A541")
			else
				wallP.mod=1
				wndGrabMouse(b)
				wndSetAttr(delBut,'textColor',"#E6A541")
				wndSetAttr(a,'textColor',"#FF0000")
			end
		end
		}
	ti(dlg,setBut)
	delBut={
		type="PUSHBUTTON",
		status="ENABLED+IMAGE",
		style="MOUSETRACK",
		x=width-170,y=height-25,w=45,h=25,
		offsetX=0,offsetY=-1,
		selectedImage="UIButtonSmLit",
		hiliteImage="UIButtonSmLit",
		image="UIButtonSm",
		disabledImage="UIButtonSmDis",
		textColor='#E6A541',
		text="Удал.",
		tooltip='Удалить стену',
		onClick=function(a,b)
			if wallP.mod==2 then
				wallP.mod=0
				wndUnGrabMouse(b)
				wndSetAttr(a,'textColor',"#E6A541")

			else
				wallP.mod=2
				wndGrabMouse(b)
				wndSetAttr(setBut,'textColor',"#E6A541")
				wndSetAttr(a,'textColor',"#FF0000")
			end
		end
		}

	ti(dlg,delBut)
	avtomBut={
		type = "PUSHBUTTON",
		x=175,y=35,w=70,h=22,
		offsetX=0,offsetY=0,
		status="ENABLED+IMAGE",
		style = "MOUSETRACK",
		selectedImage="UIButtonLgLit",
		hiliteImage="UIButtonLg",
		image="UIButtonLg",
		textColor='#FF0000',
		text="Автомат",
		tooltip='Автоматический режим',
		onClick=function(a)
			wallP.mod1=0
			wndSetAttr(RuchBut,'textColor',"#E6A541")
			wndSetAttr(poluAvtomBut,'textColor',"#E6A541")
			wndSetAttr(a,'textColor',"#FF0000")
			for i=1,11 do
				wndButtonSwitchOff(butDir[i])
			end
		end
	}
	ti(dlg,avtomBut)
	poluAvtomBut={
		type = "PUSHBUTTON",
		x=250,y=35,w=70,h=22,
		offsetX=0,offsetY=0,
		status="ENABLED+IMAGE",
		style = "MOUSETRACK",
		selectedImage="UIButtonLgLit",
		hiliteImage="UIButtonLg",
		image="UIButtonLg",
		textColor='#E6A541',
		text="П. Автомат",
		tooltip='Полуавтоматический режим',
		onClick=function(a)
			wallP.mod1=1
			wndSetAttr(RuchBut,'textColor',"#E6A541")
			wndSetAttr(avtomBut,'textColor',"#E6A541")
			wndSetAttr(a,'textColor',"#FF0000")
			for i=1,11 do
				wndButtonSwitchOff(butDir[i])
			end
		end
	}
	ti(dlg,poluAvtomBut)
	RuchBut={
		type = "PUSHBUTTON",
		x=325,y=35,w=70,h=22,
		offsetX=0,offsetY=0,
		status="ENABLED+IMAGE",
		style = "MOUSETRACK",
		selectedImage="UIButtonLgLit",
		hiliteImage="UIButtonLg",
		image="UIButtonLg",
		textColor='#E6A541',
		text="Ручной",
		tooltip='Ручной режим',
		onClick=function(a)
			wallP.mod1=2
			wndSetAttr(poluAvtomBut,'textColor',"#E6A541")
			wndSetAttr(avtomBut,'textColor',"#E6A541")
			wndSetAttr(a,'textColor',"#FF0000")
			wallP.dir=0
			for i=2,11 do
				wndButtonSwitchOn(butDir[i])
				wndButtonSwitchOff(butDir[1])
			end
		end
	}
	ti(dlg,RuchBut)
	OknoBut={
		type = "PUSHBUTTON",
		x=325,y=65,w=70,h=22,
		offsetX=0,offsetY=0,
		status="ENABLED+IMAGE",
		style = "MOUSETRACK",
		selectedImage="UIButtonLgLit",
		hiliteImage="UIButtonLg",
		image="UIButtonLg",
		textColor='#E6A541',
		text="Окно",
		tooltip='Поставить окно',
		onClick=function(a)
			if wallP.flag==192 then
				wallP.flag=0 wndSetAttr(a,'textColor',"#E6A541")
			else
				wallP.flag=192
				wndSetAttr(razrurBut,'textColor',"#E6A541")
				wndSetAttr(a,'textColor',"#FF0000")
			end
		end
	}
	ti(dlg,OknoBut)
	razrurBut={
		type = "PUSHBUTTON",
		x=250,y=65,w=70,h=22,
		offsetX=0,offsetY=0,
		status="ENABLED+IMAGE",
		style = "MOUSETRACK",
		selectedImage="UIButtonLgLit",
		hiliteImage="UIButtonLg",
		image="UIButtonLg",
		textColor='#E6A541',
		text="Рузрушимая",
		tooltip='Поставить рузрушимую стену',
		onClick=function(a)
			if wallP.flag==8 then
				wallP.flag=0
				wallP.hp=0
				wndSetAttr(a,'textColor',"#E6A541")
			else
				wallP.flag=8
				wallP.hp=1
				wndSetAttr(OknoBut,'textColor',"#E6A541")
				wndSetAttr(a,'textColor',"#FF0000")
			end
		end
	}
	ti(dlg,razrurBut)
	t={
		type = "PUSHBUTTON",
		x=175,y=65,w=70,h=22,
		offsetX=0,offsetY=0,
		status="IMAGE",
		style = "MOUSETRACK",
		selectedImage="UIButtonLgLit",
		hiliteImage="UIButtonLg",
		image="UIButtonLg",
		disabledImage='UIButtonLgDis',
		textColor='#E6A541',
		text="Секретная",
		tooltip='Поставить секретную стену',
	}
	ti(dlg,t)


	-- кнопки направления
	t={
		type = "PUSHBUTTON",
		x=245,y=95,w=20,h=20,
		offsetX=0,offsetY=0,
		status="IMAGE",
		style = "MOUSETRACK",
		selectedImage="UniEditWall0Dis",
		hiliteImage="UniEditWall0",
		image="UniEditWall0",
		disabledImage='UniEditWall0Dis',
		textColor='#E6A541',
		onClick=function(a)
			wndButtonSwitchOff(a)
			wndButtonSwitchOn(butDir[wallP.dir+1])
			wallP.dir=0
		end
	}
	ti(butDir,t)
		t={
		type = "PUSHBUTTON",
		x=275,y=95,w=20,h=20,
		offsetX=0,offsetY=0,
		status="IMAGE",
		style = "MOUSETRACK",
		selectedImage="UniEditWall1Dis",
		hiliteImage="UniEditWall1",
		image="UniEditWall1",
		disabledImage='UniEditWall1Dis',
		textColor='#E6A541',
		onClick=function(a)
			wndButtonSwitchOff(a)
			wndButtonSwitchOn(butDir[wallP.dir+1])
			wallP.dir=1
		end
	}
	ti(butDir,t)
	t={
		type = "PUSHBUTTON",
		x=305,y=95,w=20,h=20,
		offsetX=0,offsetY=0,
		status="IMAGE",
		style = "MOUSETRACK",
		selectedImage="UniEditWall2Dis",
		hiliteImage="UniEditWall2",
		image="UniEditWall2",
		disabledImage='UniEditWall2Dis',
		textColor='#E6A541',
		onClick=function(a)
			wndButtonSwitchOff(a)
			wndButtonSwitchOn(butDir[wallP.dir+1])
			wallP.dir=2
		end
	}
	ti(butDir,t)
	t={
		type = "PUSHBUTTON",
		x=290,y=225,w=20,h=20,
		offsetX=0,offsetY=0,
		status="IMAGE",
		style = "MOUSETRACK",
		selectedImage="UniEditWall3Dis",
		hiliteImage="UniEditWall3",
		image="UniEditWall3",
		disabledImage='UniEditWall3Dis',
		textColor='#E6A541',
		onClick=function(a)
			wndButtonSwitchOff(a)
			wndButtonSwitchOn(butDir[wallP.dir+1])
			wallP.dir=3
		end
	}
	ti(butDir,t)
	t={
		type = "PUSHBUTTON",
		x=260,y=225,w=20,h=20,
		offsetX=0,offsetY=0,
		status="IMAGE",
		style = "MOUSETRACK",
		selectedImage="UniEditWall4Dis",
		hiliteImage="UniEditWall4",
		image="UniEditWall4",
		disabledImage='UniEditWall4Dis',
		textColor='#E6A541',
		onClick=function(a)
			wndButtonSwitchOff(a)
			wndButtonSwitchOn(butDir[wallP.dir+1])
			wallP.dir=4
		end
	}
	ti(butDir,t)
	t={
		type = "PUSHBUTTON",
		x=260,y=195,w=20,h=20,
		offsetX=0,offsetY=0,
		status="IMAGE",
		style = "MOUSETRACK",
		selectedImage="UniEditWall5Dis",
		hiliteImage="UniEditWall5",
		image="UniEditWall5",
		disabledImage='UniEditWall5Dis',
		textColor='#E6A541',
		onClick=function(a)
			wndButtonSwitchOff(a)
			wndButtonSwitchOn(butDir[wallP.dir+1])
			wallP.dir=5
		end
	}
	ti(butDir,t)
	t={
		type = "PUSHBUTTON",
		x=290,y=195,w=20,h=20,
		offsetX=0,offsetY=0,
		status="IMAGE",
		style = "MOUSETRACK",
		selectedImage="UniEditWall6Dis",
		hiliteImage="UniEditWall6",
		image="UniEditWall6",
		disabledImage='UniEditWall6Dis',
		textColor='#E6A541',
		onClick=function(a)
			wndButtonSwitchOff(a)
			wndButtonSwitchOn(butDir[wallP.dir+1])
			wallP.dir=6
		end
	}
	ti(butDir,t)
	t={
		type = "PUSHBUTTON",
		x=275,y=165,w=20,h=20,
		offsetX=0,offsetY=0,
		status="IMAGE",
		style = "MOUSETRACK",
		selectedImage="UniEditWall7Dis",
		hiliteImage="UniEditWall7",
		image="UniEditWall7",
		disabledImage='UniEditWall7Dis',
		textColor='#E6A541',
		onClick=function(a)
			wndButtonSwitchOff(a)
			wndButtonSwitchOn(butDir[wallP.dir+1])
			wallP.dir=7
		end
	}
	ti(butDir,t)
	t={
		type = "PUSHBUTTON",
		x=255,y=145,w=20,h=20,
		offsetX=0,offsetY=0,
		status="IMAGE",
		style = "MOUSETRACK",
		selectedImage="UniEditWall8Dis",
		hiliteImage="UniEditWall8",
		image="UniEditWall8",
		disabledImage='UniEditWall8Dis',
		textColor='#E6A541',
		onClick=function(a)
			wndButtonSwitchOff(a)
			wndButtonSwitchOn(butDir[wallP.dir+1])
			wallP.dir=8
		end
	}
	ti(butDir,t)
	t={
		type = "PUSHBUTTON",
		x=275,y=125,w=20,h=20,
		offsetX=0,offsetY=0,
		status="IMAGE",
		style = "MOUSETRACK",
		selectedImage="UniEditWall9Dis",
		hiliteImage="UniEditWall9",
		image="UniEditWall9",
		disabledImage='UniEditWall9Dis',
		textColor='#E6A541',
		onClick=function(a)
			wndButtonSwitchOff(a)
			wndButtonSwitchOn(butDir[wallP.dir+1])
			wallP.dir=9
		end
	}
	ti(butDir,t)
	t={
		type = "PUSHBUTTON",
		x=295,y=145,w=20,h=20,
		offsetX=0,offsetY=0,
		status="IMAGE",
		style = "MOUSETRACK",
		selectedImage="UniEditWall10Dis",
		hiliteImage="UniEditWall10",
		image="UniEditWall10",
		disabledImage='UniEditWall10Dis',
		textColor='#E6A541',
		onClick=function(a)
			wndButtonSwitchOff(a)
			wndButtonSwitchOn(butDir[wallP.dir+1])
			wallP.dir=10
		end
	}
	ti(butDir,t)


	ti(dlg,butDir[4])
	ti(dlg,butDir[3])
	ti(dlg,butDir[2])
	ti(dlg,butDir[1])
	ti(dlg,butDir[5])
	ti(dlg,butDir[6])
	ti(dlg,butDir[7])
	ti(dlg,butDir[8])
	ti(dlg,butDir[9])
	ti(dlg,butDir[10])
	ti(dlg,butDir[11])


	dlg.onGrabMouse=function(a)
		if wallP.mod==1 then setWall() else wallDel() end
		wndGrabMouse(a)
	end

	wndCreate(dlg)
	for i=0,74 do
		wndLbAddText(listWall,mapNameByTile(i) ..'('..i..')')
	end
end

