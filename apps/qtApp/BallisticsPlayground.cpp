/* -*- mode: c++ -*-
 *
 * apps/ballistics/BallisticsPlayground.h --
 *
 * Initial software
 * Authors: Nicolas Roussel
 * Copyright © Inria
 *
 * http://libpointing.org/
 *
 * This software may be used and distributed according to the terms of
 * the GNU General Public License version 2 or any later version.
 *
 */

#include "BallisticsPlayground.h"
//#include "sessionmanager.h"
#include <pointing/utils/TimeStamp.h>

#include <QApplication>
#include <QPainter>
#include <QDateTime>
#include <QPoint>
#include <QDebug>
#include <QMessageBox>
#include <QElapsedTimer>
#include <QDir>
#include "../PointingLogger/QsLog.h"

#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#define USE_APPLE_CURSOR 0
#endif

// ----------------------------------------------------------------------------

static QPoint
GetCursorPosition(void) {
#if ! BALLISCTICSPLAYGROUND_USE_QTPOS
#ifdef __APPLE__
  CGPoint loc = CGEventGetLoca  tion(CGEventCreate(NULL)) ;
  return QPoint(loc.x, loc.y) ;
#endif
#ifdef WIN32
  POINT loc;
  GetCursorPos(&loc) ;
  return QPoint(loc.x, loc.y) ;
#endif
#ifdef __linux__
  // FIXME
#endif
#endif
  return QCursor::pos() ;
}

// ----------------------------------------------------------------------------

BallisticsPlayground::Pointer::Pointer(const char *funcuri, BallisticsPlayground *playground) {
  master = playground ;
  func = TransferFunction::create(funcuri, master->input, master->output) ;
  URI uri = func->getURI() ;
  name = QString::fromStdString(uri.asString()) ;
}

BallisticsPlayground::Pointer::Pointer(TransferFunction *func, BallisticsPlayground *playground) {
  master = playground ;
  this->func = func ;
  URI uri = func->getURI() ;
  name = QString::fromStdString(uri.asString()) ;
}

void
BallisticsPlayground::Pointer::moveBy(int input_dx, int input_dy, TimeStamp::inttime timestamp) {
  int output_dx=0, output_dy=0 ;
  func->applyi(input_dx, input_dy, &output_dx, &output_dy, timestamp) ;
  pos += QPoint(output_dx, output_dy) ;
  if (pos.rx() < 0) pos.setX(0);
  if (pos.ry() < 0) pos.setY(0);
  if (pos.rx() > master->frameSize().width()) pos.setX(master->geometry().width());
  if (pos.ry() > master->frameSize().height()) pos.setY(master->geometry().height());
}

void
BallisticsPlayground::Pointer::moveBy(int input_dx, int  input_dy, int & output_dx, int & output_dy, TimeStamp::inttime timestamp) {
  //int output_dx=0, output_dy=0 ;
  func->applyi(input_dx, input_dy, &output_dx, &output_dy, timestamp) ;
  pos += QPoint(output_dx, output_dy) ;
  if (pos.rx() < 0) pos.setX(0);
  if (pos.ry() < 0) pos.setY(0);
  if (pos.rx() > master->frameSize().width()) pos.setX(master->geometry().width());
  if (pos.ry() > master->frameSize().height()) pos.setY(master->geometry().height());
}

void
BallisticsPlayground::Pointer::moveTo(QPoint p) {
  this->pos = p; //.setX(p.x()) ;
  //pos.setY(p.y()) ;
}

BallisticsPlayground::Pointer::~Pointer(void) {
  delete func ;
}

// ----------------------------------------------------------------------------

