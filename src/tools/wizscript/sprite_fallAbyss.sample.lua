-- add this to event OnLanding
local s = getSprite(self);
local sx, sy = getImageSize(s.imageSet, s.imageNo);
if s.y + sy >= var_get("BUFFERHEI") then
  s:setState(STATE_DEAD);
  s:hide();
else
  -- add code here
  -- playSound("POUFLIGHT");
  local extra = getExtra(self);
  if extra.fallHeight >= 20 then
    -- uncomment these lines if needed
    -- triggerPlayerHitState();
    -- addToHP( -25 );
  end
end
