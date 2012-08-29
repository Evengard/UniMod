local mobTable=
{
        {
                'Demon',
                'MechanicalGolem',
                'Mimic',
                'WizardGreen',
                'Lich',
                timeR=60
        },
        {
                'UrchinShaman',
                'StoneGolem',
                'WillOWisp',
                'Beholder',
                timeR=45
        },
        {
                'Bear',
                'EmberDemon',
                'OgreBrute',
                'OgreWarlord',
                'Shade',
                'SkeletonLord',
                'BlackWolf',
                timeR=30
        },
        {
                'BlackBear',
                'EvilCherub',
                'GruntAxe',
                'SpittingSpider',
                'Troll',
                'VileZombie',
                'WhiteWolf',
                'Wolf',
                'Zombie',
                'Scorpion',
                'AlbinoSpider',
                'Skeleton',
                timeR=20
        },
        {
                'Bat',
                'FlyingGolem',
                'Ghost',
                'GiantLeech',
                'Imp',
                'SmallAlbinoSpider',
                'SmallSpider',
                'Spider',
                'Urchin',
                'Wasp',
                timeR=10
        }
}

local fragsTable={3,2,1,1,0}

local mobPlayerStartT={}
local mobGenMob={}
local mobGenOn=0
local mobGenMaxMob=30
local mobGenFirst=0
local teamPlayers=0
local teamMonsters=0

function playerOnDieMobs(a,b)
        if mobGenOn~=1 then return end
        if b~=nil then
                if bitAnd(unitClass(b),2)==2 then
                       teamScore(teamMonsters,teamGet(teamMonsters).score+2)
                end
        end
        if a==b then playerScore(a,playerScore(a)-1) end
end

--[[
function playerOnDieMobs(a,b)
        if b~=nil then
                if bitAnd(unitClass(b),2)==2 then
                        playerScore(a,playerScore(a)-10)
                end
        end
end
--]]

function mobGenGetLevel(a)
        local mob=getThingName(getThingType(a))
        local t
        local level
        for i,q in pairs(mobTable) do
                for j,k in pairs(q) do
                        if k==mob then return i end
                end
        end
end

function mobGenGetP()
        local fn
        fn=function(a)
                if getThingType(a)==0x2CA then table.insert(mobPlayerStartT,a) end
        end
        unitGetAround(fn,0,0,5800,5800)
end


function mobGenDie(a,b)
        local level
        for i,q in pairs(mobGenMob) do
                if a==q then
					table.remove(mobGenMob,i)
					level=mobGenGetLevel(a)
					setTimeout(function() mobGenMobCr(level) end,mobTable[level].timeR*30)
					unitDecay(a,40)
					if bitAnd(unitClass(b),4)==4 then
						playerScore(b,playerScore(b)+fragsTable[level])
						teamScore(teamPlayers,teamGet(teamPlayers).score+fragsTable[level])
					end
                end
        end
end

function mobGenStop()
        if mobGenOn==1 then
                local c=playerList()
                for i,q in pairs(c) do
                        if playerScore(q)~=0 then playerScore(q,0) end
                end
        end
        mobGenOn=0
        for i,q in pairs(mobGenMob) do
                unitDecay(q,1)
        end
        mobGenReset()
end

function mobGenStart()
		mobGenOn=1
		teamPlayers=teamCreate({color=1,name="People"})
		teamMonsters=teamCreate({color=2,name="Monsters"})
		local players=playerList()
		for i=1,#players do
			teamAssign({team=teamPlayers,player=players[i]})
		end
		mobGen()
end

function mobGenReset()
		mobPlayerStartT={}
		mobGenMob={}
		mobGenOn=0
		mobGenFirst=0
		teamDelete(teamPlayers)
		teamDelete(teamMonsters)
		teamPlayers=0
		teamMonsters=0
end

function mobGenMobCr(Group)
local x,y,mob,mobt
        if mobGenOn==1 then
                if #mobGenMob<mobGenMaxMob then
                        x,y=unitPos(mobPlayerStartT[math.random(1,#mobPlayerStartT)])
                        mob=mobTable[Group]
                        mob=mob[math.random(1,#mob)]
                        mob=createObject(mob,x,y)
                        unitOnDie(mob,mobGenDie)
                        table.insert(mobGenMob,mob)
                        netPointFx(0x8a,x,y)
                        setTimeout(function() if bitAnd(unitFlags(mob),0x8040)==0 then unitHunt(mob) end end,1)
                end
        end
end

function mobGenFirstMob(Group)
        if mobGenOn==1 then
                if mobGenFirst<mobGenMaxMob then
                        mobGenFirst=mobGenFirst+1
                        mobGenMobCr(Group)
                        setTimeout(function() mobGenFirstMob(Group) end,mobTable[Group].timeR*30)
                end
        end
end

function mobGen()
        mobGenGetP()
        local fn
        fn=function()
                if mobGenOn==1 then
                        if #mobGenMob<mobGenMaxMob then
                                for i=1,5 do
                                        mobGenFirstMob(i)
                                end
                        end
                end
        end
        fn()
end

function mobGenLoader(mode)
        local c=getPlayerList()
                local ingame=0
                for i,q in pairs(c) do
                        if q.isObserver==0 and q.login~="" then
                                ingame=ingame+1
                        end
                end
        local d=1+mode
        if ingame<d then mobGenStart() else mobGenStop() end
        print("ingame: "..ingame)
        print("mode: "..mode)
        print("limit (d): "..d)
        print("mobGenOn: "..mobGenOn)
        -- conExec("say "..ingame)
end

function mobGenRestart()
        mobGenReset()
        mobGenLoader(1)
end