BallisticsPlayground::BallisticsPlayground(PointingDevice *input, 
                       DisplayDevice *output, std::vector<SessionInfo> sessions,
					   QWidget *parent)
  : BALLISCTICSPLAYGROUND_BASECLASS(parent),
    disptimer(this) {
  this->input = input ;
  this->output = output ;
  this->sessions = sessions;
  this->logInit();
  this->addPointer(sessions[0].transferFunc.c_str());

  //connect(this, SIGNAL(finish()), SLOT(quit()));

#if 0 && BALLISCTICSPLAYGROUND_USE_OPENGL
  const QGLFormat fmt = format() ;
  std::cerr << "QGLFormat swapInterval: " << fmt.swapInterval() << std::endl ;
#endif

  input->setPointingCallback(pointingCallback, (void*)this) ;

  instructDesc = QString("Please continue to click the highlighted target in blue, with as high %1 as you can.");

  oDesc = QString("%1 [%2 PPI @ %3 Hz]")
    .arg(output->getURI().asString().c_str())
    .arg(output->getResolution())
    .arg(output->getRefreshRate()) ;

  pdDesc = QString("QPaintDevice: log=%1/%2 DPI, phy=%3/%4 DPI")
    .arg(this->logicalDpiX())
    .arg(this->logicalDpiY())
    .arg(this->physicalDpiX())
    .arg(this->physicalDpiY()) ;

#if BALLISCTICSPLAYGROUND_FORCE_UPDATES
  connect(&disptimer, SIGNAL(timeout()), this, SLOT(update())) ;
  //connect()
  disptimer.start(1000.0/output->getRefreshRate()) ;
#endif

  recordfile = 0 ;
  record = 0 ;

  artificialLatency = 0 ;

  buttons = 0 ;

  sysPointerViz = SYSTEM ;
  sysPointerLoc = GetCursorPosition() ;
  setMouseTracking(true) ;
  lpoint = mapFromGlobal(activePointer->pos);

  debugLevel = 3 ;

  reset() ;

}

void BallisticsPlayground::setBlockNo(int blockno){
    this->blockNo = blockno;
}

void BallisticsPlayground::setTrialNo(int trialno){
    this->trialNo = trialno;
}

void
BallisticsPlayground::logInit() {
    QsLogging::DestinationFactory fileout;
    QString filename = QDir::currentPath()+"/logs/pointingLog-"+QString::number(QDateTime::currentMSecsSinceEpoch())+".csv";
    //filename.replace(" ","");
    //filename.replace(":","");
    this->logDest = fileout.MakeFileDestination(filename, QsLogging::DisableLogRotation, QsLogging::MaxSizeBytes(50000000));
    this->logger.addDestination(this->logDest);
    this->logDest->write(QString::number(QDateTime::currentMSecsSinceEpoch())+ \
                         (QString) " Program Instance started. Logger starts successfully.\n", QsLogging::TraceLevel );
}

void
BallisticsPlayground::logSummary(SessionInfo session) {
    QString sessiondetail = QString::fromStdString("Summary: \n") + \
                                                   QString::fromStdString("time elapsed: \n") + \
                                                   QString::fromStdString("number of target misses: ") + QString::number(session.missedClicks) + \
                                                   QString::fromStdString("\nclick errors present: ") + QString::number(session.missedClicks>0);
    this->logDest->write(sessiondetail, QsLogging::TraceLevel);
}

void
BallisticsPlayground::logSession(SessionInfo session) {

    int targetL = (this->geometry().width()-sessions[blockNo].targetA)/2;
    int targetR = (this->geometry().width()+sessions[blockNo].targetA)/2;

    QString sessiondetail = QString::fromStdString("Participant: ") + session.participantName + "\n" + \
                            QString::fromStdString("Instruction: ") + session.instruction + "\n" + \
                             "Block No: " + QString::number(blockNo) + "\n" + \
                             "Training Session: " + ((std::to_string(sessions[blockNo].isTraining)=="Train")?"Yes":"No") + "\n" + \
                              "Trial No: " + QString::number(trialNo) + "\n" + \
                               "Transfer function: " + QString::fromStdString(session.transferFunc) + "\n" + \
                                "Target Amplitude (A): " + QString::number(session.targetA) + "\n" + \
                                  "Target Width (W): " + QString::number(session.targetW) + "\n" + \
                                   "Fitts ID (log2(A/W + 1)): " + QString::number(log2(session.targetA / session.targetW + 1)) + "\n" + \
                                     "Input Device: " + QString::fromStdString(session.inpuri) + "\n" + \
                                      "Output Device: " + QString::fromStdString(session.outuri) + "\n" + \
                                       "Start Location (horizontal) : " + QString::number(targetL) + "\n" + \
                                       "Target Location (horizontal) : " + QString::number(targetR) ;
    this->logDest->write(sessiondetail, QsLogging::TraceLevel);

    this->logDest->write("Date_Timesamp, sys_cursor_loc_x, sys_cursor_loc_y, screen_cursor_loc_x, screen_cursor_loc_y, button_pressed, input_displacement_x, input_displacement_y, output_displacement_x, output_displacement_y, transitional_cd_gain_value", QsLogging::TraceLevel);

                         //device_physical_loc_x, device_physical_loc_y

}

