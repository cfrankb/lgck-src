Script = {};
Script.__index = Script; 

function Script:load()
    -- get Wizard
    local wizID = findWidget("wizard");
    local wiz = CWizScript:use(wizID);
    --wiz:setWindowTitle("Script Wizard");

    -- create new page
    local wizPage = QWizardPage:new();
    wizPage:setSubTitle("Add To Score");
    -- wizPage:setTitle("hello world")

    -- create controls
    local label1 = QLabel:new("Points");
    self.combo1 = QComboBox:new();
    local points = {
        0,
        10,
        25,
        50,
        100,
        200,
        300,
        400,
        500,
        1000,
        2000,
        5000,
        10000
    };
    local i;
    for i=1, #points do
        local pts = string.format("%d", points[i])
        self.combo1:addItem(pts)
    end
    --wiz:initComboBox(self.combo1, "sprite");
    local grid = QGridLayout:new();
    grid:addWidget(label1, 0,0);
    grid:addWidget(self.combo1, 0,1)
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
    local pts = self.combo1:currentText();
    return string.format("addToScore(%s);\\n", pts);
end

-- run the onLoad automatically
script = {};
setmetatable(script, Script);
script:load();

