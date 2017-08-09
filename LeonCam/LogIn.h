#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_LogIn.h"
#include <qpushbutton.h>
#include "ForgottenPassword.h"
#include "DesignBase.h"
#include "NewProfile.h"
#include "MainApp.h"
#include "DataBase.h"
#include <qtooltip.h>

#define loginAttemptCounterMAX 3
#define loginTimeLock 900

class LogIn : public QMainWindow
{
	Q_OBJECT

public:
	LogIn(QWidget *parent = Q_NULLPTR);
private slots:
	void LogInClicked();
	void ForgotPasswordClicked();
	void NewProfileClicked();
private:
	Ui::LogInClass ui;
	DesignBase *designB;
	void LogIn::UpdateAttempts(int loginAttemptCounter, QString username);
};
