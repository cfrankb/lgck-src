--[[
    LGCK Builder Runtime
    Copyright (C) 1999, 2015  Francois Blanchette

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
]]--
-- auto-generated
CWBtnColor = {};
CWBtnColor.__index = CWBtnColor;

function CWBtnColor:color()
    return cwbtncolor_color(self.id);
end

function CWBtnColor:delete()
    cwbtncolor_delete(self.id);
    self.id = 0;end

function CWBtnColor:new(...)
   local q = {};
   setmetatable(q, CWBtnColor);
   if arg ~= nil then
      q.id = cwbtncolor_new(unpack(arg))
   else
      q.id = cwbtncolor_new()
   end
   return q
end

function CWBtnColor:setColor(arg0,arg1,arg2)
    cwbtncolor_setColor(self.id,arg0,arg1,arg2);
end

function CWBtnColor:use(id)
   local q = {};
   setmetatable(q, CWBtnColor);
   q.id = id;
   return q
end

CWizScript = {};
CWizScript.__index = CWizScript;

function CWizScript:accept()
    cwizscript_accept(self.id);
end

function CWizScript:addPage(arg0)
    return cwizscript_addPage(self.id,arg0.id);
end

function CWizScript:back()
    cwizscript_back(self.id);
end

function CWizScript:buttonText(arg0)
    return cwizscript_buttonText(self.id,arg0);
end

function CWizScript:currentId()
    return cwizscript_currentId(self.id);
end

function CWizScript:delete()
    cwizscript_delete(self.id);
    self.id = 0;end

function CWizScript:initComboBox(arg0,arg1)
    cwizscript_initComboBox(self.id,arg0.id,arg1);
end

function CWizScript:new(...)
   local q = {};
   setmetatable(q, CWizScript);
   if arg ~= nil then
      q.id = cwizscript_new(unpack(arg))
   else
      q.id = cwizscript_new()
   end
   return q
end

function CWizScript:next()
    cwizscript_next(self.id);
end

function CWizScript:reject()
    cwizscript_reject(self.id);
end

function CWizScript:removePage(arg0)
    cwizscript_removePage(self.id,arg0);
end

function CWizScript:restart()
    cwizscript_restart(self.id);
end

function CWizScript:setButton(arg0,arg1)
    cwizscript_setButton(self.id,arg0,arg1.id);
end

function CWizScript:setButtonText(arg0,arg1)
    cwizscript_setButtonText(self.id,arg0,arg1);
end

function CWizScript:setDefaultProperty(arg0,arg1,arg2)
    cwizscript_setDefaultProperty(self.id,arg0,arg1,arg2);
end

function CWizScript:setField(arg0,arg1)
    cwizscript_setField(self.id,arg0,arg1);
end

function CWizScript:setOption(arg0,...)
    if arg == nil then
        arg = {}
    end
    cwizscript_setOption(self.id,arg0,unpack(arg));
end

function CWizScript:setPage(arg0,arg1)
    cwizscript_setPage(self.id,arg0,arg1.id);
end

function CWizScript:setSideWidget(arg0)
    cwizscript_setSideWidget(self.id,arg0.id);
end

function CWizScript:setStartId(arg0)
    cwizscript_setStartId(self.id,arg0);
end

function CWizScript:setSubTitleFormat(arg0)
    cwizscript_setSubTitleFormat(self.id,arg0);
end

function CWizScript:setTitleFormat(arg0)
    cwizscript_setTitleFormat(self.id,arg0);
end

function CWizScript:setVisible(arg0)
    cwizscript_setVisible(self.id,arg0);
end

function CWizScript:setWindowTitle(arg0)
    cwizscript_setWindowTitle(self.id,arg0);
end

function CWizScript:setWizardStyle(arg0)
    cwizscript_setWizardStyle(self.id,arg0);
end

function CWizScript:startId()
    return cwizscript_startId(self.id);
end

function CWizScript:subTitleFormat()
    return cwizscript_subTitleFormat(self.id);
end

function CWizScript:testOption(arg0)
    cwizscript_testOption(self.id,arg0);
end

function CWizScript:titleFormat()
    return cwizscript_titleFormat(self.id);
end

function CWizScript:use(id)
   local q = {};
   setmetatable(q, CWizScript);
   q.id = id;
   return q
