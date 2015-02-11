dofile('./scripts/unitUtil.lua')

-- обработчики зачарований
local function powerIncStrength(enchant, holder)
    if (unitIsPlayer(holder)) then
        -- увеличиваем силу
        playerSetStrength(holder, playerGetStrength(holder) + 8)
    end
end

local function powerDecStrength(enchant, holder)
    if (unitIsPlayer(holder)) then
        -- уменьшаем силу
        playerSetStrength(holder, playerGetStrength(holder) - 8)
    end
end

local function distance(x1, y1, x2, y2)
    local dx = x1 - x2
    local dy = y1 - y2
    return math.sqrt(dx * dx + dy * dy)
end

local function pickClosestUnitNotAttacked(around, attacker, dist)
    local closest = dist * 2
    local aX, aY = unitPos(around)
    local result = nil
    
    local fn = function(p)
        if (unitIsMonster(p) or unitIsPlayer(p)) then
            if (unitCanAttack(attacker, p) and unitLastDamageDiff(p) > 4) then
                -- если юнит не был атакован последние 4 фрейма, видим и враждебен
                local dX, dY = unitPos(p)
                local d = distance(aX, aY, dX, dY)
                if (d < closest and mapTraceRay(aX, aY, dX, dY, 5)) then
                    result = p
                    closest = d
                end
            end
        end
    end
    
    unitGetAround(fn, aX - dist, aY - dist, aX + dist, aY + dist)
    return result
end

local function applyWrathEffect(ench, weapon, owner, target)
    if (unitIsMonster(target) or unitIsPlayer(target)) then
        
        if (math.random(0, 2) > 0) then
            
            local x1, y1 = unitPos(owner)
            local x2, y2 = unitPos(target)
            local dmg = 10

            if (unitIsPlayer(owner)) then
                dmg = dmg + math.floor(playerGetStrength(owner) / 10)
            else
                dmg = dmg + 10
            end

            -- бьем по основной цели вне зависимости от видимости/расстояния
            netGreenLightning(x1, y1, x2, y2, 4)
            soundMake(77, x2, y2)
            -- ей наносится только половина урона
            unitDamage(target, owner, owner, math.ceil(dmg / 2), 9)

            -- рядом с основной целью ищем отскок
            local enemy = target
            for i = 1, 4 do
                enemy = pickClosestUnitNotAttacked(enemy, owner, 70)
                if (enemy ~= nil) then
                    unitDamage(enemy, owner, nil, dmg, 9)
                    x1 = x2 y1 = y2
                    x2, y2 = unitPos(enemy)
                    soundMake(77, x2, y2)
                    netGreenLightning(x1, y1, x2, y2, 4)
                    -- можно еще запилить задержку чтобы красивее было
                end
            end
            
        end
            
    end
end

local function applyHastyEffect(ench, weapon, owner, target)
    if (unitIsPlayer(owner)) then
        
        uc = getPtrPtr(owner, 0x2EC)
        local delay = getPtrInt(uc, 0) - getFrameCounter()
        print(getPtrByte(uc, 0x5B))
        setPtrByte(uc, 0x5B, 100)
        
    end
end

local function applyCritEffect(ench, weapon, owner, damage)
    if (math.random(0, 3) == 0) then 
        -- увеличиваем урон в два раза 1|4 = 25%
        damage = damage * 2
        if (owner ~= nil) then soundMake(771, unitPos(owner)) end
    end
    
    return damage
end

local function reduceDebuffTime(unit, buffN)
    local frames = 15
    local buffTime = getPtrShort(unit, 0x158 + buffN * 2)
    if (buffTime > frames) then setPtrShort(unit, 0x158 + buffN * 2, frames) end
end

local function purificationUpdateEffect(ench, armor, owner)
    if (owner == nil) then return end
    
    -- уменьшаем длительность дебаффов
    reduceDebuffTime(owner, buffFindID("ENCHANT_CONFUSED"))
    reduceDebuffTime(owner, buffFindID("ENCHANT_SLOWED"))
    reduceDebuffTime(owner, buffFindID("ENCHANT_HELD"))
    reduceDebuffTime(owner, buffFindID("ENCHANT_RUN"))
    reduceDebuffTime(owner, buffFindID("ENCHANT_ANCHORED"))
end

local function ignitionUpdateEffect(ench, armor, owner)
    if (unitIsDead(owner)) then return end
    local oX, oY = unitPos(owner)
    local effectArea = 90
    
    -- наносим урон каждые 7 фреймов
    if (math.mod(getFrameCounter(), 7) > 0) then return end
    
    local fn = function(p)
        if (unitIsMonster(p) or unitIsPlayer(p)) then
            local eX, eY = unitPos(p)
            if (unitCanAttack(owner, p) and mapTraceRay(oX, oY, eX, eY, 5)) then
                -- все враждебные юниты рядом получают урон
                setPtrPtr(p, 0x208, owner) -- HACK
                unitDamage(p, nil, nil, 2, 1) -- 2 единицы урона огнем
            end
        end
    end
    
    unitGetAround(fn, oX - effectArea, oY - effectArea, oX + effectArea, oY + effectArea)
end

local function clarityUpdateEffect(ench, armor, owner)
    if (not unitIsPlayer(owner)) then return end
    
    local uc = getPtrPtr(owner, 0x2EC)
    local plrAction = getPtrByte(uc, 0x58)
    -- Если игрок не двигается, каждый третий фрейм добавляем единицу маны
    if (plrAction == 13 or plrAction == 10) then
        if (math.mod(getFrameCounter(), 3) == 2) then
            playerAddMana(owner, 1)
        end
    end
end

-- таблицы с описаниями зачар
local enchantWrath1Table = { name = "Wrath", text = "Wrath", effectPrehit = applyWrathEffect }
local enchantHastyTable = { name = "Hasty", text = "Hasty", effectPrehit = applyHastyEffect }
local enchantPower1Table = { name = "Power1", text = "Power", effectEngage = powerIncStrength, effectDisengage = powerDecStrength }
local enchantDestroyerTable = { name = "Crits", text = "Crits", effectAttack = applyCritEffect }
local enchantPurificationTable = { name = "Purification", text = "Purification", effectUpdate = purificationUpdateEffect }
local enchantIgnitionTable = { name = "Ignition", text = "Ignition", effectUpdate = ignitionUpdateEffect }
local enchantClarityTable = { name = "Clarity", text = "Clarity", effectUpdate = clarityUpdateEffect }
-- создаем зачарования
itemMakeEnchant(enchantHastyTable)
itemMakeEnchant(enchantWrath1Table)
itemMakeEnchant(enchantPower1Table)
itemMakeEnchant(enchantDestroyerTable)
itemMakeEnchant(enchantPurificationTable)
itemMakeEnchant(enchantIgnitionTable)
itemMakeEnchant(enchantClarityTable)