void
BallisticsPlayground::logEvent(int logLevel, EventLogDetail e) {
    if (logLevel==1) {
    try {

        QString logString = QString::number(QDateTime::currentMSecsSinceEpoch())+ \
                // ", " + QString::number(deviceLocation) +
                ", " + QString::number(QCursor::pos().x()) + \
                ", " + QString::number(QCursor::pos().y()) + \
                ", " + QString::number(e.activePointer->pos.x()) + \
                ", " + QString::number(e.activePointer->pos.y()) + \
                ", " + QString::number(buttons) + \
                ", " + QString::number(e.input_dx) + \
                ", " + QString::number(e.input_dy) + \
                ", " + QString::number(e.output_dx) + \
                ", " + QString::number(e.output_dy) + \
                ", " + QString::number(e.cdgain);

        this->logDest->write(logString, QsLogging::TraceLevel);

    }
    catch (std::exception e){
        std::cerr << "Exception: Cannot write to logger:" << e.what() << std::endl ;
    }
   }
}

void
BallisticsPlayground::addPointer(const char *function_uri) {
  std::cerr << "Adding pointer " << function_uri << std::endl ;
  activePointer = new Pointer(function_uri, this) ;
  activePointer->moveTo(GetCursorPosition());
  //activePointer = p;
  //activePointer->moveTo() ;

  update() ;
}

void
BallisticsPlayground::addPointer(TransferFunction *function) {
    *activePointer = Pointer(function, this) ;
    activePointer->moveTo(GetCursorPosition());
    //activePointer = p;
  update() ;
}

void
BallisticsPlayground::pointingCallback(void *context, TimeStamp::inttime timestamp,
				       int input_dx, int input_dy, int buttons) {
  BallisticsPlayground *playground = (BallisticsPlayground*)context ;

  playground->pointingEvent(timestamp, input_dx, input_dy, buttons) ;


}

void
BallisticsPlayground::reset(void) {
  QPoint syspos = GetCursorPosition() ;
  //for (std::list<Pointer*>::iterator p=pointers.begin();
       //Pointer* p = activePointer;
  activePointer->moveTo(syspos) ;
  QCursor::setPos(syspos) ;
}


void
BallisticsPlayground::pointingEvent(TimeStamp::inttime timestamp,
				    int input_dx, int input_dy, 
                    int buttons) {
  this->buttons = buttons ;
  //this->lastbutton = (this->lastbutton==this->buttons)?this->lastbutton:0;
    int output_dx = 0, output_dy = 0;

    /*if (blockNo>sessions.size()){
        //painter.drawText(x1, y1, "Thank you for your participation!!");
        this->showNormal();
    }*/

    int leftlim, rightlim, uplim, downlim;
    uplim = this->geometry().height()/2 - 10;
    downlim = this->geometry().height()/2 + 10;
    int targetL = (this->geometry().width()-sessions[blockNo].targetA)/2;
    int targetR = (this->geometry().width()+sessions[blockNo].targetA)/2;

    if (sessions[blockNo].targetActive) {
        leftlim = targetR - sessions[blockNo].targetW/2;
        rightlim = targetR +sessions[blockNo].targetW/2;

    }
    else {
        leftlim = targetL - 10;
        rightlim =   targetL + 10;

    }

    //if  && activePointer->pos.y() < downlim) {

    //bool targetRequired = (sessions[blockNo].instruction == "Speed")?true:(activePointer->pos.x() > centre && activePointer->pos.x() < rightlim && buttons>0 && activePointer->pos.y() > uplim && activePointer->pos.y() < downlim);
    //if (sessions[blockNo].instruction == "Speed")


    if ( buttons==4 && (this->lastbutton!=this->buttons) && (this->sessions[blockNo].targetActive || ((activePointer->pos.y() > uplim) && (activePointer->pos.y() < downlim) && (activePointer->pos.x() < rightlim) && (activePointer->pos.x() > leftlim)))) {
        this->sessions[blockNo].onTarget = true;
        sessions[blockNo].targetActive = !(sessions[blockNo].targetActive);
        }
    else if (buttons>0) this->sessions[blockNo].missedClicks += 1;


    if ((input_dx || input_dy) && buttons==1){
      activePointer->moveBy(input_dx, input_dy, output_dx, output_dy, timestamp) ;

      //this->logEvent(1, activePointer);

      this->eventLogDetail.activePointer = activePointer;
      this->eventLogDetail.input_dx = input_dx;
      this->eventLogDetail.input_dy = input_dy;
      this->eventLogDetail.output_dx = output_dx;
      this->eventLogDetail.output_dy = output_dy;


      //this->logDest->write("," + QString::number(input_dx) + "," + QString::number(input_dy) + "," +
      //                           QString::number(output_dx) + "," + QString::number(output_dy), QsLogging::TraceLevel);

    double delta = (double)(timestamp - this->last_time)/TimeStamp::one_millisecond ;


     this->physDist += sqrt(input_dx * input_dx + input_dy * input_dy);
     this->virtDist += sqrt(output_dx * output_dx + output_dy * output_dy);
     this->deltaSum += delta;

     if (this->deltaSum >= 25.0) {
            double physSpeed = this->physDist / this->deltaSum;
            double virtSpeed = this->virtDist / this->deltaSum;

            //std::cout << " control speed: " << physSpeed << "display speed: " << virtSpeed << " CD gain: " << (virtSpeed / physSpeed)*4 << std::endl;

            this->eventLogDetail.cdgain = (virtSpeed / physSpeed)*4;

            this->deltaSum = 0;
            this->physDist = 0;
            this->virtDist = 0;

/*
                   COORD coord;
                   coord.Y = int(75 - virtSpeed / physSpeed * 16);
                   //std::cout << coord.X << std::endl;
                   coord.X = int(physSpeed / 10 * 160);
                   //SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
                   //printf("%c[%d;%df", 0x1B, y, x);
                   //printf(".\n");
*/

     }
     this->logEvent(1, eventLogDetail);
    }
    this->last_time = timestamp;






  this->lastbutton = this->buttons;


  if (blockNo < sessions.size()) update() ; // scheduled
  //repaint() ; // immediate
}

