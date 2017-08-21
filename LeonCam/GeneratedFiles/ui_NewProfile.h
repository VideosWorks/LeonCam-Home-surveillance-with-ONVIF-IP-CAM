/********************************************************************************
** Form generated from reading UI file 'NewProfile.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NEWPROFILE_H
#define UI_NEWPROFILE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_NewProfile
{
public:
    QLabel *Lloading;
    QLabel *LHelp;
    QLineEdit *LEUsername;
    QLineEdit *LEPassword;
    QLineEdit *LEConfPass;
    QLineEdit *LESecQuest;
    QLineEdit *LEAnswer;
    QLabel *LTipPassword;
    QPushButton *PBBack;
    QPushButton *PBCreate;
    QLabel *LTip;

    void setupUi(QDialog *NewProfile)
    {
        if (NewProfile->objectName().isEmpty())
            NewProfile->setObjectName(QStringLiteral("NewProfile"));
        NewProfile->setWindowModality(Qt::WindowModal);
        NewProfile->resize(800, 600);
        NewProfile->setMinimumSize(QSize(800, 600));
        NewProfile->setMaximumSize(QSize(800, 600));
        QIcon icon;
        icon.addFile(QStringLiteral(":/Resources/Images/logo.ico"), QSize(), QIcon::Normal, QIcon::Off);
        NewProfile->setWindowIcon(icon);
        NewProfile->setStyleSheet(QLatin1String("#NewProfile{\n"
"	background-image: url(:/Resources/Images/background.png);}"));
        Lloading = new QLabel(NewProfile);
        Lloading->setObjectName(QStringLiteral("Lloading"));
        Lloading->setGeometry(QRect(370, 80, 61, 61));
        LHelp = new QLabel(NewProfile);
        LHelp->setObjectName(QStringLiteral("LHelp"));
        LHelp->setGeometry(QRect(250, 310, 301, 51));
        LHelp->setStyleSheet(QLatin1String("color: rgb(255, 255, 255);\n"
"background-color:rgb(36, 118, 59)"));
        LEUsername = new QLineEdit(NewProfile);
        LEUsername->setObjectName(QStringLiteral("LEUsername"));
        LEUsername->setGeometry(QRect(290, 204, 261, 25));
        LEUsername->setMaxLength(50);
        LEPassword = new QLineEdit(NewProfile);
        LEPassword->setObjectName(QStringLiteral("LEPassword"));
        LEPassword->setGeometry(QRect(290, 234, 261, 25));
        LEPassword->setEchoMode(QLineEdit::Password);
        LEConfPass = new QLineEdit(NewProfile);
        LEConfPass->setObjectName(QStringLiteral("LEConfPass"));
        LEConfPass->setGeometry(QRect(290, 264, 261, 25));
        LEConfPass->setEchoMode(QLineEdit::Password);
        LESecQuest = new QLineEdit(NewProfile);
        LESecQuest->setObjectName(QStringLiteral("LESecQuest"));
        LESecQuest->setGeometry(QRect(250, 370, 301, 25));
        LEAnswer = new QLineEdit(NewProfile);
        LEAnswer->setObjectName(QStringLiteral("LEAnswer"));
        LEAnswer->setGeometry(QRect(250, 400, 301, 25));
        LTipPassword = new QLabel(NewProfile);
        LTipPassword->setObjectName(QStringLiteral("LTipPassword"));
        LTipPassword->setGeometry(QRect(250, 230, 31, 31));
        LTipPassword->setStyleSheet(QLatin1String("#LTipPassword{background-image: url(:/Resources/Images/bulb.png);}\n"
"#LTipPassword:hover{background-image: url(:/Resources/Images/bulbHover.png);}"));
        PBBack = new QPushButton(NewProfile);
        PBBack->setObjectName(QStringLiteral("PBBack"));
        PBBack->setGeometry(QRect(250, 510, 61, 25));
        PBBack->setStyleSheet(QLatin1String("#PBBack{\n"
"color:rgb(255, 255, 255);\n"
"background-color: rgb(255, 77, 61);}\n"
"#PBBack:hover{\n"
"background-color: rgb(255, 87, 58);\n"
"}"));
        PBCreate = new QPushButton(NewProfile);
        PBCreate->setObjectName(QStringLiteral("PBCreate"));
        PBCreate->setGeometry(QRect(490, 510, 61, 25));
        PBCreate->setStyleSheet(QLatin1String("#PBCreate{\n"
"color: rgb(255, 255, 255);\n"
"background-color:rgb(36, 118, 59)}\n"
"#PBCreate:hover\n"
"{\n"
"background-color: rgb(39, 129, 63);\n"
"}"));
        LTip = new QLabel(NewProfile);
        LTip->setObjectName(QStringLiteral("LTip"));
        LTip->setGeometry(QRect(250, 200, 31, 31));
        LTip->setStyleSheet(QLatin1String("#LTip{background-image: url(:/Resources/Images/bulb.png);}\n"
"#LTip:hover{background-image: url(:/Resources/Images/bulbHover.png);}"));

        retranslateUi(NewProfile);

        PBCreate->setDefault(true);


        QMetaObject::connectSlotsByName(NewProfile);
    } // setupUi

    void retranslateUi(QDialog *NewProfile)
    {
        NewProfile->setWindowTitle(QApplication::translate("NewProfile", "LeonCam", Q_NULLPTR));
        Lloading->setText(QString());
        LHelp->setText(QApplication::translate("NewProfile", "<html><head/><body><p align=\"center\">For your protection please, type the <span style=\" font-weight:600; color:#bdf0ff;\">Security Question</span></p><p align=\"center\">and answer.This will help in future verification</p></body></html>", Q_NULLPTR));
        LEUsername->setText(QString());
        LEUsername->setPlaceholderText(QApplication::translate("NewProfile", "Username", Q_NULLPTR));
        LEPassword->setText(QString());
        LEPassword->setPlaceholderText(QApplication::translate("NewProfile", "Password", Q_NULLPTR));
        LEConfPass->setText(QString());
        LEConfPass->setPlaceholderText(QApplication::translate("NewProfile", "Confirm password", Q_NULLPTR));
        LESecQuest->setText(QString());
        LESecQuest->setPlaceholderText(QApplication::translate("NewProfile", "Security Question", Q_NULLPTR));
        LEAnswer->setText(QString());
        LEAnswer->setPlaceholderText(QApplication::translate("NewProfile", "Answer", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        LTipPassword->setToolTip(QApplication::translate("NewProfile", "<html><head/><body>\n"
"<span style=\"font-weight:600;\">Requirements:</span>\n"
"<ul style=\"margin-top:0px;\">\n"
"<li style=\"margin-left:-30px;\">minimum 8 characters</li>\n"
"<li style=\"margin-left:-30px;\">minimum 1 digit</li>\n"
"<li style=\"margin-left:-30px;\">minimum 1 capital letter</li>\n"
"<li style=\"margin-left:-30px;\">minimum lowercase</li>\n"
"</ul>\n"
"</body>\n"
"</html>", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        LTipPassword->setText(QString());
        PBBack->setText(QApplication::translate("NewProfile", "Back", Q_NULLPTR));
        PBCreate->setText(QApplication::translate("NewProfile", "Create", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        LTip->setToolTip(QApplication::translate("NewProfile", "<html><head/><body><p><span style=\" font-weight:600;\">Username</span> has <span style=\" font-weight:600;\">50</span> letters up limit</p></body></html>", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        LTip->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class NewProfile: public Ui_NewProfile {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NEWPROFILE_H
