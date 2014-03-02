#include <QtGui>
#include "wizarddialog.h"

Channel::Channel()
{
  clear();
}

void Channel::clear()
{
  sourceDlg = -1;
  input1 = UNDEFINED;
  input2 = UNDEFINED;
  weight1 = 0;  
  weight2 = 0;
}

bool Channel::isEmpty()
{
  return sourceDlg < 0;
}

QString Channel::nameOf(Input input)
{
  switch (input){
    case THROTTLE: return "THR";
    case RUDDER:   return "RUD";
    case ELEVATOR: return "ELE";
    case AILERON:  return "AIL";
    case FLAP:     return "FLP";
    case AIRBREAK: return "AIR";
    default:       return "---";
  }
}

QString Channel::print()
{
  QString str;
  str =  QString("[%1, %2]").arg(nameOf(input1)).arg(weight1);
  if ( input2 != UNDEFINED )
    str += QString("[%1, %2]").arg(nameOf(input2)).arg(weight2);
  return str;
}

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
    setPage(Page_Vtail, new VTailPage(this, "vtail",tr("V-Tail"),tr("Select channels for tail control."), Page_Conclusion));
    setPage(Page_Simpletail, new SimpleTailPage(this, "simpletail",tr("Tail"),tr("Select elevator channel."), Page_Conclusion));
    setPage(Page_Cyclic, new CyclicPage(this, "cyclic",tr("Cyclic"),tr("Which type of swash control is installed in your helicopter?"), Page_Gyro));
    setPage(Page_Gyro, new GyroPage(this, "gyro",tr("Tail Gyro"),tr("Has your helicopter got an adjustable gyro for the tail?"), Page_Flybar));
    setPage(Page_Flybar, new FlybarSelectionPage(this, "flybar",tr("Rotor Type"),tr("Has your helicopter got a flybar?")));
    setPage(Page_Fblheli, new FblPage(this, "fblheli",tr("Helicopter"),tr("Select the controls for your helicopter"), Page_Conclusion));
    setPage(Page_Helictrl, new HeliPage(this, "helictrl",tr("Helicopter"),tr("Select the controls for your helicopter"), Page_Conclusion));
    setPage(Page_Multirotor, new MultirotorPage(this, "multirotor",tr("Multirotor"),tr("Select the control channels for your multirotor"), Page_Conclusion));
    setPage(Page_Conclusion, new ConclusionPage(this, "conclusion",tr("Save Changes"),tr(
      "Manually check the direction of each control surface and reverse any channels that make controls move in the wrong direction. "
      "Remove the propeller/propellers before you try to control your model for the first time.<br>"
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
    case Page_Vtail:      message = tr("TBD."); break;
    case Page_Simpletail: message = tr("TBD."); break;
    case Page_Flybar:     message = tr("TBD."); break;
    case Page_Cyclic:     message = tr("TBD."); break;
    case Page_Gyro:       message = tr("TBD."); break;
    case Page_Fblheli:    message = tr("TBD."); break;
    case Page_Helictrl:   message = tr("TBD."); break;
    case Page_Multirotor: message = tr("TBD."); break;
    case Page_Conclusion: message = tr("TBD."); break;
    default:              message = tr("Move on. Nothin to see here.");
    }
    QMessageBox::information(this, tr("Model Wizard Help"), message);
}

StandardPage::StandardPage(int currentPage, WizardDialog *dlg, QString image, QString title, QString text, int nextPage )
    : QWizardPage()
{
    pageCurrent = currentPage;
    pageFollower = nextPage;
    wizDlg = dlg;

    setTitle(title);
    setPixmap(QWizard::WatermarkPixmap, QPixmap(QString(":/images/wizard/") + image + QString(".png")));
    topLabel = new QLabel(text+"<br>");
    topLabel->setWordWrap(true);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(topLabel);
    setLayout(layout);
}

void StandardPage::populateCB( QComboBox *CB )
{
  for (int j=0; j<MAX_CHANNELS; j++)
    CB->removeItem(0);

  for (int i=0; i<MAX_CHANNELS; i++)
  {
    if (wizDlg->channel[i].isEmpty()){
      CB->addItem(tr("Channel ") + QString("%1").arg(i+1), i);    
    }
  }
}

bool StandardPage::bookChannel(QString label, Input input1, int weight1, Input input2, int weight2 )
{
  int index = label.right(1).toInt() - 1;

  if (index<0 || index >= MAX_CHANNELS)
    return false; 
  if (!wizDlg->channel[index].isEmpty())
    return false;

  wizDlg->channel[index].sourceDlg = pageCurrent;
  wizDlg->channel[index].input1 = input1;
  wizDlg->channel[index].input2 = input2;
  wizDlg->channel[index].weight1 = weight1;  
  wizDlg->channel[index].weight2 = weight2;    
  
  return true;
}

void StandardPage::releaseChannels()
{
  for (int i=0; i<MAX_CHANNELS; i++)
  {
    if (wizDlg->channel[i].sourceDlg == pageCurrent){
      wizDlg->channel[i].clear();    
    }
  }
}

void StandardPage::cleanupPage()
{
  releaseChannels();
}

int StandardPage::nextId() const
{
    return pageFollower;
}

ModelSelectionPage::ModelSelectionPage(WizardDialog *dlg, QString image, QString title, QString text)
    : StandardPage(WizardDialog::Page_Models, dlg, image, title, text)
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
    : StandardPage(WizardDialog::Page_Wingtypes, dlg, image, title, text)
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
    : StandardPage(WizardDialog::Page_Tails, dlg, image, title, text)
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
    else if (vTailRB->isChecked())
      return WizardDialog::Page_Vtail;
    else
      return WizardDialog::Page_Tail;
}

