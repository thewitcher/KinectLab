#ifndef MAINWINDOW_H
#define MAINWINDOW_H

/**
  * @file  main-window.h
  * @brief Main window contains all necessary widgets. It is used to start/stop kinect and to show current status. There is also
  *        page "About" and "Help" in order to give some help information to users. Advanced users can find out that there are some
  *        options in "Options Page". Default values are enough to run application in standard way. It is recommended not to change
  *        these option unless it is not necessary.
  *
  * @author Marcin Haber
  * @date 06/05/2013
  */

#include <QMainWindow>
#include "kinect-details.h"

namespace Ui { class MainWindow; }

class KinectRunner;

class MainWindow : public QMainWindow
{

    Q_OBJECT
    
public:
    explicit MainWindow( QWidget * parent = 0 );
    ~MainWindow();
    
private:
    Ui::MainWindow * ui;
    KinectRunner * m_pKinnectRunner;

    enum STACKED_WIDGET_PAGE { MAIN = 0, OPTIONS = 1, ABOUT = 2, HELP = 3, RUN = 4 };

    void initialize();
    void helpInitialize();
    void aboutInitialize();

private slots:
    void startKinectThread();
    void stopKinectThread();
    void closeEvent( QCloseEvent * event );
    void showAboutPage();
    void showOptionsPage();
    void showMainPage();
    void showHelpPage();
    void showRunPage();
    void settingsUpdate();
    void updateKinectStatus( const KinectDetails & kinectDetails );

    void backToDeafaultValues();
};

#endif // MAINWINDOW_H
