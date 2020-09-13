-- functions.lua

--[[
    LGCK Builder Runtime
    Copyright (C) 1999, 2012  Francois Blanchette

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
--]]

if unpack == nil then
    -- to keep lua 5.2 happy
    unpack = table.unpack;
end

function ticks()
    return getTicks();
end

--[[

    Counters -----------------------------------------------------------

]]--

Counters = {}
Counters.__index = Counters;

function Counters:set(s, value)
	counters_set(s, value)
end

function Counters:get(s)
	return counters_get(s)
end

function Counters:inc(s)
	return counters_inc(s)
end

function Counters:dec(s)
	return counters_dec(s)
end

function Counters:add(s,v)
	return counters_add(s,v)
end


--[[

    Debug ------------------------------------------------------------

]]--

Debug = {};
Debug.__index = Debug;

function Debug:printf( fmt, ... )
  local arg = {...}
  ss_debug(string.format(fmt, unpack(arg)));
end

function Debug:hide()
-- hide the console
  local display = getDisplayByName("debug");
  display:hide();
end

function Debug:show()
-- show the console
  local display = getDisplayByName("debug");
  display:show();
end

function Debug:clear()
-- clear the console
  debugClear();
end

function showConsole(show)
-- show the console
  local display = getDisplayByName("debug");
  if show then
    display:show();
  else
    display:hide();
  end
end

--[[

    Display ----------------------------------------------------------

]]--

Display = {};
Display.__index = Display;

function display_hideShadow(displayId)
    display_enableShadows(displayId, false);
end

function display_showShadow(displayId)
    display_enableShadows(displayId, true);
end

function display_setSize(displayId, pxSize)
    display_setFontSize(displayId, pxSize);
end

function display_show(displayId)
    displaySetVisible(displayId, true);
end

function display_hide(displayId)
    displaySetVisible(displayId, false);
end

function display_setXY(displayId, x, y)
    display_move(displayId, x, y);
end

function getDisplayById(id)
  local display = {};
  setmetatable(display, Display);
  display.id = id;
  return display;
end

function display_getById(id)
    return getDisplayById(id)
end

function getDisplayByName(name)
  return getDisplayById(findDisplay(name));
end

function display_getByName(name)
  return getDisplayById(findDisplay(name));
end

function addDisplay(name, x, y, displayTime)
  return getDisplayById( addDisplayC(name, x, y, displayTime) );
end

function display_add(name, x, y, displayTime)
  return getDisplayById( addDisplayC(name, x, y, displayTime) );
end

function removeDisplayByName(name)
  removeDisplayById( findDisplay(name) );
end

function display_removeByName(name)
  removeDisplayById( findDisplay(name) );
end

function Display:setXY(x, y)
  displaySetXY(self.id, x, y);
end

function Display:move(x, y)
  displaySetXY(self.id, x, y);
end

function Display:remove()
  removeDisplayById(self.id);
  self.id = -1;
end

function Display:setType(displayType)
  displaySetType(self.id, displayType);
end

function Display:setText(text)
  displaySetText(self.id, text);
end

function Display:setColor(r, g, b, a)
  displaySetColor(self.id, r, g, b, a);
end

function Display:setExpireTime(time)
  displaySetExpireTime(self.id, time);
end

function Display:expire(time)
  displaySetExpireTime(self.id, time);
end

function Display:show(visible)
  displaySetVisible(self.id, true);
end

function Display:hide()
  displaySetVisible(self.id, false);
end

function Display:setFontSize(pxSize)
  displaySetFontSize(self.id, pxSize);
end

function Display:setSize(pxSize)
  displaySetFontSize(self.id, pxSize);
end

function Display:setContent(text)
  displaySetContent(self.id, content);
end

function Display:setAlpha(alpha)
  displaySetAlpha(self.id, alpha);
end

function Display:enableShadows(enable)
    displayEnableShadows(self.id, enable);
end

