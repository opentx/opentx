/*
 * Author - Kjell Kernen
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef WIZARDDIALOG_H
#define WIZARDDIALOG_H
#include <QWizard>
#include "wizarddata.h" 

QT_BEGIN_NAMESPACE
class QCheckBox;
class QLabel;
class QLineEdit;
class QRadioButton;
class QComboBox;
QT_END_NAMESPACE

class WizardDialog : public QWizard
{
  Q_OBJECT
public:
  enum { Page_Models, Page_Throttle, Page_Wingtypes, Page_Ailerons, Page_Flaps, 
    Page_Airbrakes, Page_Bank, Page_Rudder, Page_Tails, Page_Tail, 
    Page_Vtail, Page_Simpletail, Page_Cyclic, Page_Gyro, Page_Flybar,
    Page_Fblheli, Page_Helictrl, Page_Multirotor,
    Page_Conclusion };

  Mix mix;
  WizardDialog(QWidget *parent = 0);

  private slots:
    void showHelp();
};

class  StandardPage: public QWizardPage
{
  Q_OBJECT
public:
  StandardPage(int curPage, WizardDialog *dlg, QString image, QString title, QString text, int nextPage=-1);
  WizardDialog *wizDlg;
  void populateCB( QComboBox *);
  bool bookChannel(QString label, Input input1, int weight1, Input input2=NOINPUT, int weight2=0 );
  void releaseChannels();
  void cleanupPage();
private:
  QLabel *topLabel;
  int pageCurrent;
  int pageFollower;
  int nextId() const;
};

class  ModelSelectionPage: public StandardPage
{
  Q_OBJECT
public:
  ModelSelectionPage(WizardDialog *dlg, QString image, QString title, QString text);  
private:
  QLineEdit *nameLineEdit;
  QRadioButton *planeRB;
  QRadioButton *multirotorRB;
  QRadioButton *helicopterRB;
  bool validatePage();
  int nextId() const;  
};

class  WingtypeSelectionPage: public StandardPage
{
  Q_OBJECT
public:
  WingtypeSelectionPage(WizardDialog *dlg, QString image, QString title, QString text);  
private:
  QRadioButton *deltaWingRB;
  QRadioButton *standardWingRB;
  int nextId() const;  
};

class  TailSelectionPage: public StandardPage
{
  Q_OBJECT
public:
  TailSelectionPage(WizardDialog *dlg, QString image, QString title, QString text);  
private:
  QRadioButton *vTailRB;
  QRadioButton *standardTailRB;
  QRadioButton *simpleTailRB;
  int nextId() const;  
};

class  FlybarSelectionPage: public StandardPage
{
  Q_OBJECT
public:
  FlybarSelectionPage(WizardDialog *dlg, QString image, QString title, QString text);  
private:
  QRadioButton *flybarRB;
  QRadioButton *noFlybarRB;
  int nextId() const;  
};

class ThrottlePage: public StandardPage
{
  Q_OBJECT
public:
  ThrottlePage(WizardDialog *dlg, QString image, QString title, QString text, int nextPage=-1);  
  void initializePage();
  bool validatePage();
private:
  QRadioButton *motorRB;
  QRadioButton *noMotorRB;
  QComboBox *throttleCB;
};

class AileronsPage: public StandardPage
{
  Q_OBJECT
public:
  AileronsPage(WizardDialog *dlg, QString image, QString title, QString text, int nextPage=-1);  
  void initializePage();
  bool validatePage();
private:
  QRadioButton *oneAileronRB;
  QRadioButton *twoAileronsRB;
  QRadioButton *noAileronsRB;
  QComboBox *aileron1CB;
  QComboBox *aileron2CB;
};

class FlapsPage: public StandardPage
{
  Q_OBJECT
public:
  FlapsPage(WizardDialog *dlg, QString image, QString title, QString text, int nextPage=-1);  
  void initializePage();
  bool validatePage();
private:
  QRadioButton *oneFlapRB;
  QRadioButton *twoFlapsRB;
  QRadioButton *noFlapsRB;
  QComboBox *flap1CB;
  QComboBox *flap2CB;
};

class AirbreaksPage: public StandardPage
{
  Q_OBJECT
public:
  AirbreaksPage(WizardDialog *dlg, QString image, QString title, QString text, int nextPage=-1);  
  void initializePage();
  bool validatePage();
private:
  QRadioButton *oneAirbreakRB;
  QRadioButton *twoAirbreaksRB;
  QRadioButton *noAirbreaksRB;
  QComboBox *airbreak1CB;
  QComboBox *airbreak2CB;
};

class BankPage: public StandardPage
{
  Q_OBJECT
public:
  BankPage(WizardDialog *dlg, QString image, QString title, QString text, int nextPage=-1);  
  void initializePage();
  bool validatePage();
private:
  QRadioButton *oneElevonChRB;
  QRadioButton *twoElevonsChRB;
  QComboBox *elevon1CB;
  QComboBox *elevon2CB;
};

class RudderPage: public StandardPage
{
  Q_OBJECT
public:
  RudderPage(WizardDialog *dlg, QString image, QString title, QString text, int nextPage=-1);  
  void initializePage();
  bool validatePage();
private:
  QRadioButton *noRudderRB;
  QRadioButton *hasRudderRB;
  QComboBox *rudderCB;
};

class TailPage: public StandardPage
{
  Q_OBJECT
public:
  TailPage(WizardDialog *dlg, QString image, QString title, QString text, int nextPage=-1);  
  void initializePage();
  bool validatePage();
private:
  QComboBox *elevatorCB;
  QComboBox *rudderCB;
};

class VTailPage: public StandardPage
{
  Q_OBJECT
public:
  VTailPage(WizardDialog *dlg, QString image, QString title, QString text, int nextPage=-1);  
  void initializePage();
  bool validatePage();
private:
  QComboBox *tail1CB;
  QComboBox *tail2CB;
};

class SimpleTailPage: public StandardPage
{
  Q_OBJECT
public:
  SimpleTailPage(WizardDialog *dlg, QString image, QString title, QString text, int nextPage=-1);  
  void initializePage();
  bool validatePage();
private:
  QComboBox *elevatorCB;
};

class CyclicPage: public StandardPage
{
  Q_OBJECT
public:
  CyclicPage(WizardDialog *dlg, QString image, QString title, QString text, int nextPage=-1);  
  void initializePage();
  bool validatePage();
private:
  QRadioButton *cyclic90RB;
  QRadioButton *cyclic120RB;
  QRadioButton *cyclic120XRB;
  QRadioButton *cyclic140RB;
};

class GyroPage: public StandardPage
{
  Q_OBJECT
public:
  GyroPage(WizardDialog *dlg, QString image, QString title, QString text, int nextPage=-1);  
  void initializePage();
  bool validatePage();
private:
  QRadioButton *noGyroRB;
  QRadioButton *switchGyroRB;
  QRadioButton *potGyroRB;
};

class FblPage: public StandardPage
{
  Q_OBJECT
public:
  FblPage(WizardDialog *dlg, QString image, QString title, QString text, int nextPage=-1);  
  void initializePage();
  bool validatePage();
private:
  QComboBox *throttleCB;
  QComboBox *yawCB;
  QComboBox *pitchCB;
  QComboBox *rollCB;
};

class HeliPage: public StandardPage
{
  Q_OBJECT
public:
  HeliPage(WizardDialog *dlg, QString image, QString title, QString text, int nextPage=-1);  
  void initializePage();
  bool validatePage();
private:
  QComboBox *throttleCB;
  QComboBox *yawCB;
  QComboBox *pitchCB;
  QComboBox *rollCB;
};

class MultirotorPage: public StandardPage
{
  Q_OBJECT
public:
  MultirotorPage(WizardDialog *dlg, QString image, QString title, QString text, int nextPage=-1);  
  void initializePage();
  bool validatePage();
private:
  QComboBox *throttleCB;
  QComboBox *yawCB;
  QComboBox *pitchCB;
  QComboBox *rollCB;
};

class ConclusionPage: public StandardPage
{
  Q_OBJECT
public:
  ConclusionPage(WizardDialog *dlg, QString image, QString title, QString text, int nextPage=-1);  
  void initializePage();
  bool validatePage();
private:
  QCheckBox *proceedCB;
  QLabel *textLabel;
};

#endif  // WIZARDDIALOG_H
