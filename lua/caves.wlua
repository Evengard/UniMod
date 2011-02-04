dofile('caves.lua')

local BoxSize=10 -- размер клетки на экране

require( "cdlua" )
require( "iuplua" )
require( "iupluacd" )
require( "cdluacontextplus" )

cd.InitContextPlus()
cd.UseContextPlus(true)

local canvasControl=iup.canvas{expand='yes';font='Fixedsys';
	xmin=0,xmax=99,posx=0,dx=10}
local cnv
local map={} -- сюда клетки идут

function canvasControl:action()
	cnv:Activate()
	cnv:Background(cd.WHITE)
	cnv:Clear()
	cnv:wLineWidth(0.5)
-- здесь все делаем

	for _,q in pairs(caves) do
		cnv:Foreground(cd.DARK_GREEN)
		cnv:Arc(BoxSize*q.x,BoxSize*q.y,BoxSize*q.R*2,BoxSize*q.R*2,0,360)
		cnv:wLineWidth(0.5)
		cnv:Foreground(cd.GREEN)
		cnv:Begin(cd.OPEN_LINES)
		local caveWall=q.caveWall
		local a=2*math.pi/q.caveSize
		for i=1,q.caveSize do
			r=caveWall[i]
			cnv:Vertex(BoxSize*(q.x+math.cos(a*i)*r),
				BoxSize*(q.y+math.sin(a*i)*r)
				)
		end
		cnv:End()
	end
	local a,b=cnv:GetSize()
	local x,y
	x=0
	y=0
	cnv:Background(cd.BLUE)
	local c1=cd.EncodeColor(198,198,0)
	local c2=cd.EncodeColor(0,0,0)
	c1=cd.EncodeAlpha(c1,70)
	c2=cd.EncodeAlpha(c2,70)
	cnv:Background(c1)
	for y0=0,#map do
		y=y0*BoxSize
		for x0=0,#map do
			x=x0*BoxSize
			if map[y0][x0]=='w' then
				cnv:Foreground(c1)
			else
				cnv:Foreground(c2)
			end
			cnv:Box(x,x+BoxSize-2,y,y+BoxSize-2)
		end
	end

end
do
function canvasControl:map_cb()
	cnv=cd.CreateCanvas(cd.IUP, self)
end
function canvasControl:resize_cb()
	cnv:Activate()
	local a,b=cnv:GetSize() --нужен только размер
	local boxW=1+math.floor(a/BoxSize)
	local boxH=1+math.floor(b/BoxSize)
	for y0=0,boxH do
		map[y0]={}
	end
	makeCaves(map,boxW,boxH)
end

local button=iup.button{title="Generate"}
local text=iup.multiline{expand='yes';value="";font='Fixedsys'}

function button:action ()
    local p=print
    local v=''
    print=function(x) v=v..tostring(x)..'\n' end
--[[	local a,b=loadstring('return {'..text.value..'}')
	if not a then
		print(b)
	else
		local tiles=a()
		generate(tiles)
	end
	]]--
	local a,b=cnv:GetSize() --нужен только размер
	local boxW=1+math.floor(a/BoxSize)
	local boxH=1+math.floor(b/BoxSize)
	makeCaves(map,boxW,boxH)
	canvasControl:action()
    print=p
    return iup.DEFAULT
end

local dlg = iup.dialog{
		iup.hbox{
			EXPANDCHILDREN='BOTH';
			EXPAND='YES';
			canvasControl,
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