function Display:enableShadow(enable)
    displayEnableShadows(self.id, enable);
end

function Display:setShadowOffset(tx, ty)
    displaySetShadowOffset(self.id, tx, ty);
end

function Display:setShadowColor(r, g, b, a)
    displaySetShadowColor(self.id, r, g, b, a);
end

function Display:sizeText(...)
    local arg = {...}
    return display_sizeText(self.id, unpack(arg));
end

function Display:setFlagXY(flagX, flagY)
    return display_setFlagXY(self.id, flagX, flagY);
end

function Display:setFont(fontID)
    return display_setFont(self.id, fontID);
end

function Display:setTemplate(s)
    return display_setTemplate(self.id, s);
end

function Display:new(name, x, y, displayType)
    local display = {};
    setmetatable(display, Display);
    display.id = display_new(name, x, y, displayType);
    display.name = name;
    return display;
end

function Display:setImage(imageSet, imageNo)
    displaySetImage(self.id, imageSet, imageNo );
end

function Display:setSource(source)
    display_setSource(self.id, source);
end

function Display:find(name)
    return getDisplayById(findDisplay(name));
end

--[[

    Extra ------------------------------------------------------------

]]--


Extra = {};
Extra.__index = Extra;

function getExtra(objId)
  local extra = {};
  setmetatable(extra, Extra);
  local items = getExtraC(objId);
  extra.id = getExtraUID( objId );
  extra.objId = objId;
  extra.orgProto = items[EXTRA_ORGPROTO];
  extra.orgX = items[EXTRA_ORGX];
  extra.orgY = items[EXTRA_ORGY];
  extra.hp = items[EXTRA_HP];
  extra.oldAim1 = items[EXTRA_OLDAIM1];
  extra.oldAim2 = items[EXTRA_OLDAIM2];
  extra.fallHeight = items[EXTRA_FALLHEIGHT];
  extra.lives = items[EXTRA_LIVES];
  extra.active = items[EXTRA_ACTIVE];
  extra.bulletCount = items[EXTRA_BULLETCOUNT];
  extra.owner = items[EXTRA_OWNER];
  extra.flags = items[EXTRA_FLAGS];
  extra.pathDir = items[EXTRA_PATHDIR];
  extra.pathPtr = items[EXTRA_PATHPTR];
  extra.pathDir = items[EXTRA_ORGPROTO];
  extra.animSeq = items[EXTRA_ANIMSEQ];
  extra.animPtr = items[EXTRA_ANIMPTR];
  extra.animSpeed = items[EXTRA_ANIMSPEED];
  extra.deathIndicator = items[EXTRA_DEATHINDICATOR];
  extra.age = items[EXTRA_AGE];
  return extra;
end

--[[

    Layer ------------------------------------------------------------

]]--

Layer = {};
Layer.__index = Layer;

function addLayer(name, type, ...)
  local arg = {...}
  local layer = {};
  setmetatable(layer, Layer);
  local id = addLayerC(name, type, unpack(arg));
  layer.id = id;
  layer.name = name;
  layer.type = type;
  return layer;
end

function getLayer( name )
  local id = getLayerC(name);
  local layer = {};
  setmetatable(layer, Layer);
  layer.id = id;
  layer.name = name;
  return layer;
end

function Layer:getSize()
    return layer_getSize(self.id);
end

function Layer:addElement(imageSet, imageNo, x, y, ...)
  local arg = {...}
  local id = addElement(self.id, imageSet, imageNo, x, y, unpack(arg));
  local e = {};
  setmetatable(e, Element);
  e.id = id;
  e.layerId = self.id;
  e.imageSet = imageSet;
  e.imageNo = imageNo;
  e.x = x;
  e.y = y;
  return e;
end

function Layer:getElement(id)
  local e = {};
  setmetatable(e, Element);
  e.layerId = self.id;
  e.id = id;
  e.imageSet, e.imageNo, e.x, e.y, e.show = layer_getElement(self.id, id);
  return e;
