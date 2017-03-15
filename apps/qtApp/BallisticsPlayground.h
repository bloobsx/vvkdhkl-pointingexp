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

#ifndef BallisticsPlayground_h
#define BallisticsPlayground_h

#include "../pointing/transferfunctions/TransferFunction.h"

#include <QGLWidget>

#include <QStaticText>
#include <QKeyEvent>
#include <QTimer>
#include <QFile>
#include <QTextStream>


#include "../PointingLogger/QsLog.h"
#include "sessionmanager.h"

using namespace pointing;

// ---------------------------------------------------------------------
// Variants...

// QGLWidget or QWidget?
#define BALLISCTICSPLAYGROUND_USE_OPENGL 1

// Use QCursor::pos or platform-specific call?
#define BALLISCTICSPLAYGROUND_USE_QCURSOR_POS 1

// Change this to 1 to display QPaintDevice DPI info
#define BALLISCTICSPLAYGROUND_SHOW_PAINTDEVICEDPI 0

// Change this to 1 to schedule redisplays at output's frequency
#define BALLISCTICSPLAYGROUND_FORCE_UPDATES 0

// ---------------------------------------------------------------------

#if BALLISCTICSPLAYGROUND_USE_OPENGL
#define BALLISCTICSPLAYGROUND_BASECLASS QGLWidget
#else
#define BALLISCTICSPLAYGROUND_BASECLASS QWidget
#endif

class BallisticsPlayground : public BALLISCTICSPLAYGROUND_BASECLASS {

  Q_OBJECT ;

  struct Pointer {
    BallisticsPlayground *master ;
    TransferFunction *func ;
    QStaticText name ;
    QPoint pos ;
    Pointer(const char *funcuri, BallisticsPlayground *playground) ; 
    Pointer(TransferFunction *func, BallisticsPlayground *playground) ; 
    void moveBy(int input_dx, int input_dy, TimeStamp::inttime timestamp) ;
    void moveBy(int input_dx, int input_dy, int & output_dx, int & output_dy, TimeStamp::inttime timestamp) ;
    void moveTo(QPoint p) ;
    ~Pointer(void) ;
  }  ;

  struct EventLogDetail {
      Pointer* activePointer;
      int input_dx;
      int input_dy;
      int output_dx;
      int output_dy;
      double cdgain;
  };

  EventLogDetail eventLogDetail;

  QPoint lpoint;

  PointingDevice *input ;
  DisplayDevice *output ;
  std::vector<SessionInfo> sessions;
  int blockNo = 0;
  int trialNo = 1;
  QTimer disptimer ;

  QsLogging::Logger& logger = QsLogging::Logger::instance();
  QsLogging::DestinationPtr logDest;

  double deltaSum = 0;
  double physDist = 0;
  double virtDist = 0;
  TimeStamp::inttime last_time = 0;

  Pointer* activePointer = 0;

  QTimer recordtimer ;
  QFile *recordfile ;
  QTextStream *record ;

  QStaticText iDesc, oDesc, pdDesc;
  QString instructDesc ;
  int buttons ;
  int lastbutton = 0;

  int artificialLatency ;

  typedef enum {NONE=0, SYSTEM=1, PSEUDO=2, SYSTEM_AND_PSEUDO=3} sysPointerVisualization ;
  sysPointerVisualization sysPointerViz ;
  QPoint sysPointerLoc ;

  int debugLevel;

  void reset(void) ;

  void keyPressEvent(QKeyEvent *event) ;
  void paintEvent(QPaintEvent *event) ;
  void mouseMoveEvent(QMouseEvent *event) ;
  void logEvent(int logLevel, EventLogDetail e);
  void logSummary(SessionInfo session);
  void logSession(SessionInfo session);
  void logInit();


  static void pointingCallback(void *context, TimeStamp::inttime timestamp, int dx, int dy, int buttons) ;

private slots:

  void pointingEvent(TimeStamp::inttime timestamp, int dx, int dy, int buttons) ;
  void quit();

public:

  BallisticsPlayground(PointingDevice *input, DisplayDevice *output, std::vector<SessionInfo> sessions,  QWidget *parent=0) ;
  
  void addPointer(const char *transferfunction) ;
  void addPointer(TransferFunction *function) ;
  void setBlockNo(int blockno);
  void setTrialNo(int trialno);

  ~BallisticsPlayground(void) ;

signals:

  void finish();

} ;

#endif
