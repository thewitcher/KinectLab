#include <QDebug>
#include <QSettings>

#include "main-window.h"
#include "ui_main-window.h"

#include "kinect-runner.h"

/**
 * @brief MainWindow::MainWindow. Constructor. Creates new KinectRunner object in order to managing kinect in different thread. This method also calls
 *        initialize function for preparing all windows and createing all necessary connections.
 *
 * @author Marcin Haber
 * @date 06/05/2013
 */
MainWindow::MainWindow( QWidget * parent ):
    QMainWindow( parent ),
    ui( new Ui::MainWindow ),
    m_pKinnectRunner( new KinectRunner )
{
    ui->setupUi( this );

    initialize();
}

/**
 * @brief MainWindow::MainWindow. Main reason for creating this method is to create connections between objects and slots. Before this is done some initializing
 *        function are called in order to initializing about and help page. Initialize function is responisble for preparing and showing main window frame.
 *
 * @author Marcin Haber
 * @date 06/05/2013
 */
void MainWindow::initialize()
{
    helpInitialize();
    aboutInitialize();
    showMainPage();

    connect(ui->m_pStartButton,SIGNAL(clicked()),this,SLOT(startKinectThread()));
    connect(ui->m_pExitButton,SIGNAL(clicked()),this,SLOT(close()));
    connect(ui->m_pAboutButton,SIGNAL(clicked()),this,SLOT(showAboutPage()));
    connect(ui->m_pBackFromAboutButton,SIGNAL(clicked()),this,SLOT(showMainPage()));
    connect(ui->m_pBackFromOptionsButton,SIGNAL(clicked()),this,SLOT(showMainPage()));
    connect(ui->m_pBackFromOptionsButton,SIGNAL(clicked()),this,SLOT(settingsUpdate()));
    connect(ui->m_pOptionsButton,SIGNAL(clicked()),this,SLOT(showOptionsPage()));
    connect(ui->m_pHelpButton,SIGNAL(clicked()),this,SLOT(showHelpPage()));
    connect(ui->m_pBackFromHelpButton,SIGNAL(clicked()),this,SLOT(showOptionsPage()));
    connect(ui->m_pDefaultButton,SIGNAL(clicked()),this,SLOT(backToDeafaultValues()));
    connect(ui->m_pStopButton,SIGNAL(clicked()),this,SLOT(stopKinectThread()));
    connect(m_pKinnectRunner,SIGNAL(newKinectDetailsSignal(KinectDetails)),this,SLOT(updateKinectStatus(KinectDetails)));
}

/**
 * @brief MainWindow::~MainWindow. Destructor. Just freeing memory which was reserved earlier.
 *
 * @author Marcin Haber
 * @date 06/05/2013
 */
MainWindow::~MainWindow()
{
    delete ui;
    delete m_pKinnectRunner;
}

/**
 * @brief MainWindow::updateKinectStatus. This function is connected to signal from KinectRunner. It updates current kinect status on the screen.
 *
 * @author Marcin Haber
 * @date 06/05/2013
 */
void MainWindow::updateKinectStatus( const KinectDetails & kinectDetails )
{
    if( kinectDetails.value( "connection" ) == false )
    {
        ui->m_pConnectionStatusLabel->setText( "<h3><font color=red> Kinect connection status: disconnected </color>" );
    }
    else
    {
        ui->m_pConnectionStatusLabel->setText( "<h3><font color=green> Kinect connection status: connected </color>" );
    }

    if( kinectDetails.value( "skeleton_found" ) == true )
    {
        ui->m_pSkeletonTrackingStatusLabel->setText( "<h3><font color=green> Skeleton tracking status: skeleton was chosen and it is tracked </color>" );
    }
    else
    {
        ui->m_pSkeletonTrackingStatusLabel->setText( "<h3><font color=red> Skeleton tracking status: skeleton was lost </color>" );
    }

    if( kinectDetails.value( "sliding" ) == true )
    {
        ui->m_pSlidingLabel->setText( "<h3><font color=green> Sliding status: enabled </color>" );
    }
    else
    {
        ui->m_pSlidingLabel->setText( "<h3><font color=red> Sliding status: disabled </color>" );
    }
}

/**
 * @brief MainWindow::startKinectThread. Starts kinect thread and shows run page.
 *
 * @author Marcin Haber
 * @date 06/05/2013
 */
void MainWindow::startKinectThread()
{
    qDebug() << "Starts kinect thread";

    showRunPage();

    m_pKinnectRunner->start();
}

/**
 * @brief MainWindow::stopKinectThread. Kills kinect thread if it still runs.
 *
 * @author Marcin Haber
 * @date 06/05/2013
 */
void MainWindow::stopKinectThread()
{
    qDebug() << "Stops kinect thread";

    showMainPage();

    if( m_pKinnectRunner->isRunning() == true )
    {
        qDebug() << "Kinnect thread is running so it will be terminated before closing main window";
        m_pKinnectRunner->terminate();
    }
}

/**
 * @brief MainWindow::closeEvent. Reimplemented virtual method from QWidget. Terminates kinect thread and updates settings before application will be closed.
 *
 * @author Marcin Haber
 * @date 06/05/2013
 */
void MainWindow::closeEvent( QCloseEvent * event )
{
    qDebug() << "Closing main window";

    if( m_pKinnectRunner->isRunning() == true )
    {
        qDebug() << "Kinnect thread is running so it will be terminated before closing main window";
        m_pKinnectRunner->terminate();
    }

    settingsUpdate();

    QMainWindow::closeEvent( event );
}

/**
 * @brief MainWindow::showAboutPage. Shows about page. Updates settings.
 *
 * @author Marcin Haber
 * @date 06/05/2013
 */