end

function CWizScript:validateCurrentPage()
    return cwizscript_validateCurrentPage(self.id);
end

function CWizScript:wizardStyle()
    return cwizscript_wizardStyle(self.id);
end

QButtonGroup = {};
QButtonGroup.__index = QButtonGroup;

function QButtonGroup:addButton(arg0,arg1)
    qbuttongroup_addButton(self.id,arg0.id,arg1);
end

function QButtonGroup:delete()
    qbuttongroup_delete(self.id);
    self.id = 0;end

function QButtonGroup:exclusive()
    return qbuttongroup_exclusive(self.id);
end

function QButtonGroup:id(arg0)
    return qbuttongroup_id(self.id,arg0.id);
end

function QButtonGroup:new(...)
   local q = {};
   setmetatable(q, QButtonGroup);
   if arg ~= nil then
      q.id = qbuttongroup_new(unpack(arg))
   else
      q.id = qbuttongroup_new()
   end
   return q
end

function QButtonGroup:removeButton(arg0)
    qbuttongroup_removeButton(self.id,arg0.id);
end

function QButtonGroup:setExclusive(arg0)
    qbuttongroup_setExclusive(self.id,arg0);
end

function QButtonGroup:setId(arg0,arg1)
    qbuttongroup_setId(self.id,arg0.id,arg1);
end

function QButtonGroup:use(id)
   local q = {};
   setmetatable(q, QButtonGroup);
   q.id = id;
   return q
end

QCheckBox = {};
QCheckBox.__index = QCheckBox;

function QCheckBox:autoExclusive()
    return qcheckbox_autoExclusive(self.id);
end

function QCheckBox:checkState()
    return qcheckbox_checkState(self.id);
end

function QCheckBox:delete()
    qcheckbox_delete(self.id);
    self.id = 0;end

function QCheckBox:isCheckable()
    return qcheckbox_isCheckable(self.id);
end

function QCheckBox:isChecked()
    return qcheckbox_isChecked(self.id);
end

function QCheckBox:isTristate()
    return qcheckbox_isTristate(self.id);
end

function QCheckBox:new(...)
   local q = {};
   setmetatable(q, QCheckBox);
   if arg ~= nil then
      q.id = qcheckbox_new(unpack(arg))
   else
      q.id = qcheckbox_new()
   end
   return q
end

function QCheckBox:setAutoExclusive(arg0)
    qcheckbox_setAutoExclusive(self.id,arg0);
end

function QCheckBox:setCheckable(arg0)
    qcheckbox_setCheckable(self.id,arg0);
end

function QCheckBox:setChecked(arg0)
    qcheckbox_setChecked(self.id,arg0);
end

function QCheckBox:setText(arg0)
    qcheckbox_setText(self.id,arg0);
end

function QCheckBox:setTristate(...)
    if arg == nil then
        arg = {}
    end
    qcheckbox_setTristate(self.id,unpack(arg));
end

function QCheckBox:text()
    return qcheckbox_text(self.id);
end

function QCheckBox:use(id)
   local q = {};
   setmetatable(q, QCheckBox);
   q.id = id;
   return q
end

QComboBox = {};
QComboBox.__index = QComboBox;

function QComboBox:addItem(arg0)
    qcombobox_addItem(self.id,arg0);
end

function QComboBox:count()
    return qcombobox_count(self.id);
end

function QComboBox:currentIndex()
    return qcombobox_currentIndex(self.id);
end

function QComboBox:currentText()
    return qcombobox_currentText(self.id);
end

function QComboBox:delete()
    qcombobox_delete(self.id);
    self.id = 0;end

function QComboBox:duplicatesEnabled()
    return qcombobox_duplicatesEnabled(self.id);
end

function QComboBox:hasFrame()
    return qcombobox_hasFrame(self.id);
end

function QComboBox:hidePopup()
    qcombobox_hidePopup(self.id);
end

function QComboBox:insertItem(arg0,arg1)
    qcombobox_insertItem(self.id,arg0,arg1);
end

function QComboBox:insertPolicy()
    return qcombobox_insertPolicy(self.id);
end

function QComboBox:insertSeparator(arg0)
    qcombobox_insertSeparator(self.id,arg0);
end

