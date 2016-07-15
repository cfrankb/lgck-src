local sprite = getSprite( self );
local proto = sprite:getProto();
if ((proto.fireRate == 0 ) or (ticks % proto.fireRate == 0))
    and (not sprite:isPlayer() or testJoyState( JOY_FIRE )) then
 
    local x , y, aim = getSpriteVars( self );
    if testJoyState( JOY_LEFT ) then 
        aim = LEFT;
    elseif testJoyState( JOY_RIGHT ) then
        aim = RIGHT;
    elseif testJoyState( JOY_UP ) then
        aim = UP;
    elseif testJoyState( JOY_DOWN ) then
        aim = DOWN;
    end 

    if aim == LEFT then 
        y = y + AND(sprite:height()/2, 0xfff8);
    elseif aim == RIGHT then
        x = x + sprite:width();
        y = y + AND(sprite:height()/2, 0xfff8);
    elseif aim == UP then
        x = x + AND(sprite:width()/2, 0xfff8);
    elseif aim == DOWN then
        x = x + AND(sprite:width()/2, 0xfff8);
        y = y + sprite:height(); 
    end 

    local activeBullets = sprite:childCount();
    if aim ~= HERE and proto.buddy ~= 0 and activeBullets < proto.maxBullets then
        local bullet = addSprite(
            x, 
            y, 
            aim,
            proto.buddy
        );

        if aim == LEFT then
            bullet:moveBy(- bullet:width(), 0)
        elseif aim == UP then
            bullet:moveBy(0, - bullet:height())
        end if
        bullet:map();
        bullet:setOwner( sprite );

        if proto.bulletSound > 0 then
            playSound(proto.bulletSound);
        end        
    end
end
