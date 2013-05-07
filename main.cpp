#include <QApplication>
#include "main-window.h"
#include "kinect-details.h"

/**
 * @brief main.cpp Runs QApplication event loop and shows main window. It also register some types in order to have possibility to keep that data
 *        in thread queue (when signals are sending from one thread to another)
 *
 * @author Marcin Haber
 * @date 06/05/2013
 */
int main( int argc, char * argv[] )
{
    QApplication a( argc, argv );

    qRegisterMetaType<KinectDetails>( "KinectDetails" );

    MainWindow w;
    w.show();

    return a.exec();
}
