#include <QtGui>
#include "wizarddialog.h"

WizardDialog::WizardDialog(QWidget *parent)
    : QWizard(parent)
{
    setWindowTitle(tr("Model Wizard"));

    setPage(Page_Models, new ModelSelectionPage(this, "models",tr("Model Type"),tr("Enter model name and model type.")));
    setPage(Page_Throttle, new ThrottlePage(this, "throttle",tr("Throttle"),tr("Has your model got a motor or an engine?"), Page_Wingtypes));
    setPage(Page_Wingtypes, new WingtypeSelectionPage(this, "wingtype",tr("Wing Type"),tr("Is your model a flying wing/deltawing or has it a standard wing configuration?")));
    setPage(Page_Ailerons, new AileronsPage(this, "ailerons",tr("Ailerons"),tr("Has your model got ailerons?"), Page_Flaps));
    setPage(Page_Flaps, new FlapsPage(this, "flaps",tr("Flaps"),tr("Has your model got flaps?"), Page_Airbrakes));
    setPage(Page_Airbrakes, new AirbreaksPage(this, "airbrakes",tr("Airbrakes"),tr("Has your model got airbrakes?"), Page_Tails));
    setPage(Page_Bank, new BankPage(this, "bank",tr("Flying-wing / Delta-wing"),tr("Are the elevons controlled by servos connected to separate channels or by a single servo channel?"), Page_Rudder));
    setPage(Page_Rudder, new RudderPage(this, "rudder",tr("Rudder"),tr("Does your model have a rudder?"), Page_Conclusion));
    setPage(Page_Tails, new TailSelectionPage(this, "tails",tr("Tail Type"),tr("Select which type of tail your model is equiped with.")));
    setPage(Page_Tail, new TailPage(this, "tail",tr("Tail"),tr("Select channels for tail control."), Page_Conclusion));
    setPage(Page_Simpletail, new SimpleTailPage(this, "simpletail",tr("Tail"),tr("Select elevator channel."), Page_Conclusion));
    setPage(Page_Cyclic, new CyclicPage(this, "cyclic",tr("Cyclic"),tr("Which type of swash control is installed in your helicopter?"), Page_Gyro));
    setPage(Page_Gyro, new GyroPage(this, "gyro",tr("Tail Gyro"),tr("Has your helicopter got an adjustable gyro for the tail?"), Page_Flybar));
    setPage(Page_Flybar, new FlybarSelectionPage(this, "flybar",tr("Rotor Type"),tr("Has your helicopter got a flybar?")));
    setPage(Page_Fblheli, new FblPage(this, "fblheli",tr("Helicopter"),tr("Select the controls for your helicopter"), Page_Conclusion));
    setPage(Page_Helictrl, new HeliPage(this, "helictrl",tr("Helicopter"),tr("Select the controls for your helicopter"), Page_Conclusion));
    setPage(Page_Multirotor, new MultirotorPage(this, "multirotor",tr("Multirotor"),tr("Select the control channels for your multirotor"), Page_Conclusion));
    setPage(Page_Conclusion, new ConclusionPage(this, "conclusion",tr("Save Changes"),tr(
      "Your model should now be set up. You now have to manually check the direction of each control surface and reverse the channels that "
      "make controls move in the wrong direction.<br><br>Remove the propeller/propellers before you try to control your model for the first time.<br><br>"
      "Please note that continuing removes all old model settings!"), -1));

    setStartId(Page_Models);

#ifndef Q_WS_MAC
    setWizardStyle(ModernStyle);
#endif
    setOption(HaveHelpButton, true);
    connect(this, SIGNAL(helpRequested()), this, SLOT(showHelp()));
}

