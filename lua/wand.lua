-- ��� �������� ���� ���������� ������ � �����
--[[
wanddata (BG +0x2e0)
+0x54 (DD) - ��� ��������
+0x58 (DD) - ����
+0x64 (DD) - ����� ����� ����������
+0x68 (DD) - ����� (���������� ��� �����������)
+0x6C (DB) - �����
+0x6D (DB) - ���� �����
+0x70 (DD) - ������� ������ (���)
]]--
local wandLowMob=1339
local wandLowTime=0 --
local wandLowMaxShot=255
local wandSound=135 --46

function createWand()
	local wand=cr'lesserfireballWand' -- ������� ��� ����
	local wd=getPtrPtr(wand,0x2e0) -- ����� ��������
	local functionShot
	-- ������������� �������� �����
		setPtrShort(wd,0x54,wandLowMob) -- ��� �������� (�������)
		setPtrShort(wd,0x64,wandLowTime) -- ������������� ����� ����� ����������
		setPtrByte(wd,0x6d,wandLowMaxShot) -- ������������� ���� ���������
		setPtrByte(wd,0x6c,wandLowMaxShot)
		setPtrByte(wd,0x58,wandSound) -- ������������� ����
	-- end
	functionShot=function(player,wand)
		-- ������ ��������
		local wd=getPtrPtr(wand,0x2e0) -- ����� ��������
		local shot=getPtrByte(wd,0x6c)-1 -- ����� ������
		local maxShot=getPtrByte(wd,0x6d)
		local frameCounter=getFrameCounter()
		--
		-- ������ ��������
		if frameCounter-getPtrShort(wd,0x68)<=getPtrShort(wd,0x64) then return end -- ����� ������-����� �����<=����� �� ��������)
		if shot<=-1 then return end -- ���� ������ ��� ����� ���� �� ������� �� ����
		--
		--��� ��������
		local xm,ym=playerMouse(player)
		local x,y=unitPos(player)
		if mapTraceRay(x,y,xm,ym)==false then return end -- ���� ������� �� ����� �������

		local mob=createObject("blackwolf",xm,ym) --������� ����
		netPointFx(0x81,xm,ym) -- ������ 
		netPointFx(0x8a,xm,ym)
		unitBecomePet(mob,player) -- ������ �����������
		setPtrByte(mob,0x7e,getPtrByte(player,0x7e))
		setTimeout(function() local x,y=unitPos(mob) netPointFx(0x84,x,y) netPointFx(0x8a,x,y) soundMake(46,x,y) unitDecay(mob,1) end,30*60) --������� ���� ����� 30 ���

		soundMake(getPtrShort(wd,0x58),unitPos(player)) -- ������ ����
		setPtrByte(wd,0x6c,shot) -- ������������� ����� ��������
		setPtrShort(wd,0x70,((100*shot)-((100*shot)%maxShot))/maxShot) --������� ������������� (��� �������)
		netReportCharges(player,wand,shot,maxShot) -- ���������� ���������
		setPtrShort(wd,0x68,frameCounter) -- ����� ������
		--
	end
	unitOnUse(wand,functionShot) -- ������������� �� ���������
end
