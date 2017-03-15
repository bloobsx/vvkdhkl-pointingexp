#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QComboBox>

#include <QGridLayout>
#include <QStringList>
#include <QDebug>
#include <QMainWindow>

class SessionInfo
{
public:
    ~SessionInfo();
    QString participantName;
    int numTrials;
    bool isTraining;
    QString instruction;
    std::string transferFunc;
    bool onTarget = false;
    //int targetL;
    //int targetR;
    int targetW;
    int targetA;
    bool targetActive = false;
    std::string inpuri; // = "any:?";
    std::string outuri; // = "any:?";
    int missedClicks = 0;
};


class MainWindow: public QMainWindow
{
  Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

/*private:
    MainWindow *ui;*/
};

class SessionDialog : public QDialog
{

 Q_OBJECT

private:

 QLabel* labelParticipant;
 QLabel* labelConfFile;
 QLabel* labelBlockNo;
 QLabel* labelTrialNo;

 QLineEdit* comboParticipant;
 QLineEdit* confFile;
 QLineEdit* blockNo;
 QLineEdit* trialNo;
 //QComboBox* instruction;
 //QComboBox* transferFunc;
 //QComboBox* isTraining;
 QToolBar* tools;
 QPushButton* loadFile;



 void setUpGUI();


public:
 QDialogButtonBox* buttons;
 explicit SessionDialog();
 QString getParticipantName();
 int getBlockNo();
 int getTrialNo();
 QString getConfFileName();
 void prepareSlots();
 //int runApp(int argc, char** argv, std::vector<SessionInfo> sessions);
 bool proceedExp;


public slots:
 void proceed();
 void loadFileDialog();
 void finish();


};


#endif // SESSIONMANAGER_H