void WizardDialog::showHelp()
{
    QString message;

    switch (currentId()) {
    case Page_Models:     message = tr("Select the type of model you want to create settings for."); break;
    case Page_Throttle:   message = tr("Select the receiver channel that is connected to your ESC or throttle servo."); break;
    case Page_Wingtypes:  message = tr("The wing control surfaces of flying wings and delta winged aircraft are called elevons and are used for controlling both elevation and roll. "
                                       "These aircrafts do not have a tail with control surfaces"); break;
    case Page_Ailerons:   message = tr("One or two channels are used to control the Ailerons. "
                                       "If your servos are connected by a common Y-cable you should select the single-servo option."); break;
    case Page_Flaps:      message = tr("This wizard assumes that your flaps are controlled by a switch. "
                                       "If your flaps are controlled by a potentiometer you can change that manually later."
                                       "Just select the correct receiver channel and an arbitrary switch."); break;
    case Page_Airbrakes:  message = tr("Air brakes are used to reduce the speed of advanced sail planes. "
                                       "It is very uncommon on other types of planes."); break;
    case Page_Bank:       message = tr("Please note that, even if the elevons are controlled by separate servos, "
                                       "these may be controlled by a common channel."); break;
    case Page_Rudder:     message = tr("TBD."); break;
    case Page_Tails:      message = tr("Select the tail type of your plane."); break;
    case Page_Tail:       message = tr("TBD."); break;
    case Page_Simpletail: message = tr("TBD."); break;
    case Page_Flybar:     message = tr("TBD."); break;
    case Page_Cyclic:     message = tr("TBD."); break;
    case Page_Gyro:       message = tr("TBD."); break;
    case Page_Fblheli:    message = tr("TBD."); break;
    case Page_Helictrl:   message = tr("TBD."); break;
    case Page_Multirotor: message = tr("TBD."); break;
    case Page_Conclusion: message = tr("TBD.");  break;
    default:              message = tr("Move on. Nothin to see here.");
    }
    QMessageBox::information(this, tr("Model Wizard Help"), message);
}

StandardPage::StandardPage(WizardDialog *dlg, QString image, QString title, QString text, int nextPage)
    : QWizardPage()
{
    pageFollower = nextPage;
    wizardDialog = dlg;

    setTitle(title);
    setPixmap(QWizard::WatermarkPixmap, QPixmap(QString(":/images/wizard/") + image + QString(".png")));
    topLabel = new QLabel(text+"<br>");
    topLabel->setWordWrap(true);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(topLabel);
    setLayout(layout);
}

int StandardPage::nextId() const
{
    return pageFollower;
}

ModelSelectionPage::ModelSelectionPage(WizardDialog *dlg, QString image, QString title, QString text)
    : StandardPage(dlg, image, title, text)
{
    nameLineEdit = new QLineEdit;
    planeRB = new QRadioButton(tr("Plane"));
    planeRB->setChecked(true);
    multirotorRB = new QRadioButton(tr("Multirotor"));
    helicopterRB = new QRadioButton(tr("Helicopter"));

    registerField("evaluate.name*", nameLineEdit);

    QLayout *l = layout();
    l->addWidget(new QLabel(tr("Model Name:")));
    l->addWidget(nameLineEdit);
    l->addWidget(new QLabel(""));
    l->addWidget(new QLabel(tr("Model Type:")));
    l->addWidget(planeRB);
    l->addWidget(multirotorRB);
    l->addWidget(helicopterRB);
}

int ModelSelectionPage::nextId() const 
{
    if (helicopterRB->isChecked())
      return WizardDialog::Page_Cyclic;
    else if (multirotorRB->isChecked())
      return WizardDialog::Page_Multirotor;
    else
      return WizardDialog::Page_Throttle;
}

WingtypeSelectionPage::WingtypeSelectionPage(WizardDialog *dlg, QString image, QString title, QString text)
    : StandardPage(dlg, image, title, text)
{
    standardWingRB = new QRadioButton(tr("Standard Wing"));
    standardWingRB->setChecked(true);
    deltaWingRB = new QRadioButton(tr("Flying Wing / Deltawing"));

    QLayout *l = layout();
    l->addWidget(standardWingRB);
    l->addWidget(deltaWingRB);
}

