function wndAsk(text,fnYes,fnNo) -- типо мессаж бокса
	local wind
	local t={}
	wind=wndLoad('YesNoUni.wnd',t)
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
	textFild=wndChildById(wind,23661)
	wndSetTextLB(textFild,text)
	ButtonYes=wndChildById(wind,23662)
	wndSetTextBut(ButtonYes,'Да')
	ButtonNo=wndChildById(wind,23663)
	wndSetTextBut(ButtonNo,'Нет')
end