function QComboBox:isEditable()
    return qcombobox_isEditable(self.id);
end

function QComboBox:itemText(arg0)
    return qcombobox_itemText(self.id,arg0);
end

function QComboBox:maxCount()
    return qcombobox_maxCount(self.id);
end

function QComboBox:maxVisibleItems()
    return qcombobox_maxVisibleItems(self.id);
end

function QComboBox:minimumContentsLength()
    return qcombobox_minimumContentsLength(self.id);
end

function QComboBox:modelColumn()
    return qcombobox_modelColumn(self.id);
end

function QComboBox:new(...)
   local q = {};
   setmetatable(q, QComboBox);
   if arg ~= nil then
      q.id = qcombobox_new(unpack(arg))
   else
      q.id = qcombobox_new()
   end
   return q
end

function QComboBox:removeItem(arg0)
    qcombobox_removeItem(self.id,arg0);
end

function QComboBox:setCurrentIndex(arg0)
    qcombobox_setCurrentIndex(self.id,arg0);
end

function QComboBox:setDuplicatesEnabled(arg0)
    qcombobox_setDuplicatesEnabled(self.id,arg0);
end

function QComboBox:setFrame(arg0)
    qcombobox_setFrame(self.id,arg0);
end

function QComboBox:use(id)
   local q = {};
   setmetatable(q, QComboBox);
   q.id = id;
   return q
end

QGridLayout = {};
QGridLayout.__index = QGridLayout;

function QGridLayout:addWidget(arg0,arg1,arg2,...)
    if arg == nil then
        arg = {}
    end
    qgridlayout_addWidget(self.id,arg0.id,arg1,arg2,unpack(arg));
end

function QGridLayout:delete()
    qgridlayout_delete(self.id);
    self.id = 0;end

function QGridLayout:new(...)
   local q = {};
   setmetatable(q, QGridLayout);
   if arg ~= nil then
      q.id = qgridlayout_new(unpack(arg))
   else
      q.id = qgridlayout_new()
   end
   return q
end

function QGridLayout:use(id)
   local q = {};
   setmetatable(q, QGridLayout);
   q.id = id;
   return q
end

QLabel = {};
QLabel.__index = QLabel;

function QLabel:alignment()
    return qlabel_alignment(self.id);
end

function QLabel:clear()
    qlabel_clear(self.id);
end

function QLabel:delete()
    qlabel_delete(self.id);
    self.id = 0;end

function QLabel:hasScaledContents()
    return qlabel_hasScaledContents(self.id);
end

function QLabel:hasSelectedText()
    return qlabel_hasSelectedText(self.id);
end

function QLabel:indent()
    return qlabel_indent(self.id);
end

function QLabel:margin()
    return qlabel_margin(self.id);
end

function QLabel:new(...)
   local q = {};
   setmetatable(q, QLabel);
   if arg ~= nil then
      q.id = qlabel_new(unpack(arg))
   else
      q.id = qlabel_new()
   end
   return q
end

function QLabel:openExternalLinks()
    return qlabel_openExternalLinks(self.id);
end

function QLabel:selectedText()
    return qlabel_selectedText(self.id);
end

function QLabel:selectionStart()
    return qlabel_selectionStart(self.id);
end

function QLabel:setAlignment(arg0)
    qlabel_setAlignment(self.id,arg0);
end

function QLabel:setBuddy(arg0)
    qlabel_setBuddy(self.id,arg0.id);
end

function QLabel:setFrameStyle(arg0)
    qlabel_setFrameStyle(self.id,arg0);
end

function QLabel:setIndent(arg0)
    qlabel_setIndent(self.id,arg0);
end

function QLabel:setMargin(arg0)
    qlabel_setMargin(self.id,arg0);
end

function QLabel:setNum(arg0)
    qlabel_setNum(self.id,arg0);
end

function QLabel:setOpenExternalLinks(arg0)
    qlabel_setOpenExternalLinks(self.id,arg0);
end

function QLabel:setScaledContents(arg0)
    qlabel_setScaledContents(self.id,arg0);
end

function QLabel:setText(arg0)
    qlabel_setText(self.id,arg0);
end

function QLabel:setTextFormat(arg0)
    qlabel_setTextFormat(self.id,arg0);
end