FlybarSelectionPage::FlybarSelectionPage(WizardDialog *dlg, QString image, QString title, QString text)
    : StandardPage(WizardDialog::Page_Flybar, dlg, image, title, text)
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
    : StandardPage(WizardDialog::Page_Throttle, dlg, image, title, text, nextPage)
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

void ThrottlePage::initializePage(){
  populateCB(throttleCB);
}

bool ThrottlePage::validatePage() {
  releaseChannels();
  return bookChannel(throttleCB->currentText(), THROTTLE, 100 );
}

AileronsPage::AileronsPage(WizardDialog *dlg, QString image, QString title, QString text, int nextPage)
    : StandardPage(WizardDialog::Page_Ailerons, dlg, image, title, text, nextPage)
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

void AileronsPage::initializePage(){
  populateCB(aileron1CB);
  populateCB(aileron2CB);
}

bool AileronsPage::validatePage() {
  releaseChannels();
  if (noAileronsRB->isChecked()) { 
    return true;
  }
  if (oneAileronRB->isChecked()) { 
    return (bookChannel(aileron1CB->currentText(), AILERON, 100 )); 
  }
  return( bookChannel(aileron1CB->currentText(), AILERON, 100 ) && 
          bookChannel(aileron2CB->currentText(), AILERON, 100 ));
}

FlapsPage::FlapsPage(WizardDialog *dlg, QString image, QString title, QString text, int nextPage)
    : StandardPage(WizardDialog::Page_Flaps, dlg, image, title, text, nextPage)
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

void FlapsPage::initializePage(){
  populateCB(flap1CB);
  populateCB(flap2CB);
}

bool FlapsPage::validatePage() {
  releaseChannels();
  if (noFlapsRB->isChecked()) { 
    return true;
  }
  if (oneFlapRB->isChecked()) { 
    return (bookChannel(flap1CB->currentText(), FLAP, 100 )); 
  }
  return( bookChannel(flap1CB->currentText(), FLAP, 100 ) && 
          bookChannel(flap2CB->currentText(), FLAP, 100 ));
}

AirbreaksPage::AirbreaksPage(WizardDialog *dlg, QString image, QString title, QString text, int nextPage)
    : StandardPage(WizardDialog::Page_Airbrakes, dlg, image, title, text, nextPage)
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

