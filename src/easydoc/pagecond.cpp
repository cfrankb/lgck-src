/*
    LGCK Builder Runtime
    Copyright (C) 1999, 2020  Francois Blanchette

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
*/
#include "pagecond.h"
#include "ui_pagecond.h"
#include "testcase.h"

CPageCond::CPageCond(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CPageCond)
{
    ui->setupUi(this);
    const char *comboTypes[] = {
        "argCount",
        "boolean",
        "number",
        "string",
        "table"
    };
    for (unsigned int i=0; i < sizeof(comboTypes)/sizeof(const char*);++i) {
        ui->cbType->addItem(comboTypes[i]);
    }
    const char *comboOperators[]={
        "==",
        "!="
        ">",
        "<",
        "<=",
        ">=",
        "NOP"
    };
    for (unsigned int i=0; i < sizeof(comboOperators)/sizeof(const char*);++i) {
        ui->cbOperator->addItem(comboOperators[i]);
    }
    for (int i=0; i < 8;++i) {
        ui->cbArg->addItem(i ? QString("%1").arg(i) : "");
    }
}

CPageCond::~CPageCond()
{
    delete ui;
}

void CPageCond::load(const CStep & step, int i)
{
    const Condition & cond = step.m_conditions[i];
    ui->cbArg->setCurrentIndex(cond.argNum);
    ui->cbType->setCurrentIndex(cond.type);
    ui->cbOperator->setCurrentIndex(cond.op);
    ui->eValue->setText(cond.value.trimmed());
}

void CPageCond::save(CStep & step, int i)
{
    Condition & cond = step.m_conditions[i];
    cond.argNum = ui->cbArg->currentIndex();
    cond.type = ui->cbType->currentIndex();
    cond.op = ui->cbOperator->currentIndex();
    cond.value = ui->eValue->text().trimmed();
}