function QLabel:setTextInteractionFlags(arg0)
    qlabel_setTextInteractionFlags(self.id,arg0);
end

function QLabel:setWordWrap(arg0)
    qlabel_setWordWrap(self.id,arg0);
end

function QLabel:text()
    return qlabel_text(self.id);
end

function QLabel:textFormat()
    return qlabel_textFormat(self.id);
end

function QLabel:textInteractionFlags()
    return qlabel_textInteractionFlags(self.id);
end

function QLabel:use(id)
   local q = {};
   setmetatable(q, QLabel);
   q.id = id;
   return q
end

function QLabel:wordWrap()
    return qlabel_wordWrap(self.id);
end

QLineEdit = {};
QLineEdit.__index = QLineEdit;

function QLineEdit:alignment()
    return qlineedit_alignment(self.id);
end

function QLineEdit:backspace()
    qlineedit_backspace(self.id);
end

function QLineEdit:copy()
    qlineedit_copy(self.id);
end

function QLineEdit:cursorBackward(arg0,...)
    if arg == nil then
        arg = {}
    end
    qlineedit_cursorBackward(self.id,arg0,unpack(arg));
end

function QLineEdit:cursorForward(arg0,...)
    if arg == nil then
        arg = {}
    end
    qlineedit_cursorForward(self.id,arg0,unpack(arg));
end

function QLineEdit:cursorMoveStyle()
    return qlineedit_cursorMoveStyle(self.id);
end

function QLineEdit:cursorPosition()
    return qlineedit_cursorPosition(self.id);
end

function QLineEdit:cut()
    qlineedit_cut(self.id);
end

function QLineEdit:delete()
    qlineedit_delete(self.id);
    self.id = 0;end

function QLineEdit:deselect()
    qlineedit_deselect(self.id);
end

function QLineEdit:displayText()
    return qlineedit_displayText(self.id);
end

function QLineEdit:endl(arg0)
    qlineedit_end(self.id,arg0);
end

function QLineEdit:hasSelectedText()
    return qlineedit_hasSelectedText(self.id);
end

function QLineEdit:home(arg0)
    qlineedit_home(self.id,arg0);
end

function QLineEdit:inputMask()
    return qlineedit_inputMask(self.id);
end

function QLineEdit:insert(arg0)
    qlineedit_insert(self.id,arg0);
end

function QLineEdit:isModified()
    return qlineedit_isModified(self.id);
end

function QLineEdit:isReadOnly()
    return qlineedit_isReadOnly(self.id);
end

function QLineEdit:isRedoAvailable()
    return qlineedit_isRedoAvailable(self.id);
end

function QLineEdit:isUndoAvailable()
    return qlineedit_isUndoAvailable(self.id);
end

function QLineEdit:maxLength()
    return qlineedit_maxLength(self.id);
end

function QLineEdit:new(...)
   local q = {};
   setmetatable(q, QLineEdit);
   if arg ~= nil then
      q.id = qlineedit_new(unpack(arg))
   else
      q.id = qlineedit_new()
   end
   return q
end

function QLineEdit:paste()
    qlineedit_paste(self.id);
end

function QLineEdit:placeholderText()
    return qlineedit_placeholderText(self.id);
end

function QLineEdit:redo()
    qlineedit_redo(self.id);
end

function QLineEdit:selectAll()
    qlineedit_selectAll(self.id);
end

function QLineEdit:setAlignment(arg0)
    qlineedit_setAlignment(self.id,arg0);
end

function QLineEdit:setCursorMoveStyle(arg0)
    qlineedit_setCursorMoveStyle(self.id,arg0);
end

function QLineEdit:setCursorPosition(arg0)
    qlineedit_setCursorPosition(self.id,arg0);
end

function QLineEdit:setInputMask(arg0)
    qlineedit_setInputMask(self.id,arg0);
end

function QLineEdit:setMaxLength(arg0)
    qlineedit_setMaxLength(self.id,arg0);
end

function QLineEdit:setModified(arg0)
    qlineedit_setModified(self.id,arg0);
end

function QLineEdit:setPlaceholderText(arg0)
    qlineedit_setPlaceholderText(self.id,arg0);
end

function QLineEdit:setReadOnly(arg0)
    qlineedit_setReadOnly(self.id,arg0);
end