void MainWindow::showAboutPage()
{
    settingsUpdate();

    ui->m_pStackedWidget->setCurrentIndex( ABOUT );
}

/**
 * @brief MainWindow::showRunPage. Shows run page.
 *
 * @author Marcin Haber
 * @date 06/05/2013
 */
void MainWindow::showRunPage()
{
    ui->m_pRunPageLabel->setText( "<h2>Run page" );
    ui->m_pConnectionStatusLabel->setText( "<h3><font color=red>Kinect status: kinect is disconnected now</color>" );
    ui->m_pSkeletonTrackingStatusLabel->setText( "<h3><font color=red>Skeleton tracking status: no sekelton tracking</color>" );

    ui->m_pStackedWidget->setCurrentIndex( RUN );
}

/**
 * @brief MainWindow::showOptionsPage. Shows options page. Reads options from configuration file. If file does not exist then options are set to default values.
 *
 * @author Marcin Haber
 * @date 06/05/2013
 */
void MainWindow::showOptionsPage()
{
    qDebug() << "showOptionsPage()";

    // load settings
    QSettings settings( "settings.ini", QSettings::IniFormat );

    settings.beginGroup( "AlgorithmParameters" );
    ui->m_pPrayTimeSpinBox->setValue( settings.value( "prayTime", 5000  ).toInt() );
    ui->m_pSlideSpeedSpinBox->setValue( settings.value( "slideSpeed", 200  ).toInt() );
    ui->m_pRightRangeSpinBox->setValue( settings.value( "slideXRangeOnTheRight", 0.15  ).toDouble() );
    ui->m_pLeftRangeSpinBox->setValue( settings.value( "slideXRangeOnTheLeft", -0.15  ).toDouble() );
    ui->m_pXDeltaSpinBox->setValue( settings.value( "xDelta", 0.20  ).toDouble() );
    ui->m_pYDeltaSpinBox->setValue( settings.value( "yDelta", 0.50  ).toDouble() );
    settings.endGroup();

    ui->m_pStackedWidget->setCurrentIndex( OPTIONS );
}

/**
 * @brief MainWindow::showMainPage. Shows main page.
 *
 * @author Marcin Haber
 * @date 06/05/2013
 */
void MainWindow::showMainPage()
{
    ui->m_pStackedWidget->setCurrentIndex( MAIN );
}

/**
 * @brief MainWindow::showHelpPage. Shows help page.
 *
 * @author Marcin Haber
 * @date 06/05/2013
 */
void MainWindow::showHelpPage()
{
    ui->m_pStackedWidget->setCurrentIndex( HELP );
}

/**
 * @brief MainWindow::settingsUpdate. Writes all setting/options into settings file.
 *
 * @author Marcin Haber
 * @date 06/05/2013
 */
void MainWindow::settingsUpdate()
{
    QSettings settings( "settings.ini", QSettings::IniFormat );

    settings.beginGroup( "AlgorithmParameters" );
    settings.setValue( "prayTime", ui->m_pPrayTimeSpinBox->value() );
    settings.setValue( "slideSpeed", ui->m_pSlideSpeedSpinBox->value() );
    settings.setValue( "slideXRangeOnTheRight", ui->m_pRightRangeSpinBox->value() );
    settings.setValue( "slideXRangeOnTheLeft", ui->m_pLeftRangeSpinBox->value() );
    settings.setValue( "xDelta", ui->m_pXDeltaSpinBox->value() );
    settings.setValue( "yDelta", ui->m_pYDeltaSpinBox->value() );
    settings.endGroup();
}

/**
 * @brief MainWindow::backToDeafaultValues. Set options values in options page to defaults.
 *
 * @author Marcin Haber
 * @date 06/05/2013
 */
void MainWindow::backToDeafaultValues()
{
    ui->m_pPrayTimeSpinBox->setValue( 5000 );
    ui->m_pSlideSpeedSpinBox->setValue( 200 );
    ui->m_pRightRangeSpinBox->setValue( 0.15 );
    ui->m_pLeftRangeSpinBox->setValue( -0.15 );
    ui->m_pXDeltaSpinBox->setValue( 0.20 );
    ui->m_pYDeltaSpinBox->setValue( 0.50 );
}

/**
 * @brief MainWindow::helpInitialize. Prepares help page.
 *
 * @author Marcin Haber
 * @date 06/05/2013
 */
void MainWindow::helpInitialize()
{
    ui->m_pHelpLabel->setText( "<h3>This is a short description how to understand all options in options page:</h3><br>"
                               "<h3>Pray time</h3> - it is a time to keep hands near each other for switching on/off sliding mode<br>"
                               "<h3>Slide speed</h3> - how fast you should move your right hand in order to change slide<br>"
                               "<h3>Slide x range on the right</h3> - how far you should move your hand in right direction in order to change slide<br>"
                               "<h3>Slide y range on the left</h3> - how far you should move your hand in left direction in order to change slide<br>"
                               "<h3>x delta</h3> - it is a difference between left hand x position and right hand x position. It defines how near hands are in horizontal<br>"
                               "<h3>y delta</h3> - it is a difference between left hand y position and right hand y position. It defines how near hands are in vertical<br>");
}

/**
 * @brief MainWindow::aboutInitialize. Prepares about page.
 *
 * @author Marcin Haber
 * @date 06/05/2013
 */
void MainWindow::aboutInitialize()
{
    ui->m_pAboutLabel->setText( "<h1>Application for hands slide detection</h1>" );
    ui->m_pPruftechnikLogoLabel->setText( "Logo" );
    ui->m_pAuthorDetailsLabel->setText( "Designed by Marcin Haber from Pruftechnik Technology" );
}
