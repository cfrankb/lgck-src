Script = {};
Script.__index = Script; 

function Script:load()
    -- get Wizard
    local wizID = findWidget("wizard");
    local wiz = CWizScript:use(wizID);
    --wiz:setWindowTitle("Script Wizard");

    -- create new page
    local wizPage = QWizardPage:new();
    wizPage:setSubTitle("Select ImageSet");
    -- wizPage:setTitle("hello world")

    -- create controls
    local label1 = QLabel:new("ImageSet");
    self.combo1 = QComboBox:new();
    wiz:initComboBox(self.combo1, "image");
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
    local imageSet = self.combo1:currentText();
    return string.format("local frames = frameSet_getSize(%s);\\n", imageSet);
end

-- run the onLoad automatically
script = {};
setmetatable(script, Script);
script:load();

