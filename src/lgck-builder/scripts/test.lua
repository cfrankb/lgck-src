-- get Wizard
local wizID = findWidget("wizard");
local wiz = QWizard:use(wizID);
wiz:setWindowTitle("Script Wizard");

-- create new page
local wizPage = QWizardPage:new();
wizPage:setSubTitle("4343443");
wizPage:setTitle("hello world")

local label1 = QLabel:new("blah");
local label2 = QLabel:new("wee");
label2:setText('hello world');
local edit = QLineEdit:new();
edit:setPlaceholderText("foo");
local grid = QGridLayout:new();
local combo = QComboBox:new();
combo:addItem("abc");
combo:addItem("123");
combo:addItem("456");
combo:addItem("cat");
combo:addItem("dog");
combo:setCurrentIndex(3);

local ck_allo = QCheckBox:new("momo");
ck_allo:setText("allo");
local ck_toto = QCheckBox:new("toto");
ck_toto:setChecked(true);

local bg_group1 = QButtonGroup:new();
bg_group1:setExclusive(true);
bg_group1:addButton(ck_allo,1);
bg_group1:addButton(ck_toto,2);
bindWidget("group1", bg_group1.id);

grid:addWidget(label1, 0,0);
grid:addWidget(edit, 0,1)
grid:addWidget(label2, 1,0);
grid:addWidget(combo, 1,1)
grid:addWidget(ck_allo, 2,1)
grid:addWidget(ck_toto, 3,1)
wizPage:setLayout(grid);
wiz:addPage(wizPage);
-- wiz:setStartId(1);