void AirbreaksPage::initializePage(){
  populateCB(airbreak1CB);
  populateCB(airbreak2CB);
}

bool AirbreaksPage::validatePage() {
  releaseChannels();
  if (noAirbreaksRB->isChecked()) { 
    return true;
  }
  if (oneAirbreakRB->isChecked()) { 
    return (bookChannel(airbreak1CB->currentText(), AIRBREAK, 100 )); 
  }
  return( bookChannel(airbreak1CB->currentText(), AIRBREAK, 100 ) && 
          bookChannel(airbreak2CB->currentText(), AIRBREAK, 100 ));
}

BankPage::BankPage(WizardDialog *dlg, QString image, QString title, QString text, int nextPage)
    : StandardPage(WizardDialog::Page_Bank, dlg, image, title, text, nextPage)
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

void BankPage::initializePage(){
  populateCB(elevon1CB);
  populateCB(elevon2CB);
}

bool BankPage::validatePage() {
  releaseChannels();
  if (oneElevonChRB->isChecked()) { 
    return (bookChannel(elevon1CB->currentText(), AILERON, 100, ELEVATOR, 100 )); 
  }
  return( bookChannel(elevon1CB->currentText(), AILERON, 100, ELEVATOR, 100 ) && 
          bookChannel(elevon2CB->currentText(), AILERON, 100, ELEVATOR, 100 ));
}

RudderPage::RudderPage(WizardDialog *dlg, QString image, QString title, QString text, int nextPage)
    : StandardPage(WizardDialog::Page_Rudder, dlg, image, title, text, nextPage)
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

void RudderPage::initializePage(){
  populateCB(rudderCB);
}

bool RudderPage::validatePage() {
  releaseChannels();
  if (noRudderRB->isChecked())
     return true;

  return (bookChannel(rudderCB->currentText(), RUDDER, 100)); 
}

VTailPage::VTailPage(WizardDialog *dlg, QString image, QString title, QString text, int nextPage)
    : StandardPage(WizardDialog::Page_Tail, dlg, image, title, text, nextPage)
{
    tail1CB = new QComboBox();
    tail2CB = new QComboBox();

    QLayout *l = layout();
    l->addWidget(new QLabel(tr("First Tail Channel:")));
    l->addWidget(tail1CB);
    l->addWidget(new QLabel(tr("Second Tail Channel:")));
    l->addWidget(tail2CB);
}

void VTailPage::initializePage(){
  populateCB(tail1CB);
  populateCB(tail2CB);
}

bool VTailPage::validatePage() {
  releaseChannels();
  return( bookChannel(tail1CB->currentText(), ELEVATOR, 100, RUDDER, 100 ) && 
          bookChannel(tail2CB->currentText(), ELEVATOR, 100, RUDDER, 100 ));
}

TailPage::TailPage(WizardDialog *dlg, QString image, QString title, QString text, int nextPage)
    : StandardPage(WizardDialog::Page_Tail, dlg, image, title, text, nextPage)
{
    elevatorCB = new QComboBox();
    rudderCB = new QComboBox();

    QLayout *l = layout();
    l->addWidget(new QLabel(tr("Rudder Channel:")));
    l->addWidget(rudderCB);
    l->addWidget(new QLabel(tr("Elevator Channel:")));
    l->addWidget(elevatorCB);
}

void TailPage::initializePage(){
  populateCB(elevatorCB);
  populateCB(rudderCB);
}

bool TailPage::validatePage() {
  releaseChannels();
  return( bookChannel(elevatorCB->currentText(), ELEVATOR, 100 ) && 
          bookChannel(rudderCB->currentText(),   RUDDER,   100 ));
}

SimpleTailPage::SimpleTailPage(WizardDialog *dlg, QString image, QString title, QString text, int nextPage)
    : StandardPage(WizardDialog::Page_Simpletail, dlg, image, title, text, nextPage)
{  
    elevatorCB = new QComboBox();

    QLayout *l = layout();
    l->addWidget(new QLabel(tr("Elevator Channel:")));
    l->addWidget(elevatorCB);
}

