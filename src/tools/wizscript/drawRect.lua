Script = {};
Script.__index = Script; 

function Script:load()
    -- get Wizard
    local wizID = findWidget("wizard");
    local wiz = QWizard:use(wizID);
    wiz:setWindowTitle("Script Wizard");

    -- create new page
    local wizPage = QWizardPage:new();
    wizPage:setSubTitle("Draw Rectangle");
    -- wizPage:setTitle("hello world")

    -- create controls
    local label0 = QLabel:new("x1");
    local label1 = QLabel:new("y1");
    local label2 = QLabel:new("x2");
    local label3 = QLabel:new("y2");
    self.edit0 = QLineEdit:new();
    self.edit0:setText('0')
    self.edit1 = QLineEdit:new();
    self.edit1:setText('0')
    self.edit2 = QLineEdit:new();
    self.edit2:setText('100')
    self.edit3 = QLineEdit:new();
    self.edit3:setText('100')

    local label4 = QLabel:new("Rectangle Color");
    self.btn = CWBtnColor:new();
    self.btn:setColor(128, 240, 255);

    self.ckFill = QCheckBox:new("Filled");
    self.ckFill:setChecked(true);

    local grid = QGridLayout:new();
    grid:addWidget(label0, 0,0);
    grid:addWidget(label1, 1,0);
    grid:addWidget(label2, 2,0);
    grid:addWidget(label3, 3,0);
    grid:addWidget(self.edit0, 0,1);
    grid:addWidget(self.edit1, 1,1);
    grid:addWidget(self.edit2, 2,1);
    grid:addWidget(self.edit3, 3,1);
    grid:addWidget(label4, 4,0);
    grid:addWidget(self.btn, 4,1)
    grid:addWidget(self.ckFill, 5,1)

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
    local x1 = tonumber(self.edit0:text());
    local y1 = tonumber(self.edit1:text());
    local x2 = tonumber(self.edit2:text());
    local y2 = tonumber(self.edit3:text());
    local rgba = self.btn:color() + 0xff000000;
    local fill = 'false';
    if self.ckFill:isChecked() then
        fill = 'true';
    end
    return string.format("--draw rectangle\\nss_paint(%d, %d, %d, %d, 0x%x, %s);\\n", x1, y1, x2,y2, rgba, fill);
end

-- run the onLoad automatically
script = {};
setmetatable(script, Script);
script:load();

