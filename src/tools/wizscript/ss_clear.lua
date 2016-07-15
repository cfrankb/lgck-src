Script = {};
Script.__index = Script; 

function Script:load()
    -- get Wizard
    local wizID = findWidget("wizard");
    local wiz = QWizard:use(wizID);
    wiz:setWindowTitle("Script Wizard");

    -- create new page
    local wizPage = QWizardPage:new();
    wizPage:setSubTitle("Select Clear Color");
    -- wizPage:setTitle("hello world")

    -- create controls
    local label1 = QLabel:new("Color");
    self.btn = CWBtnColor:new();
    self.btn:setColor(0, 0, 0);
    local grid = QGridLayout:new();
    grid:addWidget(label1, 0,0);
    grid:addWidget(self.btn, 0,1)
    wizPage:setLayout(grid);
    self.pageId = wiz:addPage(wizPage);
    self.wiz = wiz;
    self.wizPage = wizPage;
end

function Script:unload()
    self.wiz:removePage(self.pageId);
    return 0;
end

function Script:save()
    local rgb = self.btn:color();
    local red, green, blue = rgb2tuple(rgb);
    return string.format("ss_clear(%d, %d, %d);\\n", red, green, blue);
end

-- run the onLoad automatically
script = {};
setmetatable(script, Script);
script:load();

