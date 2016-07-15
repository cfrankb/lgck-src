Script = {};
Script.__index = Script; 

function Script:load()
    self.wiz = 123;
end

function Script:unload()
    --self.wiz:removePage(self.wizPage);
    return 0;
end

function Script:save()
end

-- run onLoad
script = {};
setmetatable(script, Script);
script:load();
 