int WingtypeSelectionPage::nextId() const
{
    if (deltaWingRB->isChecked())
      return WizardDialog::Page_Bank;
    else
      return WizardDialog::Page_Ailerons;
}

TailSelectionPage::TailSelectionPage(WizardDialog *dlg, QString image, QString title, QString text)
    : StandardPage(dlg, image, title, text)
{
    standardTailRB = new QRadioButton(tr("Elevator and Rudder"));
    standardTailRB->setChecked(true);
    simpleTailRB = new QRadioButton(tr("Only Elevator"));
    vTailRB = new QRadioButton(tr("V-tail"));

    QLayout *l = layout();
    l->addWidget(new QLabel(tr("Tail Type:")));
    l->addWidget(standardTailRB);
    l->addWidget(simpleTailRB);
    l->addWidget(vTailRB);
}

int TailSelectionPage::nextId() const
{
    if (simpleTailRB->isChecked())
      return WizardDialog::Page_Simpletail;
    else
      return WizardDialog::Page_Tail;
}

FlybarSelectionPage::FlybarSelectionPage(WizardDialog *dlg, QString image, QString title, QString text)
    : StandardPage(dlg, image, title, text)
{
    flybarRB = new QRadioButton(tr("Has Flybar"));
    flybarRB->setChecked(true);
    noFlybarRB = new QRadioButton(tr("Flybarless"));

    QLayout *l = layout();
    l->addWidget(new QLabel(tr("Flybar:")));
    l->addWidget(flybarRB);
    l->addWidget(noFlybarRB);
}

int FlybarSelectionPage::nextId() const
{
    if (flybarRB->isChecked())
      return WizardDialog::Page_Helictrl;
    else
      return WizardDialog::Page_Fblheli;
}

ThrottlePage::ThrottlePage(WizardDialog *dlg, QString image, QString title, QString text, int nextPage)
    : StandardPage(dlg, image, title, text, nextPage)
{
    motorRB = new QRadioButton(tr("Yes"));
    noMotorRB = new QRadioButton(tr("No"));
    motorRB->setChecked(true);
    throttleCB = new QComboBox();

    QLayout *l = layout();
    l->addWidget(motorRB);
    l->addWidget(noMotorRB);
    l->addWidget(new QLabel(tr("<br>Throttle Channel:")));
    l->addWidget(throttleCB);
}

AileronsPage::AileronsPage(WizardDialog *dlg, QString image, QString title, QString text, int nextPage)
    : StandardPage(dlg, image, title, text, nextPage)
{
    noAileronsRB = new QRadioButton(tr("No"));
    oneAileronRB = new QRadioButton(tr("Yes, controlled by a single channel"));
    twoAileronsRB = new QRadioButton(tr("Yes, controlled by two channels"));
    noAileronsRB->setChecked(true);
    aileron1CB = new QComboBox();
    aileron2CB = new QComboBox();
 
    QLayout *l = layout();
    l->addWidget(noAileronsRB);
    l->addWidget(oneAileronRB);
    l->addWidget(twoAileronsRB);
    l->addWidget(new QLabel(tr("<br>First Aileron Channel:")));
    l->addWidget(aileron1CB);
    l->addWidget(new QLabel(tr("Second Aileron Channel:")));
    l->addWidget(aileron2CB);
}

