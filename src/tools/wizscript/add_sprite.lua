Script = {};
Script.__index = Script; 

function Script:load()
    -- get Wizard
    local wizID = findWidget("wizard");
    local wiz = CWizScript:use(wizID);
    wiz:setWindowTitle("Script Wizard");

    -- create new page
    local wizPage = QWizardPage:new();
    wizPage:setSubTitle("Add New Sprite");
    -- wizPage:setTitle("hello world")

    -- create controls
    local label0 = QLabel:new("x");
    local label1 = QLabel:new("y");
    local label2 = QLabel:new("aim");
    local label3 = QLabel:new("sprite");
    self.edit0 = QLineEdit:new();
    self.edit0:setText('0')
    self.edit1 = QLineEdit:new();
    self.edit1:setText('0')
    self.combo2 = QComboBox:new();
    local aims = {'UP', 'DOWN', 'LEFT', 'RIGHT'}
    local i;
    for i=1, #aims do
        self.combo2:addItem(aims[i]);
    end
    self.combo3 = QComboBox:new();
    wiz:initComboBox(self.combo3, "sprite");
    local grid = QGridLayout:new();
    grid:addWidget(label0, 0,0);
    grid:addWidget(label1, 1,0);
    grid:addWidget(label2, 2,0);
    grid:addWidget(label3, 3,0);
    grid:addWidget(self.edit0, 0,1);
    grid:addWidget(self.edit1, 1,1);
    grid:addWidget(self.combo2, 2,1);
    grid:addWidget(self.combo3, 3,1);

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
    local x = tonumber(self.edit0:text());
    local y = tonumber(self.edit1:text());
    local aim = self.combo2:currentText();
    local chSprite = self.combo3:currentText();
    return string.format('-- add sprite\\nlocal newSprite = Sprite:new(%d, %d, %s, %s);\\n',
        x, y, aim, chSprite);
end

-- run the onLoad automatically
script = {};
setmetatable(script, Script);
script:load();

