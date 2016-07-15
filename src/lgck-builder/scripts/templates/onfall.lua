local s = getSprite( self );
local tmp = copySprite ( s );

local fall = 0;
while tmp:canFall() do
  tmp:move( DOWN );
  fall = fall + 1;
end

tmp:clear();

if fall < 20 then
  playSound("short fall.ogg");
else
  playSound("fall.ogg");
end

