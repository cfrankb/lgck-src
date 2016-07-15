-- this script will decide which fall sound to play.,
-- add this to the onFall event
local s = getSprite( self );
local tmp = copySprite ( s );
local fall = 0;
while tmp:canFall() do
  tmp:move( DOWN );
  fall = fall + 1;
end
tmp:clear();
if fall < 20 then
  -- add action here
  -- playSound("short fall.ogg");
else
  -- add action here
  -- playSound("fall.ogg");
end 
