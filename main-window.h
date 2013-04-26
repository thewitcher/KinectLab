#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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

    void initialize();

private slots:
    void startKinectThread();
};

#endif // MAINWINDOW_H