FlapsPage::FlapsPage(WizardDialog *dlg, QString image, QString title, QString text, int nextPage)
    : StandardPage(dlg, image, title, text, nextPage)
{
    noFlapsRB = new QRadioButton(tr("No"));
    oneFlapRB = new QRadioButton(tr("Yes, controlled by a single channel"));
    twoFlapsRB = new QRadioButton(tr("Yes, controlled by two channels"));
    noFlapsRB->setChecked(true);
    flap1CB = new QComboBox();
    flap2CB = new QComboBox();

    QLayout *l = layout();
    l->addWidget(noFlapsRB);
    l->addWidget(oneFlapRB);
    l->addWidget(twoFlapsRB);
    l->addWidget(new QLabel(tr("<br>First Flap Channel:")));
    l->addWidget(flap1CB);
    l->addWidget(new QLabel(tr("Second Flap Channel:")));
    l->addWidget(flap2CB);
}

AirbreaksPage::AirbreaksPage(WizardDialog *dlg, QString image, QString title, QString text, int nextPage)
    : StandardPage(dlg, image, title, text, nextPage)
{
    noAirbreaksRB = new QRadioButton(tr("No"));
    oneAirbreakRB = new QRadioButton(tr("Yes, controlled by a single channel"));
    twoAirbreaksRB = new QRadioButton(tr("Yes, controlled by two channels"));
    noAirbreaksRB->setChecked(true);
    airbreak1CB = new QComboBox();
    airbreak2CB = new QComboBox();

    QLayout *l = layout();
    l->addWidget(noAirbreaksRB);
    l->addWidget(oneAirbreakRB);
    l->addWidget(twoAirbreaksRB);
    l->addWidget(new QLabel(tr("<br>First Airbreak Channel:")));
    l->addWidget(airbreak1CB);
    l->addWidget(new QLabel(tr("Second Airbreak Channel:")));
    l->addWidget(airbreak2CB);
}

BankPage::BankPage(WizardDialog *dlg, QString image, QString title, QString text, int nextPage)
    : StandardPage(dlg, image, title, text, nextPage)
{
    oneElevonChRB = new QRadioButton(tr("One"));
    oneElevonChRB->setChecked(true);
    twoElevonsChRB = new QRadioButton(tr("Two"));
    elevon1CB = new QComboBox();
    elevon2CB = new QComboBox();

    QLayout *l = layout();
    l->addWidget(oneElevonChRB);
    l->addWidget(twoElevonsChRB);
    l->addWidget(new QLabel(tr("<br>First Elevon Channel:")));
    l->addWidget(elevon1CB);
    l->addWidget(new QLabel(tr("Second Elevon Channel:")));
    l->addWidget(elevon2CB);
}
RudderPage::RudderPage(WizardDialog *dlg, QString image, QString title, QString text, int nextPage)
    : StandardPage(dlg, image, title, text, nextPage)
{
    noRudderRB = new QRadioButton(tr("No"));
    noRudderRB->setChecked(true);
    hasRudderRB = new QRadioButton(tr("Yes"));
    rudderCB = new QComboBox();

    QLayout *l = layout();
    l->addWidget(noRudderRB);
    l->addWidget(hasRudderRB);
    l->addWidget(new QLabel(tr("<br>Rudder Channel:")));
    l->addWidget(rudderCB);
}

TailPage::TailPage(WizardDialog *dlg, QString image, QString title, QString text, int nextPage)
    : StandardPage(dlg, image, title, text, nextPage)
{
    elevatorCB = new QComboBox();
    rudderCB = new QComboBox();

    QLayout *l = layout();
    l->addWidget(new QLabel(tr("Rudder Channel:")));
    l->addWidget(rudderCB);
    l->addWidget(new QLabel(tr("Elevator Channel:")));
    l->addWidget(elevatorCB);
}

SimpleTailPage::SimpleTailPage(WizardDialog *dlg, QString image, QString title, QString text, int nextPage)
    : StandardPage(dlg, image, title, text, nextPage)
{  
    elevatorCB = new QComboBox();

    QLayout *l = layout();
    l->addWidget(new QLabel(tr("Elevator Channel:")));
    l->addWidget(elevatorCB);
}

