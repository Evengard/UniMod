clientOnJoin=function ()
	netVersionRq()
end

local function updateAsk(text,dVers,fnYes,fnNo) -- ��������� 1. ����� � �������� ����� �����, 2. ��������� ������ �������
										   -- 3. �� ����������� ���� �� � 4. �� ���� ���
	local wind
	local t={}
	
	wind=wndLoad('updateWnd.wnd',t)
	t.wndProc=function(t,m,a,b)
		if m==0x4007 then
			if wndGetId(a)==23662 then --��
				if fnYes~=nil then
					setTimeout(fnYes,0)
				 end
			 wndClose(wind)
			 else
				if wndGetId(a)==23663 then --���
					if fnNo~=nil then
						setTimeout(fnNo,0)
					end
				 wndClose(wind)
				end
			 end
		end
	end

	wndLbAddText(wndChildById(wind,23661),text)

	wndLbAddText(wndChildById(wind,23664),"������� ������ UniMod-a: "..netGetVersion())
	wndLbAddText(wndChildById(wind,23665),"��������� ������ UniMod-a: "..dVers)

	ButtonYes=wndChildById(wind,23662)
	wndButtonSetText(ButtonYes,'��')
	ButtonNo=wndChildById(wind,23663)
	wndButtonSetText(ButtonNo,'���')
end
-- � ����� ������� (������ �����) os.execute ��������, � ����� ����� ������, ��� ���� �������� ���
local exec=os.execute -- ������ � �����

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