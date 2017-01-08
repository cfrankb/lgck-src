#include "pagecond.h"
#include "ui_pagecond.h"
#include "testcase.h"

CPageCond::CPageCond(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CPageCond)
{
    ui->setupUi(this);
    const char *comboValues[] = {
        "argCount",
        "boolean",
        "number",
        "string",
        "table"
    };
    for (unsigned int i=0; i < sizeof(comboValues)/sizeof(const char*);++i) {
        ui->cbType->addItem(comboValues[i]);
    }
    for (int i=0; i < 8;++i) {
        ui->cbArg->addItem(i ? QString("%1").arg(i) : "");
    }
}

CPageCond::~CPageCond()
{
    delete ui;
}

void CPageCond::set(int arg, int type, const QString & value)
{
    ui->cbArg->setCurrentIndex(arg);
    ui->cbType->setCurrentIndex(type);
    ui->eValue->setText(value.trimmed());
}

void CPageCond::get(int & arg, int & type, QString & value)
{
    arg = ui->cbArg->currentIndex();
    type = ui->cbType->currentIndex();
    value = ui->eValue->text().trimmed();
}

void CPageCond::load(const CStep & step, int i)
{
    const Condition & cond = step.m_conditions[i];
    set(cond.argNum, cond.type, cond.value);
}

void CPageCond::save(CStep & step, int i)
{
    Condition & cond = step.m_conditions[i];
    get(cond.argNum, cond.type, cond.value);
}