CyclicPage::CyclicPage(WizardDialog *dlg, QString image, QString title, QString text, int nextPage)
    : StandardPage(dlg, image, title, text, nextPage)
{
    cyclic90RB = new QRadioButton(tr("90"));
    cyclic90RB->setChecked(true);
    cyclic120RB = new QRadioButton(tr("120"));
    cyclic120XRB = new QRadioButton(tr("120x"));
    cyclic140RB = new QRadioButton(tr("140"));

    QLayout *l = layout();
    l->addWidget(cyclic90RB);
    l->addWidget(cyclic120RB);
    l->addWidget(cyclic120XRB);
    l->addWidget(cyclic140RB);
}

GyroPage::GyroPage(WizardDialog *dlg, QString image, QString title, QString text, int nextPage)
    : StandardPage(dlg, image, title, text, nextPage)
{
    noGyroRB = new QRadioButton(tr("No"));
    noGyroRB->setChecked(true);
    switchGyroRB = new QRadioButton(tr("Yes, controled by a switch"));
    potGyroRB = new QRadioButton(tr("Yes, controlled by a pot"));

    QLayout *l = layout();
    l->addWidget(noGyroRB);
    l->addWidget(switchGyroRB);
    l->addWidget(potGyroRB);
}

FblPage::FblPage(WizardDialog *dlg, QString image, QString title, QString text, int nextPage)
    : StandardPage(dlg, image, title, text, nextPage)
{
    throttleCB = new QComboBox();
    yawCB = new QComboBox();
    pitchCB = new QComboBox();
    rollCB = new QComboBox();

    QLayout *l = layout();
    l->addWidget(new QLabel(tr("Trottle Channel:")));
    l->addWidget(throttleCB);
    l->addWidget(new QLabel(tr("Yaw Channel:")));
    l->addWidget(yawCB);
    l->addWidget(new QLabel(tr("Pitch Channel:")));
    l->addWidget(pitchCB);
    l->addWidget(new QLabel(tr("Roll Channel:")));
    l->addWidget(rollCB);
}

HeliPage::HeliPage(WizardDialog *dlg, QString image, QString title, QString text, int nextPage)
    : StandardPage(dlg, image, title, text, nextPage)
{
    throttleCB = new QComboBox();
    yawCB = new QComboBox();
    pitchCB = new QComboBox();
    rollCB = new QComboBox();

    QLayout *l = layout();
    l->addWidget(new QLabel(tr("Trottle Channel:")));
    l->addWidget(throttleCB);
    l->addWidget(new QLabel(tr("Yaw Channel:")));
    l->addWidget(yawCB);
    l->addWidget(new QLabel(tr("Pitch Channel:")));
    l->addWidget(pitchCB);
    l->addWidget(new QLabel(tr("Roll Channel:")));
    l->addWidget(rollCB);
}

MultirotorPage::MultirotorPage(WizardDialog *dlg, QString image, QString title, QString text, int nextPage)
    : StandardPage(dlg, image, title, text, nextPage)
{
    throttleCB = new QComboBox();
    yawCB = new QComboBox();
    pitchCB = new QComboBox();
    rollCB = new QComboBox();

    QLayout *l = layout();
    l->addWidget(new QLabel(tr("Trottle Channel:")));
    l->addWidget(throttleCB);
    l->addWidget(new QLabel(tr("Yaw Channel:")));
    l->addWidget(yawCB);
    l->addWidget(new QLabel(tr("Pitch Channel:")));
    l->addWidget(pitchCB);
    l->addWidget(new QLabel(tr("Roll Channel:")));
    l->addWidget(rollCB);
}

ConclusionPage::ConclusionPage(WizardDialog *dlg, QString image, QString title, QString text, int nextPage)
    : StandardPage(dlg, image, title, text, nextPage)
{
    proceedCB = new QCheckBox(tr("OK, I understand."));
    registerField("evaluate.proceed*", proceedCB);

    QLayout *l = layout();
    l->addWidget(proceedCB);
}