void SimpleTailPage::initializePage(){
  populateCB(elevatorCB);
}

bool SimpleTailPage::validatePage() {
  releaseChannels();
  return( bookChannel(elevatorCB->currentText(), ELEVATOR, 100 ));
}

CyclicPage::CyclicPage(WizardDialog *dlg, QString image, QString title, QString text, int nextPage)
    : StandardPage(WizardDialog::Page_Cyclic, dlg, image, title, text, nextPage)
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

void CyclicPage::initializePage(){
}

bool CyclicPage::validatePage() {
  releaseChannels();
  return true;
}

GyroPage::GyroPage(WizardDialog *dlg, QString image, QString title, QString text, int nextPage)
    : StandardPage(WizardDialog::Page_Gyro, dlg, image, title, text, nextPage)
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

void GyroPage::initializePage(){
}

bool GyroPage::validatePage() {
  releaseChannels();
  return true;
}

FblPage::FblPage(WizardDialog *dlg, QString image, QString title, QString text, int nextPage)
    : StandardPage(WizardDialog::Page_Fblheli, dlg, image, title, text, nextPage)
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

void FblPage::initializePage(){
  populateCB(throttleCB);
  populateCB(yawCB);
  populateCB(pitchCB);
  populateCB(rollCB);
}

bool FblPage::validatePage() {
  releaseChannels();
  return( bookChannel(throttleCB->currentText(), THROTTLE, 100 ) && 
          bookChannel(yawCB->currentText(),      RUDDER,   100 ) &&
          bookChannel(pitchCB->currentText(),    ELEVATOR, 100 ) &&
          bookChannel(rollCB->currentText(),     AILERON,  100 ));
}

HeliPage::HeliPage(WizardDialog *dlg, QString image, QString title, QString text, int nextPage)
    : StandardPage(WizardDialog::Page_Helictrl, dlg, image, title, text, nextPage)
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

void HeliPage::initializePage(){
  populateCB(throttleCB);
  populateCB(yawCB);
  populateCB(pitchCB);
  populateCB(rollCB);
}

bool HeliPage::validatePage() {
  releaseChannels();
  return( bookChannel(throttleCB->currentText(), THROTTLE, 100 ) && 
          bookChannel(yawCB->currentText(),      RUDDER,   100 ) &&
          bookChannel(pitchCB->currentText(),    ELEVATOR, 100 ) &&
          bookChannel(rollCB->currentText(),     AILERON,  100 ));
}

MultirotorPage::MultirotorPage(WizardDialog *dlg, QString image, QString title, QString text, int nextPage)
    : StandardPage(WizardDialog::Page_Multirotor, dlg, image, title, text, nextPage)
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

void MultirotorPage::initializePage(){
  populateCB(throttleCB);
  populateCB(yawCB);
  populateCB(pitchCB);
  populateCB(rollCB);
}

bool MultirotorPage::validatePage() {
  releaseChannels();
  return( bookChannel(throttleCB->currentText(), THROTTLE, 100 ) && 
          bookChannel(yawCB->currentText(),      RUDDER,   100 ) &&
          bookChannel(pitchCB->currentText(),    ELEVATOR, 100 ) &&
          bookChannel(rollCB->currentText(),     AILERON,  100 ));
}

ConclusionPage::ConclusionPage(WizardDialog *dlg, QString image, QString title, QString text, int nextPage)
    : StandardPage(WizardDialog::Page_Conclusion, dlg, image, title, text, nextPage)
{
    textLabel = new QLabel();
    proceedCB = new QCheckBox(tr("OK, I understand."));
    registerField("evaluate.proceed*", proceedCB);

    QLayout *l = layout();
    l->addWidget(textLabel);
    l->addWidget(proceedCB);
}

void ConclusionPage::initializePage(){
  QString str;
  for (int i=0; i<MAX_CHANNELS; i++){
    if (!wizDlg->channel[i].isEmpty()){
      str += QString(tr("Channel %1: ").arg(i+1));
      str += wizDlg->channel[i].print();
      str += QString("<br>");
    }
  }
  textLabel->setText(str);
}