void
BallisticsPlayground::keyPressEvent(QKeyEvent *event) {
  if (event->text()==" ") {
    reset() ;
    update() ;
  } else if (event->text()=="p") {
    sysPointerViz =( sysPointerVisualization)((sysPointerViz+1)%4) ;
    setCursor(sysPointerViz&SYSTEM ? Qt::ArrowCursor : Qt::BlankCursor) ;
    update() ;
  } else if (event->text()=="d") {
    debugLevel++;
    if (debugLevel == 3) debugLevel=0;
    update() ;
  }

}

void
BallisticsPlayground::mouseMoveEvent(QMouseEvent *event) {
  sysPointerLoc = event->pos() ;
}


void
BallisticsPlayground::paintEvent(QPaintEvent * /*event*/) {

    QPainter painter(this) ;
    painter.setRenderHint(QPainter::Antialiasing, false) ;

    painter.setFont(QFont("Courier", 11)) ;

    if (iDesc.text().isEmpty() && input->isActive())
      iDesc = QString("%1 [%2 CPI @ %3 Hz]")
        .arg(input->getURI().asString().c_str())
        .arg(input->getResolution())
        .arg(input->getUpdateFrequency()) ;

    painter.setPen(Qt::black) ;
    painter.setBrush(Qt::red) ;

    if (sysPointerViz&PSEUDO)
      painter.drawRect(sysPointerLoc.x()-2, sysPointerLoc.y()-2, 4, 4) ;

    painter.setBrush(Qt::white) ;

    int targetL = (this->geometry().width()-sessions[blockNo].targetA)/2;
    int targetR = (this->geometry().width()+sessions[blockNo].targetA)/2;

    painter.setBrush(sessions[blockNo].targetActive?QBrush(Qt::gray):QBrush(Qt::blue));
    //painter.drawRect(QPointF(sessions[blockNo].targetL - sessions[blockNo].targetW, 400 - sessions[blockNo].targetW, 2 * sessions[blockNo].targetW, 2 * sessions[blockNo].targetW);
    painter.drawEllipse(QPointF(targetL, this->geometry().height()/2), 10, 10);
    painter.drawLine(targetL, this->geometry().height()/2 - 30, targetL, this->geometry().height()/2 + 30);
    painter.drawLine(targetL - 30, this->geometry().height()/2, targetL + 30, this->geometry().height()/2);
    painter.drawStaticText(QPointF(targetL-20, this->geometry().height()/2 + 40), QString("start"));


    painter.setBrush(sessions[blockNo].targetActive?QBrush(Qt::green):QBrush(Qt::gray));
    //painter.drawRect(/*QPointF(*/sessions[blockNo].targetR - sessions[blockNo].targetW, 400 - sessions[blockNo].targetW, 2 * sessions[blockNo].targetW, 2 * sessions[blockNo].targetW);
    painter.drawEllipse(QPointF(targetR, this->geometry().height()/2), sessions[blockNo].targetW/2, sessions[blockNo].targetW/2);
    if (sessions[blockNo].instruction=="Accuracy") {
        painter.setBrush(QBrush(Qt::red));
        painter.drawLine(targetR, this->geometry().height()/2 - sessions[blockNo].targetW/2 - 10, targetR, this->geometry().height()/2 + sessions[blockNo].targetW/2 + 10);
        painter.drawLine(targetR - sessions[blockNo].targetW/2 - 10, this->geometry().height()/2, targetR + sessions[blockNo].targetW/2 + 10, this->geometry().height()/2);

    }
    painter.drawStaticText(QPointF(targetR-25, this->geometry().height()/2 + sessions[blockNo].targetW/2 + 15), QString("target"));



    if (debugLevel > 1) {
      int x1 = 20, y1 = 20 ;\

  #if BALLISCTICSPLAYGROUND_SHOW_PAINTDEVICEDPI
      painter.drawStaticText(x1,y1, pdDesc) ;

      y1 += 20 ;
  #endif

      painter.drawStaticText(x1,y1, oDesc) ;

      y1 += 20 ;

      painter.drawStaticText(x1,y1, iDesc) ;




      //for (std::list<Pointer*>::const_iterator p=pointers.begin();
           //Pointer p=activePointer;
    //QPoint lp;
      //if (buttons==1) {
        /*QPoint*/ lpoint = mapFromGlobal(activePointer->pos) ;


        int x = lpoint.x(), y = lpoint.y() ;
        QImage image("images/cursor.png");
        painter.drawImage(x, y, image);

        painter.drawText(20, 80, "Cursor location:  "+QString::number(x)) ;

        y1 += 60;
        QPen writePen(Qt::red);
        painter.setBrush(QBrush(Qt::red));
        painter.setPen(writePen);
        painter.setFont(QFont("Arial", 14, QFont::Bold));
        painter.drawText(x1, y1, /*QStaticText(*/instructDesc.arg(this->sessions[blockNo].instruction));


        if (((/*trialNo==1 || (*/trialNo<=sessions[blockNo].numTrials && sessions[blockNo].onTarget))) {

            if (trialNo>1) this->logSummary(sessions[blockNo]);

            sessions[blockNo].onTarget = false;
            if (trialNo < sessions[blockNo].numTrials && !sessions[blockNo].targetActive)
            {
                trialNo = trialNo + 1;
                //sessions[blockNo].targetActive = !sessions[blockNo].targetActive ;
            }
            else if (!sessions[blockNo].targetActive) {
                trialNo = 1;
                blockNo += 1;
                //TransferFunction *tf = TransferFunction

                QMessageBox* mbox = new QMessageBox(this);

                if (blockNo < sessions.size()) {
                //painter.drawText(x1, y1, "Welcome to the new block. please press any key to continue");
                   //this->keyPressEvent(QKeyEvent(QEvent::KeyPress, Qt::Key_Pause, Qt::NoModifier, "Pause"));

                mbox->setText("New block begins from here. Parameters such as the transfer function (mouse pointer acceleration ratios), instructions, and other details change until next block. Please read the details before proceeding. You proceed when you click OK below.");
                mbox->setDetailedText("New block begins");
                //mbox->addButton();
                mbox->setMinimumSize(800, 800);
                mbox->exec();

                delete mbox;

                this->addPointer(sessions[blockNo].transferFunc.c_str());
                }
                else {
                mbox->setText("Thank you for the participation! Those were all the tasks we had for you.");
                mbox->exec();

                delete mbox;
                this->showNormal();

                }



            }

            this->logSession(sessions[blockNo]);

        }

      }




}

void BallisticsPlayground::quit(){
    //this->~BallisticsPlayground();
    qApp->quit();
}

BallisticsPlayground::~BallisticsPlayground(void) {


  delete output ;
  delete input ;

}


