        /* -*- mode: c++ -*-
     *
     * apps/ballistics/ballistics.cpp --
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

    #include <QApplication>
    #include <QStyleFactory>

    #include <iostream>
    #include <stdexcept>
    //#include <QMainWindow>
    #include <QTimer>
    #include <fstream>
    #include <iostream>
    #include <string>
    #include <vector>
    #include <QFileDialog>
    #include "sessionmanager.h"

    #include "../PointingLogger/QsLog.h"

    #ifdef _MSC_VER // Visual Studio C++
    #  define USE_GETOPT 0
    int optind = 1 ;
    #else
    #  include <getopt.h>
    #  define USE_GETOPT 1
    #endif

int
runApp(std::vector<SessionInfo> sessions, int blockno, int trialno) {

       try {

          QApplication::setStyle(QStyleFactory::create("plastique")) ;

          QApplication* app = qApp; //(0, NULL);


        bool transparent = false ;



    /* #if USE_GETOPT
        int ch ;
        while ((ch = getopt(argc, argv, "i:o:t")) != -1) {
          switch (ch) {
          case 'i': input_uri = optarg ; br eak ;
          case 'o': output_uri = optarg ; break ;
          case 't': transparent = true ; break ;
          default:
        std::cerr << "Usage: " << argv[0] << " [-i pointing-device-uri] [-o display-device-uri] [-t(ransparent window)]" << std::endl ;
        return -1 ;
          }
        }
    #endif */


        //argc -= optind ;
        //argv += optind ;


    #if BALLISCTICSPLAYGROUND_USE_OPENGL
        std::cerr << "OpenGL-based BallisticsPlayground will use a swap interval of 0" << std::endl ;
        QGLFormat fmt ;
        fmt.setDirectRendering(true) ;
        fmt.setSwapInterval(0) ;
        QGLFormat::setDefaultFormat(fmt) ;
    #endif

        PointingDevice *input = PointingDevice::create(sessions[0].inpuri) ;
        for (TimeStamp reftime, now;
         !input->isActive() && now-reftime<15*TimeStamp::one_second;
         now.refresh())
        PointingDevice::idle(500) ;



        DisplayDevice *output = DisplayDevice::create( sessions[0].outuri) ;

        BallisticsPlayground * playground = new BallisticsPlayground(input, output, sessions) ;

        playground->setBlockNo(blockno);
        playground->setTrialNo(trialno);
        //playground->addPointer(sessions[0].transferFunc.c_str()) ;



        if (transparent) {
    #if BALLISCTICSPLAYGROUND_USE_OPENGL
          std::cerr << "Warning: can't set transparent background with OpenGL-based BallisticsPlayground" << std::endl ;
    #else
          playground->setAttribute(Qt::WA_TranslucentBackground, true) ;
          playground->setWindowFlags(Qt::WindowStaysOnTopHint) ;
          playground->setFocusPolicy(Qt::NoFocus) ;
    #endif
        }

        playground->showFullScreen() ;


        playground->setCursor(Qt::BlankCursor);
        if (!transparent) playground->raise() ;


       int result = qApp->exec() ;

        delete playground;
       //delete app;
        return result;


      } catch (std::runtime_error e) {
        std::cerr << "Runtime error: " << e.what() << std::endl ;
      } catch (std::exception e) {
        std::cerr << "Exception: " << e.what() << std::endl ;
      }


      return -1 ;
}


int main(int argc, char** argv) {
    QApplication app(argc, argv);

    SessionDialog sDialog;

    sDialog.prepareSlots();

    sDialog.exec();



     if (sDialog.proceedExp){
        app.quit();


        FILE * fName = fopen(sDialog.getConfFileName().toStdString().c_str(), "r");
         //const char* pname = sDialog.getParticipantName().toStdString().c_str();

         std::vector<SessionInfo> sessions;

         if (fName)

         {



          while (true)
          {
              char buffer[10000];
              if (fgets(buffer, 10000, fName)==NULL) break;

              SessionInfo session; // = new SessionInfo();
              session.participantName = sDialog.getParticipantName();

              session.inpuri = std::string(strdup(strtok(buffer, " ")));
              session.outuri = std::string(strdup(strtok(NULL, " ")));
              session.numTrials = atoi(strtok(NULL, " "));

              session.instruction = QString::fromStdString(strdup(strtok(NULL, " ")));
              session.isTraining = (std::string(strtok(NULL, " "))=="Train")?true:false;
              session.transferFunc = strdup(strtok(NULL, " "));
              //std::cout<<"tf: "<<session.transferFunc;
              //std::cout<<"name "<<session.outuri<<std::endl;

              //session.targetL = atoi(strtok(NULL, " "));
              session.targetW = atoi(strtok(NULL, " "));
              session.targetA = atoi(strtok(NULL, "\n"));
              //session.targetR = session.targetL + session.targetA;
              //session.targetLeftActive = true;
              sessions.push_back(session);
              //session->~SessionInfo();

        }
         fclose(fName);
         }
        //std::cout<<"name here "<<std::endl;


         runApp(sessions, sDialog.getBlockNo(), sDialog.getTrialNo());

     }

    //return app.exec();


}
