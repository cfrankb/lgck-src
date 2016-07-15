local s = getSprite(self)
if s:get("waypoint") ~= 1 then
	s:set("waypoint", 1);
    -- You can modify the following line to change the image 
    -- s:setImage( OR(s.imageNo,2) );
	snapshot_take();
end
