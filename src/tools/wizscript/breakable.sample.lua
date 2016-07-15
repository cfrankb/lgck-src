local sprite = getSprite( self );
local image = sprite.imageNo;
if sprite:isPlayerThere( UP ) then
	sprite:set("touched", 1);
end
if (sprite:get("touched") ~= 0 or image > 3) and ticks % 3 == 0 then
	image = image + 1
	sprite:unMap();
	if image < sprite:frameCount() then
		sprite:setImage( sprite.imageSet, image);
	else
		sprite:hide();
	end
	sprite:map();
	sprite:set("touched", 0);
end 
