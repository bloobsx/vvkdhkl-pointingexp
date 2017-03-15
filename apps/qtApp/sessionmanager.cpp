#include "sessionmanager.h"
#include "ballistics.cpp"

MainWindow::MainWindow()
{
    //ui->setupUi(this);
    //this->s
}

MainWindow::~MainWindow()
{
    //delete ui;
}



SessionDialog::SessionDialog() {
    this->setMinimumSize(400, 150);
    this->setWindowTitle(tr("Welcome screen"));
    //parent->setWindowTitle();
    setUpGUI();

    setModal(true);

}

void
SessionDialog::setUpGUI(){
    QGridLayout* formGridLayout = new QGridLayout( this );

    labelParticipant = new QLabel(this);
    labelParticipant->setText(tr("Participant's Name: "));
    comboParticipant = new QLineEdit( this );

    labelConfFile = new QLabel(this);
    labelConfFile->setText( tr("Configuration File (.conf): ") );
    labelBlockNo = new QLabel(this);
    labelBlockNo->setText( tr("Block No (Starts from 0) ") );
    labelTrialNo = new QLabel(this);
    labelTrialNo->setText( tr("Trial No (Starts from 1) ") );

    confFile = new QLineEdit(this);
    blockNo = new QLineEdit(this);
    blockNo->setText("0");
    blockNo->setValidator(new QIntValidator(this));
    trialNo = new QLineEdit(this);
    trialNo->setText("1");
    trialNo->setValidator(new QIntValidator(this));
    /*instruction->addItem("Speed");
    instruction->addItem("Precision");

    transferFunc = new QComboBox(this);
    transferFunc->addItem("Sigmoid (default)");
    transferFunc->addItem("Constant");
    transferFunc->addItem("Current System");
    transferFunc->addItem("OSX System");
    transferFunc->addItem("Windows system");
    transferFunc->addItem("XOrg System");

    isTraining = new QComboBox(this);
    isTraining->addItem("Yes");
    isTraining->addItem("No");*/

    loadFile = new QPushButton( "Select file", this );
    //loadAct = new QAction();
    //loadFile->addAction(QAction addButton(QDialogButtonBox::button(QDialogButtonBox::StandardButton));//->setText("Load...");


    buttons = new QDialogButtonBox( this );
    buttons->addButton( QDialogButtonBox::Ok );
    buttons->button( QDialogButtonBox::Ok )->setText( tr("Enter") );
    buttons->addButton( QDialogButtonBox::Cancel );
    buttons->button( QDialogButtonBox::Cancel )->setText( tr("Quit") );

    // connect slots

    formGridLayout->addWidget( labelParticipant, 0, 0 );
    formGridLayout->addWidget( comboParticipant, 0, 1 );
    formGridLayout->addWidget( buttons, 5, 0, 1, 2 );
    formGridLayout->addWidget( labelConfFile, 1, 0);
    formGridLayout->addWidget( confFile, 1, 1);
    formGridLayout->addWidget( loadFile, 1, 2);
    formGridLayout->addWidget( blockNo, 2, 1 );
    formGridLayout->addWidget( trialNo, 3, 1 );
    formGridLayout->addWidget( labelBlockNo, 2, 0);
    formGridLayout->addWidget( labelTrialNo, 3, 0);

    setLayout( formGridLayout );
}

void
SessionDialog::prepareSlots(){
    connect( buttons->button( QDialogButtonBox::Cancel ),
     SIGNAL (clicked()),
     this,
     SLOT (finish())
     );

    connect( buttons->button( QDialogButtonBox::Ok ),
     SIGNAL (clicked()),
     this,
     SLOT (proceed()) );

    connect( loadFile,
     SIGNAL (clicked()),
     this,
     SLOT (loadFileDialog()) );
}

QString SessionDialog::getParticipantName(){
    return comboParticipant->text();

}

int SessionDialog::getBlockNo(){
    return blockNo->text().toDouble() ;
}

int SessionDialog::getTrialNo(){
    return trialNo->text().toDouble();
}

QString SessionDialog::getConfFileName(){
    return confFile->text();

}

void SessionDialog::proceed(){

    this->proceedExp = true;
    this->close();
    //this->runApp(0, NULL, sessions);


}

void SessionDialog::finish(){

    this->close();
    qApp->quit();
}

void SessionDialog::loadFileDialog(){
    QString fileName = QFileDialog::getOpenFileName(this, //sDialog.parent(),
            "Open Image", "./", "*.conf");
    this->confFile->setText(fileName);
}

SessionInfo::~SessionInfo(void){
    //delete participantName;
    //delete instruction;
    //delete transferFunc;
    //delete inpuri;
    //delete outuri;
}