end

function Layer:clear()
  layer_clear( self.id );
end

function Layer:offsetX()
  return layer_getOffsetX( self.id );
end

function Layer:offsetY()
  return layer_getOffsetY( self.id );
end

function Layer:setOffsetX(x)
  layer_setOffsetX( self.id, x );
end

function Layer:setOffsetY(y)
  layer_setOffsetY( self.id, y );
end

function Layer:setSpeed(h, v)
  layer_setSpeed( self.id, h, v );
end

function Layer:delete()
  layer_delete( self.id);
end

function Layer:new(name, layerType, hSpeed, vSpeed)
  local layer = {};
  setmetatable(layer, Layer);
  layer.id = layer_new(name, layerType, hSpeed, vSpeed);
  layer.name = name;
  return layer;
end

function Layer:find(name)
    return getLayer(name);
end

--[[

    Element ----------------------------------------------------------

]]--

Element = {};
Element.__index = Element;

function Element:move(aim)
    return element_move(self.layerId, self.id, aim);
end

function Element:moveTo(x,y)
    self.x = x;
    self.y = y;
    return element_moveTo(self.layerId, self.id, x, y);
end

function Element:moveBy(tx, ty)
    self.x = self.x + tx;
    self.y = self.y + ty;
    return element_moveBy(self.layerId, self.id, tx, ty);
end

function Element:show()
    self.show = true;
    return element_setVisible(self.layerId, self.id, true);
end

function Element:hide()
    self.show = false;
    return element_setVisible(self.layerId, self.id, false);
end

function Element:setImage(imageSet, imageNo)
    self.imageSet = imageSet;
    self.imageNo = imageNo;
    return element_setImage(self.layerId, self.id, imageSet, imageNo);
end


--[[

    HitTest ----------------------------------------------------------

]]--

HitTest = {};
HitTest.__index = HitTest;

