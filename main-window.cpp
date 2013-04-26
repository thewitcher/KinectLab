#include "main-window.h"
#include "ui_main-window.h"

#include <QDebug>

#include "kinect-runner.h"

MainWindow::MainWindow( QWidget * parent ):
    QMainWindow( parent ),
    ui( new Ui::MainWindow ),
    m_pKinnectRunner( new KinectRunner )
{
    ui->setupUi( this );

    initialize();
}

void MainWindow::initialize()
{
    connect(ui->m_pStartButton,SIGNAL(clicked()),this,SLOT(startKinectThread()));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_pKinnectRunner;
}

void MainWindow::startKinectThread()
{
    qDebug() << "Starts kinect thread";

    m_pKinnectRunner->start();
}
