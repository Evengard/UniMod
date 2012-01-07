-- все подробно дабы ознакомить юзеров с кодом
--[[
wanddata (BG +0x2e0)
+0x54 (DD) - Чем стрелять
+0x58 (DD) - звук
+0x64 (DD) - время между выстрелами
+0x68 (DD) - время (необходимо для перезарядки)
+0x6C (DB) - Заряд
+0x6D (DB) - Макс заряд
+0x70 (DD) - процент заряда (ЛОЛ)
]]--
local wandLowMob=1339
local wandLowTime=0 --
local wandLowMaxShot=255
local wandSound=135 --46

function createWand()
	local wand=cr'lesserfireballWand' -- создаем сам ванд
	local wd=getPtrPtr(wand,0x2e0) -- берем вандДату
	local functionShot
	-- устанавливаем свойства ванда
		setPtrShort(wd,0x54,wandLowMob) -- чем стреляем (урчином)
		setPtrShort(wd,0x64,wandLowTime) -- устанавливаем время между выстрелами
		setPtrByte(wd,0x6d,wandLowMaxShot) -- устанавливаем макс выстрелов
		setPtrByte(wd,0x6c,wandLowMaxShot)
		setPtrByte(wd,0x58,wandSound) -- устанавливаем звук
	-- end
	functionShot=function(player,wand)
		-- всякие значения
		local wd=getPtrPtr(wand,0x2e0) -- берем вандДату
		local shot=getPtrByte(wd,0x6c)-1 -- берем заряды
		local maxShot=getPtrByte(wd,0x6d)
		local frameCounter=getFrameCounter()
		--
		-- всякие проверки
		if frameCounter-getPtrShort(wd,0x68)<=getPtrShort(wd,0x64) then return end -- время сейчас-вермя тогда<=время на задержку)
		if shot<=-1 then return end -- если меньше или равно нулю то выходим из сабы
		--
		--код выстрела
		local xm,ym=playerMouse(player)
		local x,y=unitPos(player)
		if mapTraceRay(x,y,xm,ym)==false then return end -- если невидно то нафиг парится

		local mob=createObject("blackwolf",xm,ym) --создаем моба
		netPointFx(0x81,xm,ym) -- эффект 
		netPointFx(0x8a,xm,ym)
		unitBecomePet(mob,player) -- делаем дружелюбным
		setPtrByte(mob,0x7e,getPtrByte(player,0x7e))
		setTimeout(function() local x,y=unitPos(mob) netPointFx(0x84,x,y) netPointFx(0x8a,x,y) soundMake(46,x,y) unitDecay(mob,1) end,30*60) --убираем моба через 30 сек

		soundMake(getPtrShort(wd,0x58),unitPos(player)) -- делаем звук
		setPtrByte(wd,0x6c,shot) -- устанавливаем новое значение
		setPtrShort(wd,0x70,((100*shot)-((100*shot)%maxShot))/maxShot) --процент разряженности (без остатка)
		netReportCharges(player,wand,shot,maxShot) -- отправляем изменения
		setPtrShort(wd,0x68,frameCounter) -- время сейчас
		--
	end
	unitOnUse(wand,functionShot) -- устанавливаем фн стреляния
end
