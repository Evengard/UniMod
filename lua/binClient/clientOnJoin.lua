clientOnJoin=function ()
	netVersionRq()
end

local function updateAsk(text,dVers,fnYes,fnNo) -- параметры 1. Текст в огромном текст боксе, 2. Доступная версия юнимода
										   -- 3. Фн выполняется если да и 4. фн если нет
	local wind
	local t={}
	
	wind=wndLoad('updateWnd.wnd',t)
	t.wndProc=function(t,m,a,b)
		if m==0x4007 then
			if wndGetId(a)==23662 then --да
				if fnYes~=nil then
					setTimeout(fnYes,0)
				 end
			 wndClose(wind)
			 else
				if wndGetId(a)==23663 then --нет
					if fnNo~=nil then
						setTimeout(fnNo,0)
					end
				 wndClose(wind)
				end
			 end
		end
	end

	wndLbAddText(wndChildById(wind,23661),text)

	wndLbAddText(wndChildById(wind,23664),"Текущая версия UniMod-a: "..netGetVersion())
	wndLbAddText(wndChildById(wind,23665),"Доступная версия UniMod-a: "..dVers)

	ButtonYes=wndChildById(wind,23662)
	wndButtonSetText(ButtonYes,'Да')
	ButtonNo=wndChildById(wind,23663)
	wndButtonSetText(ButtonNo,'Нет')
end
-- к этому моменту (внутри дллки) os.execute работает, а потом будет удален, для чего сохраним его
local exec=os.execute -- кладем в локал

updateVersion=function ()
	 exec ('start http://forum.noxworld.ru/kartostroenie-skripting-modding/reliz-al~fa-versii-unimod-a-701/')
end
clientOnServerVersion=function (myVer,himVer)
	if myVer<himVer then
		local function fnYes()
			updateVersion()
		end
		local function fnNo()
		
		end
		httpGet()
		updateAsk()
	end
end