function QLineEdit:setSelection(arg0,arg1)
    qlineedit_setSelection(self.id,arg0,arg1);
end

function QLineEdit:setText(arg0)
    qlineedit_setText(self.id,arg0);
end

function QLineEdit:setTextMargins(arg0,arg1,arg2,arg3)
    qlineedit_setTextMargins(self.id,arg0,arg1,arg2,arg3);
end

function QLineEdit:text()
    return qlineedit_text(self.id);
end

function QLineEdit:undo()
    qlineedit_undo(self.id);
end

function QLineEdit:use(id)
   local q = {};
   setmetatable(q, QLineEdit);
   q.id = id;
   return q
end

QListWidget = {};
QListWidget.__index = QListWidget;

function QListWidget:addItem(arg0)
    qlistwidget_addItem(self.id,arg0);
end

function QListWidget:count()
    return qlistwidget_count(self.id);
end

function QListWidget:currentRow()
    return qlistwidget_currentRow(self.id);
end

function QListWidget:delete()
    qlistwidget_delete(self.id);
    self.id = 0;end

function QListWidget:insertItem(arg0,arg1)
    qlistwidget_insertItem(self.id,arg0,arg1);
end

function QListWidget:isSortingEnabled()
    return qlistwidget_isSortingEnabled(self.id);
end

function QListWidget:new(...)
   local q = {};
   setmetatable(q, QListWidget);
   if arg ~= nil then
      q.id = qlistwidget_new(unpack(arg))
   else
      q.id = qlistwidget_new()
   end
   return q
end

function QListWidget:setCurrentRow(arg0)
    qlistwidget_setCurrentRow(self.id,arg0);
end

function QListWidget:setSortingEnabled(arg0)
    qlistwidget_setSortingEnabled(self.id,arg0);
end

function QListWidget:sortItems(arg0)
    qlistwidget_sortItems(self.id,arg0);
end

function QListWidget:use(id)
   local q = {};
   setmetatable(q, QListWidget);
   q.id = id;
   return q
end

QRadioButton = {};
QRadioButton.__index = QRadioButton;

function QRadioButton:autoExclusive()
    return qradiobutton_autoExclusive(self.id);
end

function QRadioButton:delete()
    qradiobutton_delete(self.id);
    self.id = 0;end

function QRadioButton:isCheckable()
    return qradiobutton_isCheckable(self.id);
end

function QRadioButton:isChecked()
    return qradiobutton_isChecked(self.id);
end

function QRadioButton:new(...)
   local q = {};
   setmetatable(q, QRadioButton);
   if arg ~= nil then
      q.id = qradiobutton_new(unpack(arg))
   else
      q.id = qradiobutton_new()
   end
   return q
end

function QRadioButton:setAutoExclusive(arg0)
    qradiobutton_setAutoExclusive(self.id,arg0);
end

function QRadioButton:setCheckable(arg0)
    qradiobutton_setCheckable(self.id,arg0);
end

function QRadioButton:setChecked(arg0)
    qradiobutton_setChecked(self.id,arg0);
end

function QRadioButton:setText(arg0)
    qradiobutton_setText(self.id,arg0);
end

function QRadioButton:text()
    return qradiobutton_text(self.id);
end

function QRadioButton:use(id)
   local q = {};
   setmetatable(q, QRadioButton);
   q.id = id;
   return q
end

QSlider = {};
QSlider.__index = QSlider;

function QSlider:delete()
    qslider_delete(self.id);
    self.id = 0;end

function QSlider:hasTracking()
    return qslider_hasTracking(self.id);
end

function QSlider:invertedAppearance()
    return qslider_invertedAppearance(self.id);
end

function QSlider:invertedControls()
    return qslider_invertedControls(self.id);
end

function QSlider:isSliderDown()
    return qslider_isSliderDown(self.id);
end

function QSlider:maximum()
    return qslider_maximum(self.id);
end

function QSlider:minimum()
    return qslider_minimum(self.id);
end

function QSlider:new(...)
   local q = {};
   setmetatable(q, QSlider);
   if arg ~= nil then
      q.id = qslider_new(unpack(arg))
   else
      q.id = qslider_new()
   end
   return q
end

function QSlider:orientation()
    return qslider_orientation(self.id);
end

function QSlider:pageStep()
    return qslider_pageStep(self.id);
