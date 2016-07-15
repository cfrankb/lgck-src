Script = {};
Script.__index = Script; 

function Script:load()
    -- get Wizard
    local wizID = findWidget("wizard");
    local wiz = QWizard:use(wizID);
    wiz:setWindowTitle("Script Wizard");

    -- create new page
    local wizPage = QWizardPage:new();
    wizPage:setSubTitle("Draw Text");
    -- wizPage:setTitle("hello world")

    -- create controls
    local label0 = QLabel:new("x");
    local label1 = QLabel:new("y");
    local label2 = QLabel:new("text");
    local label3 = QLabel:new("size");
    local label4 = QLabel:new("color");
    local label5 = QLabel:new("shadow offset");
    local label6 = QLabel:new("shadow color");
    self.edit0 = QLineEdit:new();
    self.edit0:setText('0')
    self.edit1 = QLineEdit:new();
    self.edit1:setText('0')
    self.edit2 = QLineEdit:new();
    self.edit2:setText('Hello World !!!');
    self.edit2:setPlaceholderText("put your text here");
    self.edit3 = QLineEdit:new();
    self.edit3:setText('10')
    self.edit5 = QLineEdit:new();
    self.edit5:setText('2')
    self.btnColor = CWBtnColor:new();
    self.btnColor:setColor(128, 240, 255);
    self.btnShadow = CWBtnColor:new();
    self.btnShadow:setColor(28, 20, 20);

    local grid = QGridLayout:new();
    grid:addWidget(label0, 0,0);
    grid:addWidget(label1, 1,0);
    grid:addWidget(label2, 2,0);
    grid:addWidget(label3, 3,0);
    grid:addWidget(label4, 4,0);
    grid:addWidget(label5, 5,0);
    grid:addWidget(label6, 6,0);
    grid:addWidget(self.edit0, 0,1);
    grid:addWidget(self.edit1, 1,1);
    grid:addWidget(self.edit2, 2,1);
    grid:addWidget(self.edit3, 3,1);
    grid:addWidget(self.edit5, 5,1);
    grid:addWidget(self.btnColor, 4,1)
    grid:addWidget(self.btnShadow, 6,1)

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
    -- TODO: add alpha
    local ttext = self.edit2:text();
    local tsize = tonumber(self.edit3:text());
    local y2 = tonumber(self.edit3:text());
    local shOffset = tonumber(self.edit5:text());
    local rgbaColor = self.btnColor:color() + 0xff000000;
    local rgbaShadow = self.btnShadow:color() + 0xff000000;
    return string.format('-- draw text\\nss_drawText(%d, %d, %q, %d, %d, 0x%x, %d, 0x%x);\\n',
        x1, y1, ttext, 0, tsize, rgbaColor, shOffset, rgbaShadow);
end

-- run the onLoad automatically
script = {};
setmetatable(script, Script);
script:load();

