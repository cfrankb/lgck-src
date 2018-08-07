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