end

function QSlider:setInvertedAppearance(arg0)
    qslider_setInvertedAppearance(self.id,arg0);
end

function QSlider:setInvertedControls(arg0)
    qslider_setInvertedControls(self.id,arg0);
end

function QSlider:setMaximum(arg0)
    qslider_setMaximum(self.id,arg0);
end

function QSlider:setMinimum(arg0)
    qslider_setMinimum(self.id,arg0);
end

function QSlider:setPageStep(arg0)
    qslider_setPageStep(self.id,arg0);
end

function QSlider:setRange(arg0,arg1)
    qslider_setRange(self.id,arg0,arg1);
end

function QSlider:setSingleStep(arg0)
    qslider_setSingleStep(self.id,arg0);
end

function QSlider:setSliderDown(arg0)
    qslider_setSliderDown(self.id,arg0);
end

function QSlider:setSliderPosition(arg0)
    qslider_setSliderPosition(self.id,arg0);
end

function QSlider:setTickInterval(arg0)
    qslider_setTickInterval(self.id,arg0);
end

function QSlider:setTickPosition(arg0)
    qslider_setTickPosition(self.id,arg0);
end

function QSlider:setTracking(arg0)
    qslider_setTracking(self.id,arg0);
end

function QSlider:singleStep()
    return qslider_singleStep(self.id);
end

function QSlider:sliderPosition()
    return qslider_sliderPosition(self.id);
end

function QSlider:tickInterval()
    return qslider_tickInterval(self.id);
end

function QSlider:tickPosition()
    return qslider_tickPosition(self.id);
end

function QSlider:triggerAction(arg0)
    qslider_triggerAction(self.id,arg0);
end

function QSlider:use(id)
   local q = {};
   setmetatable(q, QSlider);
   q.id = id;
   return q
end

function QSlider:value()
    return qslider_value(self.id);
end

QSpinBox = {};
QSpinBox.__index = QSpinBox;

function QSpinBox:cleanText()
    return qspinbox_cleanText(self.id);
end

function QSpinBox:delete()
    qspinbox_delete(self.id);
    self.id = 0;end

function QSpinBox:maximum()
    return qspinbox_maximum(self.id);
end

function QSpinBox:minimum()
    return qspinbox_minimum(self.id);
end

function QSpinBox:new(...)
   local q = {};
   setmetatable(q, QSpinBox);
   if arg ~= nil then
      q.id = qspinbox_new(unpack(arg))
   else
      q.id = qspinbox_new()
   end
   return q
end

function QSpinBox:prefix()
    return qspinbox_prefix(self.id);
end

function QSpinBox:setMaximum(arg0)
    qspinbox_setMaximum(self.id,arg0);
end

function QSpinBox:setMinimum(arg0)
    qspinbox_setMinimum(self.id,arg0);
end

function QSpinBox:setPrefix(arg0)
    qspinbox_setPrefix(self.id,arg0);
end

function QSpinBox:setRange(arg0,arg1)
    qspinbox_setRange(self.id,arg0,arg1);
end

function QSpinBox:setSingleStep(arg0)
    qspinbox_setSingleStep(self.id,arg0);
end

function QSpinBox:setSuffix(arg0)
    qspinbox_setSuffix(self.id,arg0);
end

function QSpinBox:setValue(arg0)
    qspinbox_setValue(self.id,arg0);
end

function QSpinBox:singleStep()
    return qspinbox_singleStep(self.id);
end

function QSpinBox:suffix()
    return qspinbox_suffix(self.id);
end

function QSpinBox:use(id)
   local q = {};
   setmetatable(q, QSpinBox);
   q.id = id;
   return q
end

function QSpinBox:value()
    return qspinbox_value(self.id);
end

QVBoxLayout = {};
QVBoxLayout.__index = QVBoxLayout;

function QVBoxLayout:addWidget(arg0)
    qvboxlayout_addWidget(self.id,arg0.id);
end

function QVBoxLayout:delete()
    qvboxlayout_delete(self.id);
    self.id = 0;end

function QVBoxLayout:new(...)
   local q = {};
   setmetatable(q, QVBoxLayout);
   if arg ~= nil then
      q.id = qvboxlayout_new(unpack(arg))
   else
      q.id = qvboxlayout_new()
   end
   return q