function getHitTest( objId, aim )
  local h = {};
  setmetatable(h, HitTest);
  local bk, fw, ac, flags, player = getHitTestC( objId, aim );

  h.bkCount = #bk;
  h.fwCount = (#fw)/2;
  h.acCount = (#ac)/2;
  h.player = player;
  h.flags = flags;
  h.bkClass = {};
  h.fwClass = {};
  h.acClass = {};
  h.fwEntry = {};
  h.acEntry = {};

  for i=1, h.bkCount do
    h.bkClass[i] = bk[i];
  end

  local j;
  j = 0;
  for i=1, h.fwCount do
    h.fwClass[i] = fw[j + 1];
    h.fwEntry[i] = fw[j + 2];
    j = j + 2;
  end

  j = 0;
  for i=1, h.acCount do
    h.acClass[i] = ac[j + 1];
    h.acEntry[i] = ac[j + 2];
    j = j + 2;
  end

  return h;
end

function HitTest:hasClass(classID)
  for i=1, h.bkCount do
    if h.bkClass[i] == classID then
        return true
    end
  end
  for i=1, h.fwCount do
    if h.fwClass[i] == classID then
        return true
    end
  end
  for i=1, h.acCount do
    if h.acClass[i] == classID then
        return true
    end
  end
  return false
end

function HitTest:hasPlayer()
  return h.player ~= 0
end

function HitTest:hasSprite(spriteID)
  for i=1, h.fwCount do
    if h.fwEntry[i] == spriteID then
        return true
    end
  end
  for i=1, h.acCount do
    if h.acEntry[i] == spriteID then
        return true
    end
  end
  return false
end




--[[

    Inventory ---------------------------------------------------------

]]--

Inventory = {};
Inventory.__index = Inventory;

function Inventory:addItem( protoId )
    addToInventory ( protoId ) ;
end

function Inventory:hasItem( protoId )
    return hasItem ( protoId ) ;
end

function Inventory:removeItem( protoId )
    removeFromInventory ( protoId ) ;
end

function Inventory:reset( )
    resetInventory ( ) ;
end

--[[

    Proto ------------------------------------------------------------

]]--

Proto = {};
Proto.__index = Proto;

function getProto(objType)
    local items = getProtoC(objType);
    local proto = {};
    proto.name = items[PPARAM_NAME];
    proto.noSmartMap = items[PPARAM_NO_SMART_MAP];
    proto.jumpMode = items[PPARAM_JUMP_MODE];
    proto.maxFall = items[PPARAM_MAX_FALL];
    proto.imageSet = items[PPARAM_FRAMESET];
    proto.imageNo = items[PPARAM_FRAMENO];
    proto.moveSpeed = items[PPARAM_MOVE_SPEED];
    proto.aniSpeed = items[PPARAM_ANI_SPEED];
    proto.defaultAim = items[PPARAM_DEFAULT_AIM];
    proto.points = items[PPARAM_POINTS];
    proto.buddy = items[PPARAM_BUDDY];
    proto.options = items[PPARAM_OPTIONS];
    proto.chProto = items[PPARAM_CHPROTO];
    proto.chSound = items[PPARAM_CHSOUND];
    proto.fallSpeed = items[PPARAM_FALLSPEED];
    proto.damages = items[PPARAM_DAMAGES];
    proto.bonusHP = items[PPARAM_BONUS_HP];
    proto.powerLevel = items[PPARAM_POWERLEVELS];
    proto.rebirthDelay = items[PPARAM_RBDELAY];
    proto.rebirths = items[PPARAM_REBIRTHS];
    proto.autoProto = items[PPARAM_A_PROTO];
    proto.autoBullet = items[PPARAM_A_BULLET];
    proto.autoSound = items[PPARAM_A_SOUND];
    proto.autoTrigger = items[PPARAM_A_TRIGGER];
    proto.hp = items[PPARAM_HP];
    proto.autoProtoTime = items[PPARAM_A_PROTO_TIME];
    proto.autoBulletTime = items[PPARAM_A_BULLET_TIME];
    proto.autoSoundTime = items[PPARAM_A_SOUND_TIME];
    proto.autoTriggerTime = items[PPARAM_A_TRIGGER_TIME];
    proto.maxBullets = items[PPARAM_MAX_BULLETS];
    proto.fireRate = items[PPARAM_FIRE_RATES];
    proto.extra1 = items[PPARAM_EXTRA1];
    proto.extra1 = items[PPARAM_EXTRA2];
    proto.bulletSound = items[PPARAM_B_SOUND];
    proto.coinBonus = items[PPARAM_COINS_BONUS];
    proto.livesBonus = items[PPARAM_LIVES_BONUS];
    proto.ammoBonus = items[PPARAM_AMMO_BONUS];
    proto.bulletOptions = items[PPARAM_BULLET_OPTIONS];
    return proto;
end

--[[

    Sprite -----------------------------------------------------------

]]--

Sprite = {};
Sprite.__index = Sprite;

function getPlayer()
    local id = getPlayerC ();
    return getSprite(id);
end

function copySprite(src)
    local id = copySpriteC(src.id);
    return getSprite(id);
end

function setSpriteAsGoal(id)
    markSpriteAsGoal(id);
end

function getSprite (...) 
    local sprite = {};
    setmetatable(sprite, Sprite);
    local arg = {...};
    if #arg == 1 then
        sprite.id = arg[1]; 
        sprite.x, sprite.y, sprite.aim, sprite.objType, sprite.imageSet, sprite.imageNo,
            sprite.triggerFlag, sprite.stateFlag, sprite.actionMask = getSpriteVars(unpack(arg));
        sprite.class = getObjClass(sprite.id);
        sprite.name = getObjName(sprite.id);
        sprite.extraUID = getExtraUID(sprite.id);
        return sprite;
    else
        Debug:printf("--getSprite() requires 1 argument");
        return nil;
    end
end

function Sprite:use(...)
    return getSprite(unpack(arg));
end

function addSprite(...)
  local arg = {...}
  local id = addSpriteC(unpack(arg));
  return getSprite(id);
end

function Sprite:new(...)
    return addSprite(unpack(arg));
end

function Sprite:setOwner(owner)
    sprite_setOwner(self.id, owner.id);
end

function Sprite:copy()
    return copySprite(self);
end

function Sprite:childCount()
    return sprite_childCount(self.id);
end

function Sprite:attackPlayer()
  return attackPlayer(self.id);
end

function Sprite:canFall()
  return canFall(self.id);
end

function Sprite:canMove(aim)
  return canMove(self.id, aim);
end

function Sprite:isPlayerThere(aim)
  return isPlayerThere(self.id, aim);
end

function Sprite:isVisible()
  return isVisible(self.id);
end

function Sprite:map()
  mapSprite(self.id);
end

function Sprite:moveBy(tx, ty)
    moveSpriteBy(self.id, tx, ty);
end

function Sprite:moveTo(x, y)
    moveSpriteTo(self.id, x, y);
end

function Sprite:move(...)
    local arg = {...};
    if #arg == 1 then
        moveSprite(self.id, arg[1]);
    elseif #arg == 0 then
        moveSprite(self.id, self.aim);
    else
        Debug:printf("--Sprite:move() requires 0 or 1 argument");        
    end
end

function Sprite:objClass()
  return getObjClass(self.id);
end

function Sprite:objName()
  return getObjName(self.id);
end

function Sprite:objType()
  return getObjType(self.id);
end

function Sprite:setAim(aim)
  self.aim = aim;
  setAim(self.id, aim);
end

function Sprite:setTriggerKey(key)
  setTriggerKey(self.id, key);
end

function Sprite:testFlags(mask)
  return testFlags(self.id, mask);
end

function Sprite:unMap()
  unMapSprite(self.id);
end

function Sprite:freeze()
  freezeSprite(self.id);
end

function Sprite:unFreeze()
  unFreezeSprite(self.id);
end

function Sprite:activate()
  activateSprite(self.id);
end

function Sprite:setAsGoal()
  markSpriteAsGoal(self.id);
end

function Sprite:unsetAsGoal()
  sprite_unmarkAsGoal(self.id);
end

function Sprite:unmarkAsGoal()
  sprite_unmarkAsGoal(self.id);
end

function Sprite:markAsGoal()
  markSpriteAsGoal(self.id);
end

function Sprite:hide()
  hideSprite(self.id);
end

function Sprite:show()
  showSprite(self.id);
end

function Sprite:tryAnimation(...)
    -- IN: int animSeq
    local arg = {...}
    if #arg == 1 then
        return tryAnimation(self.id, ...); 
    else
        Debug:printf("-- Sprite:tryAnimation requires 1 argument");
        return false;
    end
end

function Sprite:setImage(...)
    -- IN: int imageSet*
    -- IN: int imageNo
    local arg = {...}
    if #arg == 2 then
        self.imageNo = arg[2]; -- imageNo;
        self.imageSet = arg[1]; -- imageSet;
        setImage(self.id, unpack(arg)); 
    elseif #arg == 1 then
        self.imageNo = arg[1]; -- imageNo;
        setImage(self.id, self.imageSet, unpack(arg)); 
    else
        Debug:printf("--Sprite:setImage() requires one or two arguments");
    end 
end

function Sprite:setState(...)
    -- IN: int stateFlag
    -- IN: int flip*
    local arg = {...}
    if #arg == 2 then
        self.stateFlag = setState( self.id, unpack(arg) );
    elseif #arg == 1 then
        self.stateFlag = setState( self.id, unpack(arg), true );
    else 
        Debug:printf("--Sprite:setState() requires one or two arguments");    
    end
    
    return self.stateFlag;
end

function Sprite:getExtra()
    return getExtra(self.id);
end

function Sprite:getProto()
    return getProto(self.objType);
end

function Sprite:proto()
    return getProto(self.objType);
end

function Sprite:callEvent(event)
    return callObjEvent(self.id, event);
end

function Sprite:stopAnimation()
    return stopAnimation(self.id);
end

function Sprite:isMonster()
    return isMonster(self.id);
end

function Sprite:isPlayer()
    return sprite_isPlayer(self.id);
end

function Sprite:tryPath(...)
    -- IN: int path
    -- IN: int aim*
    -- OUT: bool
    local arg = {...}
    if #arg==1 or #arg==2 then
        return tryPath(self.id, unpack(arg));
    else
        Debug:printf("-- Sprite:tryPath() requires one or two arguments");
        return false;
    end
end

function Sprite:triggerHitState()
    return triggerHitState(self.id);
end

function Sprite:land()
    return land(self.id);
end

function Sprite:isHidden()
    return isHidden(self.id);
end

function Sprite:isDead()
    return isDead(self.id);
end

function Sprite:kill()
    return killSprite(self.id);
end

function Sprite:spawn()
    return spawn(self.id);
end

function Sprite:changeTo(...)
-- IN: int proto
-- OUT: void
    local arg = {...}
    if #arg == 1 then
        changeTo(self.id, arg[1] );
        self = getSprite(self.id);
    else
        Debug:printf("-- Sprite:changeTo() requires one argument");
    end
end

function Sprite:clear()
    clearSprite(self.id);
end

function Sprite:height()
    return sprite_getHeight(self.id);
end

function Sprite:width()
    return sprite_getWidth(self.id);
end

function Sprite:isActive()
    return sprite_isActive(self.id);
end

function Sprite:getString()
    return sprite_getString(self.id);
end

function Sprite:isFrozen()
    return sprite_isFrozen(self.id);
end

function Sprite:isGoal()
    return sprite_isGoal(self.id);
end

function Sprite:setHP(...)
    local arg = {...}
    if #arg == 1 then
        sprite_setHP(self.id, arg[1] );
    else
        Debug:printf("-- Sprite:setHP() requires one argument");
    end
end

function Sprite:getHP()
    return sprite_getHP(self.id);
end

function Sprite:frameCount()
	return sprite_frameCount(self.id);
end

function Sprite:hitTest(aim)
    return getHitTest(self.id, aim);
end

function Sprite:set(name, value)
    sprite_set(self.id, name, value);
end

function Sprite:get(name)
    return sprite_get(self.id, name);
end

function Sprite:addItem(protoID)
    return sprite_addItem(self.id, protoID);
end

function Sprite:hasItem(protoID)
    return sprite_hasItem(self.id, protoID);
end

function Sprite:resetInventory()
    return sprite_resetInventory(self.id);
end

function Sprite:removeItem(protoID)
    sprite_removeItem(self.id, protoID);
end

function Sprite:togglePathPlayback(enabled)
    sprite_togglePathPlayback(self.id, enabled);
end


--[[

    String -----------------------------------------------------------

]]--

function sprintf(fmt, ...)
  local arg = {...}
  return string.format(fmt, unpack(arg));
end

--[[

    System -----------------------------------------------------------

]]--

System = {};
System.__index = System;

function isPaused()
    return ss_getPause();
end

function firePlayerBullet(id, ticks)
    local sprite = getSprite( id );
    local proto = sprite:getProto();
    local ammo = Counters:get("ammo")
    if AND(proto.bulletOptions, BULLET_ENABLED) and
        ((proto.fireRate == 0 ) or (ticks % proto.fireRate == 0))
        and testJoyState( JOY_FIRE )
        and (ammo > 0 or AND(proto.bulletOptions, BULLET_UNLIMITED))
        then
        local x , y, aim = getSpriteVars( id );
        if testJoyState( JOY_LEFT ) then
            aim = LEFT;
            x = x - 8;
            y = y + 16;
        elseif testJoyState( JOY_RIGHT ) then
            aim = RIGHT;
            x = x + sprite:width();
            y = y + 16;
        elseif testJoyState( JOY_UP ) then
            aim = UP;
            x = x + 8;
            y = y - 8;
        elseif testJoyState( JOY_DOWN ) then
            aim = DOWN;
            x = x + 8;
            y = y + sprite:height();
        end
        local activeBullets = sprite:childCount();
        Counters:dec("ammo");
        if aim ~= HERE  and activeBullets < proto.maxBullets then
            local bullet = addSprite (
                x,
                y,
                aim,
                proto.buddy
                --spriteIdFromUuid("2b611fd2-0696-2065-2980-706e3f9539a3")
            );
            bullet:setOwner( sprite );
            if proto.bulletSound > 0 then
                playSound(proto.bulletSound);
            end
        end
    end
end

function unfoldEvents1()
    -- first check if there is a player object
    local ticks = getTicks();   
    local player_id = getPlayerC()
    if player_id == INVALID then
        return EVENT_NO_PLAYER;
    end

    -- if we got a closureEvent and time is elapsed
    -- return to caller    
    local closureEvent, closureTime = getClosure();
    local mtime = getTime();
    --if (closureEvent ~= EVENT_NO_EVENT and mtime >= closureTime) then
    if closureEvent ~= EVENT_NO_EVENT then
        if mtime >= closureTime then
            return closureEvent;
        else
            ss_notifyClosure();
        end
    end	

    -- perform pending tasks
    -- (e.g. spawn sprites)
    ss_manageTasks();

    -- autoCenter screen on player
    ss_autoCenter();
    
    -- if time for tick is ready do some stuff
    if mtime >= ss_getNextTick() then
        ss_setNextTick( mtime + getTickScale());
        -- check if the ESC or ALT-F4 was pressed
        local keyEvent = ss_manageKeyEvents();
        if keyEvent ~= EVENT_NO_EVENT then
            return keyEvent;
        end

        -- manage automatic elements
        local paused = isPaused()
        if not paused then
            ss_animate();
            if closureEvent ~= EVENT_LEVEL_COMPLETED
                and closureEvent ~= EVENT_TIMEOUT then
                ss_doManage();
                ss_managePlayer();
            end
            ss_manageAuto();
        end

        -- check if player is dead
        local player = getPlayer();
        local extra = getExtra(player_id);
        if closureEvent == EVENT_NO_EVENT
            and AND (player.stateFlag, STATE_DEAD) > 0
            and extra.deathIndicator >= DI_REMOVAL then
            -- empty keyboard buffer
            while ss_pollDevice() do
                ss_clearKeys();
            end
            -- close level
            setClosure ( EVENT_PLAYER_DIED );
        end

        -- test for `end of level`
        closureEvent = getClosureEvent();
        if closureEvent == EVENT_NO_EVENT and (isLevelEnded() or
            (ticks % 8 == 0  and isEndLevelMeet())) then
            snapshot_clear();
            setClosure( EVENT_LEVEL_COMPLETED );
        end

        -- test for timeout
        if mtime >= getNextSecond() then
            setNextSecond( mtime + 1000 );
            if (not paused and getTimeLeft() > 0) then
                decTimeLeft();
                if getTimeLeft() == 0 and closureEvent == EVENT_NO_EVENT then
                    callLvEvent( EL_TIMEOUT );
                    setClosure( EVENT_TIMEOUT );
                end
            end
        end

        -- update HP display
        updateHP();
        -- call the onHandler here to make things cleaner
        callLvEvent( EL_HANDLER );

        local sName = strv_get("loadGame");
        if sName ~= "" then
            strv_set("loadGame", "")
            loadGame(sName);
        end

        if var_get("reloadSnapshot") == 1 then
            var_set("reloadSnapshot", 0)
            snapshot_reload();
        end
        
        if ticks % 8 == 0 then
            countdown_cycle();
        end

        -- move to the tick
        nextTick();
    end

    return EVENT_NO_EVENT;
end