end

function QVBoxLayout:use(id)
   local q = {};
   setmetatable(q, QVBoxLayout);
   q.id = id;
   return q
end

QWizard = {};
QWizard.__index = QWizard;

function QWizard:accept()
    qwizard_accept(self.id);
end

function QWizard:addPage(arg0)
    return qwizard_addPage(self.id,arg0.id);
end

function QWizard:back()
    qwizard_back(self.id);
end

function QWizard:buttonText(arg0)
    return qwizard_buttonText(self.id,arg0);
end

function QWizard:currentId()
    return qwizard_currentId(self.id);
end

function QWizard:delete()
    qwizard_delete(self.id);
    self.id = 0;end

function QWizard:new(...)
   local q = {};
   setmetatable(q, QWizard);
   if arg ~= nil then
      q.id = qwizard_new(unpack(arg))
   else
      q.id = qwizard_new()
   end
   return q
end

function QWizard:next()
    qwizard_next(self.id);
end

function QWizard:reject()
    qwizard_reject(self.id);
end

function QWizard:removePage(arg0)
    qwizard_removePage(self.id,arg0);
end

function QWizard:restart()
    qwizard_restart(self.id);
end

function QWizard:setButton(arg0,arg1)
    qwizard_setButton(self.id,arg0,arg1.id);
end

function QWizard:setButtonText(arg0,arg1)
    qwizard_setButtonText(self.id,arg0,arg1);
end

function QWizard:setDefaultProperty(arg0,arg1,arg2)
    qwizard_setDefaultProperty(self.id,arg0,arg1,arg2);
end

function QWizard:setField(arg0,arg1)
    qwizard_setField(self.id,arg0,arg1);
end

function QWizard:setOption(arg0,...)
    if arg == nil then
        arg = {}
    end
    qwizard_setOption(self.id,arg0,unpack(arg));
end

function QWizard:setPage(arg0,arg1)
    qwizard_setPage(self.id,arg0,arg1.id);
end

function QWizard:setSideWidget(arg0)
    qwizard_setSideWidget(self.id,arg0.id);
end

function QWizard:setStartId(arg0)
    qwizard_setStartId(self.id,arg0);
end

function QWizard:setSubTitleFormat(arg0)
    qwizard_setSubTitleFormat(self.id,arg0);
end

function QWizard:setTitleFormat(arg0)
    qwizard_setTitleFormat(self.id,arg0);
end

function QWizard:setVisible(arg0)
    qwizard_setVisible(self.id,arg0);
end

function QWizard:setWindowTitle(arg0)
    qwizard_setWindowTitle(self.id,arg0);
end

function QWizard:setWizardStyle(arg0)
    qwizard_setWizardStyle(self.id,arg0);
end

function QWizard:startId()
    return qwizard_startId(self.id);
end

function QWizard:subTitleFormat()
    return qwizard_subTitleFormat(self.id);
end

function QWizard:testOption(arg0)
    qwizard_testOption(self.id,arg0);
end

function QWizard:titleFormat()
    return qwizard_titleFormat(self.id);
end

function QWizard:use(id)
   local q = {};
   setmetatable(q, QWizard);
   q.id = id;
   return q
end

function QWizard:validateCurrentPage()
    return qwizard_validateCurrentPage(self.id);
end

function QWizard:wizardStyle()
    return qwizard_wizardStyle(self.id);
end

QWizardPage = {};
QWizardPage.__index = QWizardPage;

function QWizardPage:delete()
    qwizardpage_delete(self.id);
    self.id = 0;end

function QWizardPage:new(...)
   local q = {};
   setmetatable(q, QWizardPage);
   if arg ~= nil then
      q.id = qwizardpage_new(unpack(arg))
   else
      q.id = qwizardpage_new()
   end
   return q
end

function QWizardPage:setLayout(arg0)
    qwizardpage_setLayout(self.id,arg0.id);
end

function QWizardPage:setSubTitle(arg0)
    qwizardpage_setSubTitle(self.id,arg0);
end

function QWizardPage:setTitle(arg0)
    qwizardpage_setTitle(self.id,arg0);
end

function QWizardPage:use(id)
   local q = {};
   setmetatable(q, QWizardPage);
   q.id = id;
   return q
end

