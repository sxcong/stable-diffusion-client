/*
 * QPhotoView viewer widget.
 *
 * License: GPL V2. See file COPYING for details.
 *
 * Author:  Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#include <QApplication>
#include <QGraphicsPixmapItem>
#include <QResizeEvent>
#include <QKeyEvent>
#include <QTime>
#include <QDesktopWidget>
#include <QStyle>
#include <QDebug>

#include "PhotoView.h"
#include "PhotoDir.h"
#include "Photo.h"
#include "Canvas.h"
#include "Panner.h"
#include "SensitiveBorder.h"
#include "BorderPanel.h"
#include "TextBorderPanel.h"
#include "ExifBorderPanel.h"
//#include "Logger.h"
#include "imagefilemanager.h"
#include "exifinfo.h"
#include "mainwidget.h"


static const int DefaultIdleTimeout = 4000; // millisec


PhotoView::PhotoView( MainWidget* pMain )
    : QGraphicsView()
{
    m_pMainWidget = pMain;
    _lastPhoto = 0;
    _zoomMode = ZoomFitImage;
    _zoomFactor = 1.0;
    _zoomIncrement = 1.2;
    _idleTimeout = DefaultIdleTimeout;

    setGeometry(0, 0, 640, 480);
    setScene( new QGraphicsScene );
    setSceneRect( 0, 0, 640, 480);


    _actions = new Actions(this);
    //Q_CHECK_PTR( photoDir );

    _canvas = new Canvas( this );

    createBorders();
    layoutBorders(QSizeF(640, 480));

    //QSize pannerMaxSize( qApp->desktop()->screenGeometry().size() / 6 );
    //_panner = new Panner( pannerMaxSize, this );

    createPanels();

    //
    // Visual tweaks
    //

    QPalette pal = palette();
    pal.setColor( QPalette::Base, Qt::black );
    setPalette( pal );
    setFrameStyle( QFrame::NoFrame );

    setVerticalScrollBarPolicy	( Qt::ScrollBarAlwaysOff );
    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );


    // Enable mouse tracking so a mouse cursor that was set invisible by an
    // item (e.g., Canvas) can be made visible again upon mouse movement.
    setMouseTracking( true );

    connect( &_idleTimer, SIGNAL( timeout()    ),
         this,	   SLOT	 ( hideCursor() ) );

    _idleTimer.setSingleShot( true );
    _idleTimer.start( _idleTimeout );
    _cursor = viewport()->cursor();
}


PhotoView::~PhotoView()
{
    if ( style() != qApp->style() )
    {
    // Delete the style we explicitly created just for this widget
    delete style();
    }

    delete scene();
}


static QString formatBytes(qint64 bytes)
{
    QString sizeString;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    QLocale locale;
    sizeString = locale.formattedDataSize(bytes);
#else
    double size = bytes;

    int reductionAmount = 0;
    double newSize = size/1024;
    while (newSize > 1024)
    {
        newSize /= 1024;
        reductionAmount++;
        if (reductionAmount > 2)
            break;
    }

    QString unit;
    switch (reductionAmount)
    {
    case 0: {
        unit = " KiB";
        break;
    }
    case 1: {
        unit = " MiB";
        break;
    }
    case 2: {
        unit = " GiB";
        break;
    }
    case 3: {
        unit = " TiB";
        break;
    }
    }

    sizeString = QString::number(newSize, 'f', 2) + unit;
#endif
    return sizeString;
}

void PhotoView::loadImageFile(const QString& fileName, bool bRefreshDir)
{
    m_fileInfo = QFileInfo(fileName);
   // qDebug()<<m_fileInfo.absolutePath()<<m_fileInfo.absoluteFilePath();//C:/Users/Tiger/Pictures/1.jpg
    if (m_fileInfo.isFile())
    {
        QString path = m_fileInfo.absolutePath();//C:/Users/Tiger/Pictures
        //ImageFileManager::Instance().setFolder(path);
        ImageFileManager::Instance().setFile(fileName, bRefreshDir);
        QString szFile = ImageFileManager::Instance().getCurrentFile();
        m_isPixmapLoaded = ImageUtils::Instance().readFile(szFile, m_readData);


            //emit updatedLoadedPixmapItem();
            emit signal_updatedLoadedPixmapItem(szFile);
            _canvas->setPixmap(m_readData.pixmap);
            _pixmap = m_readData.pixmap;
            setSceneRect( 0, 0, size().width(), size().height());
            //setWindowTitle
            reloadCurrent(size());

    }
}

int PhotoView::loadMimeData(const QMimeData *mimeData)
{
    if (mimeData == nullptr)
        return -1;

    if (!mimeData->hasUrls())
        return -2;

    const QList<QUrl> urlList = mimeData->urls();

    bool first = true;
    for (const auto &url : urlList)
    {
        QString szFile;
        if (url.isLocalFile())
        {
            szFile = url.toLocalFile();
            loadImageFile(szFile, true);

            return 0;
        }
        else
        {
            return -1;
        }
    }
    return 0;
}

void PhotoView::wheelEvent(QWheelEvent *event)
{
   // bool willZoom = event->modifiers() == Qt::ControlModifier || event->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier);
   // if (!willZoom)
   //     return;

    const int yDelta = event->angleDelta().y();
    if (yDelta == 0)
        return;
    if (yDelta < 0)
    {
        zoomOut();
    }
    else
    {
        zoomIn();
    }
}

void PhotoView::dropEvent(QDropEvent *event)
{
    QGraphicsView::dropEvent(event);
    int ret = loadMimeData(event->mimeData());
    qDebug()<<"dropEvent"<<ret;
}

void PhotoView::dragEnterEvent(QDragEnterEvent *event)
{
    QGraphicsView::dragEnterEvent(event);
    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
}

void PhotoView::dragMoveEvent(QDragMoveEvent *event)
{
    QGraphicsView::dragMoveEvent(event);
    event->acceptProposedAction();
}

void PhotoView::dragLeaveEvent(QDragLeaveEvent *event)
{
    QGraphicsView::dragLeaveEvent(event);
    event->accept();
}

void PhotoView::clear()
{
    _canvas->clear();
    setWindowTitle( "QPhotoView" );
}


void PhotoView::resizeEvent ( QResizeEvent * event )
{
    //qDebug()<<" event->size()"<< event->size();
    if ( event->size() != event->oldSize() )
    {
        layoutBorders( event->size() );
        reloadCurrent( event->size() );
    }
}


QPixmap scaleImage( QPixmap & origPixmap, qreal scaleFactor )
{
    if ( qFuzzyCompare( scaleFactor, 1.0 ) )
        return origPixmap;

    if ( origPixmap.isNull() )
        return origPixmap;

    QPixmap pixmap( origPixmap );

    pixmap = pixmap.scaled( scaleFactor * origPixmap.size(),
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation );

    return pixmap;
}


qreal scaleFactor( const QSize & origSize, const QSize & boundingSize )
{
    if ( origSize.width() == 0 || origSize.height() == 0 )
    return 0.0;

    qreal scaleFactorX = boundingSize.width()  / ( (qreal) origSize.width()  );
    qreal scaleFactorY = boundingSize.height() / ( (qreal) origSize.height() );

    return qMin( scaleFactorX, scaleFactorY );
}


QPixmap PhotoView::getPixmap(const QSize & size)
{
    QPixmap scaledPixmap;

    if ( _pixmap.isNull() )
    {
        return _pixmap;
    }

    qreal scaleFac = scaleFactor( _pixmap.size(), size );

    if ( scaleFac <= 1.0 ) // not larger than cached pixmap
    {
        scaledPixmap = scaleImage(_pixmap, scaleFac);
    }
    else // larger than cached pixmap
    {
        scaledPixmap = _pixmap;
        scaleFac     = scaleFactor( scaledPixmap.size(), size );
        scaledPixmap = scaleImage( scaledPixmap, scaleFac );
    }

    return scaledPixmap;
}


QPixmap PhotoView::getPixmap2( const QSizeF & size )
{
    return getPixmap( QSize( qRound( size.width()	),
              qRound( size.height() ) ) );
}

bool PhotoView::reloadCurrent( const QSize & size )
{
   // qDebug()<<"reloadCurrent"<<m_isPixmapLoaded<<size<<_zoomMode;
    if (!m_isPixmapLoaded)
        return false;
    bool success = true;

    QPixmap pixmap;
    QSizeF origSize = m_readData.pixmap.size();

    switch ( _zoomMode )
    {
    case NoZoom:
        qDebug()<<"NoZoom"<<origSize;
        pixmap = m_readData.pixmap;
        _zoomFactor = 1.0;
        break;


    case ZoomFitImage:
        pixmap = getPixmap(size);

        if ( origSize.width() != 0 )
         _zoomFactor = pixmap.width() / origSize.width();
        break;


 /*   case ZoomFitWidth:

        if ( origSize.width() != 0 )
        {
             _zoomFactor = size.width() / origSize.width();
             pixmap = getPixmap(_zoomFactor * origSize);
        }
        break;


    case ZoomFitHeight:

        if ( origSize.height() != 0 )
        {
        _zoomFactor = size.height() / origSize.height();
        pixmap = photo->pixmap( _zoomFactor * origSize );
        }
        break;


    case ZoomFitBest:

        if ( origSize.width() != 0 && origSize.height() != 0 )
        {
        qreal zoomFactorX = size.width()  / origSize.width();
        qreal zoomFactorY = size.height() / origSize.height();
        _zoomFactor = qMax( zoomFactorX, zoomFactorY );

        pixmap = photo->pixmap( _zoomFactor * origSize );
        }
        break;
*/
    case UseZoomFactor:
        //qDebug()<<"UseZoomFactor"<<_zoomFactor*origSize;
        pixmap = getPixmap2(_zoomFactor * origSize);
        break;

        // Deliberately omitting 'default' branch so the compiler will warn
        // about unhandled enum values
    };

    _canvas->setPixmap( pixmap );
    success = ! pixmap.isNull();

    if ( success )
    {
        //updatePanner(size);
        _canvas->fixPosAnimated( false ); // not animated
    }

    setSceneRect( 0, 0, size.width(), size.height() );

    return success;
}

void PhotoView::updatePanner( const QSizeF & vpSize )
{
    /*
    if (m_pPhotoDir == nullptr)
        return ;

    QSizeF viewportSize = vpSize;

    if ( ! viewportSize.isValid() )
    viewportSize = size();

    if ( viewportSize.width()  < _panner->size().width()  * 2  ||
     viewportSize.height() < _panner->size().height() * 2  )
    {
    // If the panner would take up more than half the available space
    // in any direction, don't show it.

    _panner->hide();
    }
    else
    {
    Photo * photo = m_pPhotoDir->current();

    if ( ! photo )
    {
        _panner->hide();
    }
    else
    {
        QSizeF  origSize   = photo->size();
        QPointF canvasPos  = _canvas->pos();
        QSizeF  canvasSize = _canvas->size();

        qreal   pannerX    = 0.0;
        qreal   pannerY    = -_panner->size().height();

        if ( canvasSize.width() < viewportSize.width() )
        pannerX = canvasPos.x();

        if ( canvasSize.height() < viewportSize.height() )
        pannerY += canvasPos.y() + canvasSize.height();
        else
        pannerY += viewportSize.height();

        _panner->setPos( pannerX, pannerY );

        QPointF visiblePos( -canvasPos.x(), -canvasPos.y() );

        QSizeF visibleSize( qMin( viewportSize.width(),
                      canvasSize.width()  ),
                qMin( viewportSize.height(),
                      canvasSize.height() ) );

        QRectF visibleRect( visiblePos  / _zoomFactor,
                visibleSize / _zoomFactor );

        _panner->updatePanRect( visibleRect, origSize );
    }
    }
    */
}


void PhotoView::createBorders()
{
    _rightBorder     = createBorder( "RightBorder"	   );
/*    _topBorder	       = createBorder( "TopBorder"	   );
    _topRightCorner    = createBorder( "TopRightCorner"	   );
    _rightBorder       = createBorder( "RightBorder"	   );
    _bottomRightCorner = createBorder( "BottomRightCorner" );
    _bottomBorder      = createBorder( "BottomBorder"	   );
    _bottomLeftCorner  = createBorder( "BottomLeftCorner"  );
    _leftBorder	       = createBorder( "LeftBorder"	   );
*/
 //   _rightBorder->setBrush(QBrush(QColor(255, 0, 0)));

}


SensitiveBorder * PhotoView::createBorder( const QString & objName )
{
    SensitiveBorder * border = new SensitiveBorder( this );
    border->setObjectName( objName );

#if 0
    connect( border, SIGNAL( borderEntered() ),
         this,   SLOT  ( showBorder()    ) );

    connect( border, SIGNAL( borderLeft()    ),
         this,   SLOT  ( hideBorder()    ) );
#endif

    border->hide();
    return border;
}


void PhotoView::layoutBorders( const QSizeF & size )
{
    if (_rightBorder == nullptr)
        return;
    qreal top	    = 0.0;
    qreal left	    = 0.0;
    qreal width	    = size.width();
    qreal height    = size.height();
    qreal thickness = 500.0;

   /* if ( width < 4 * thickness )
    thickness = width / 4;

    if ( height < 4 * thickness )
    thickness = height / 4;*/


    _rightBorder->setRect(width - thickness, 50,
               thickness-10, 600);
/*
   _topBorder->setRect( thickness, top,
             width - 2 * thickness, thickness );

    _topRightCorner->setRect( width - thickness, top,
                  thickness, thickness );

    _rightBorder->setRect( width - thickness, thickness,
               thickness, height - 2 * thickness );

    qDebug()<<"_rightBorder->setRect();"<<width - thickness<< thickness<< thickness<< height - 2 * thickness;

    _bottomRightCorner->setRect( width - thickness, height - thickness,
                 thickness, thickness );

    _bottomBorder->setRect( thickness, height - thickness,
                width - 2 * thickness, thickness );

    _bottomLeftCorner->setRect( left, height - thickness,
                thickness, thickness );

    _leftBorder->setRect( left, thickness,
              thickness, height - 2 * thickness );
*/

    //_exifPanel->appearNow();
    //_titlePanel->appearAnimated();
    //_exifPanel->appearAnimated();
    //_navigationPanel->appearAnimated();
    //_toolPanel->appearAnimated();

    if (_exifPanel)
        _exifPanel->setPos( _rightBorder->rect().topLeft());
}


void PhotoView::createPanels()
{
 /*   _titlePanel = new TextBorderPanel( this, _topRightCorner );
    _titlePanel->setSize( 500, 50 );
    _titlePanel->setBrush(QBrush(QColor(0, 160, 230)));
    _titlePanel->setBorderFlags( BorderPanel::RightBorder |
                                 BorderPanel::TopBorder );
*/
    _exifPanel = new TextBorderPanel( this, _rightBorder );
    _exifPanel->setSize( _rightBorder->rect().width(), _rightBorder->rect().height() );
    //_exifPanel->setPos(100, 100);
    _exifPanel->setBorderFlags( BorderPanel::CenterInView );
    //_exifPanel->setBorderFlags( BorderPanel::LeftBorder|BorderPanel::TopBorder );
    _exifPanel->setAlignment( Qt::AlignVCenter|Qt::AlignLeft );
    _exifPanel->hide();
    //_exifPanel->appearNow();

 /*   _navigationPanel = new BorderPanel( this, _bottomBorder );
    _navigationPanel->setSize( 400, 100 );
    _navigationPanel->setBorderFlags( BorderPanel::BottomBorder );
    _navigationPanel->setAlignment( Qt::AlignRight );
    _navigationPanel->setToolTip("_navigationPanel");

    _toolPanel = new TextBorderPanel( this, _leftBorder );
    _toolPanel->setText("_toolPanel");
    _toolPanel->setSize( 100, 400 );
    _toolPanel->setBorderFlags( BorderPanel::LeftBorder );
    _toolPanel->setAlignment( Qt::AlignTop );*/

}


void PhotoView::showBorder()
{
    if ( sender() )
    {
    //logDebug() << "Show border " << sender()->objectName() << endl;
    }
}


void PhotoView::hideBorder()
{
    if ( sender() )
    {
    //logDebug() << "Hide border " << sender()->objectName() << endl;
    }
}


void PhotoView::setIdleTimeout( int millisec )
{
    _idleTimeout = millisec;

    if ( _idleTimeout > 0 )
    _idleTimer.start( _idleTimeout );
    else
    _idleTimer.stop();
}


void PhotoView::setZoomMode( ZoomMode mode )
{
    _zoomMode = mode;
    reloadCurrent( size() );
}


void PhotoView::setZoomMode()
{
    QAction * action = qobject_cast<QAction *>( sender() );

    if ( action )
    {
        int zoomMode = action->data().toInt();
        setZoomMode( static_cast<ZoomMode>( zoomMode ) );
    }
}


void PhotoView::setZoomFactor( qreal factor )
{
    _zoomFactor = factor;

    if ( qFuzzyCompare( _zoomFactor, 1.0 ) )
    setZoomMode( NoZoom );
    else
    setZoomMode( UseZoomFactor );
}

void PhotoView::noZoom()
{
    setZoomFactor(1.0);
}

void PhotoView::zoomIn()
{
    if ( ! qFuzzyCompare( _zoomIncrement, 0.0 ) )
    setZoomFactor( _zoomFactor * _zoomIncrement );
}


void PhotoView::zoomOut()
{
    if ( ! qFuzzyCompare( _zoomIncrement, 0.0 ) )
    setZoomFactor( _zoomFactor / _zoomIncrement );
}


void PhotoView::hideCursor()
{
    // logDebug() << endl;
    _cursor = viewport()->cursor();
    viewport()->setCursor( Qt::BlankCursor );
    _canvas->hideCursor();
}


void PhotoView::showCursor()
{
    viewport()->setCursor( _cursor );
    _canvas->showCursor();
}


void PhotoView::toggleFullscreen()
{
    emit signal_full_screen();
    return;
}


void PhotoView::forceReload()
{
    /*
    if (m_pPhotoDir == nullptr)
        return ;
    Photo * photo = m_pPhotoDir->current();

    if ( photo )
    {
        photo->dropCache();
        loadImage();
    }
    */
}


void PhotoView::navigate( NavigationTarget where )
{
 /*   if (m_pPhotoDir == nullptr)
        return ;

    switch ( where )
    {
        case NavigateCurrent:                            break;
        case NavigateNext:      m_pPhotoDir->toNext();     break;
        case NavigatePrevious:  m_pPhotoDir->toPrevious(); break;
        case NavigateFirst:     m_pPhotoDir->toFirst();    break;
        case NavigateLast:      m_pPhotoDir->toLast();     break;
    }
*/
  //  loadImage();
}


void PhotoView::navigate()
{

    QAction * action = qobject_cast<QAction *>( sender() );
    qDebug()<<"navigate()"<<action->data();

    if ( action )
    {
        int where = action->data().toInt();
        navigate( static_cast<NavigationTarget>( where ) );
    }
}


void PhotoView::mouseMoveEvent ( QMouseEvent * event )
{
    // logDebug() << endl;
    _idleTimer.start( _idleTimeout );
    showCursor();

    QGraphicsView::mouseMoveEvent( event );
}


void PhotoView::keyPressEvent( QKeyEvent * event )
{
    if ( ! event )
    return;

    switch ( event->key() )
    {
    case Qt::Key_2:	       setZoomFactor( 2.0 );	break;
    case Qt::Key_3:	       setZoomFactor( 3.0 );	break;
    case Qt::Key_4:	       setZoomFactor( 4.0 );	break;
    case Qt::Key_5:	       setZoomFactor( 5.0 );	break;
    case Qt::Key_6:	       setZoomFactor( 6.0 );	break;
    case Qt::Key_7:	       setZoomFactor( 7.0 );	break;
    case Qt::Key_8:	       setZoomFactor( 8.0 );	break;
    case Qt::Key_9:	       setZoomFactor( 9.0 );	break;
    case Qt::Key_0:	       setZoomFactor( 10.0 );   break;

    /*case Qt::Key_Y:
        {
        const int max=10;
        //logInfo() << "*** Benchmark start" << endl;
        QTime time;
        time.start();

        for ( int i=0; i < max; ++i )
        {
            m_pPhotoDir->toNext();
            loadImage();
        }
       // logInfo() << "*** Benchmark end; time: "
       //                   << time.elapsed() / 1000.0 << " sec / "
       //                   << max << " images"
       //                   << endl;
        }
        break;
        */

    default:
        QGraphicsView::keyPressEvent( event );
    }

}


#define CONNECT_ACTION(ACTION, RECEIVER, RCVR_SLOT) \
    connect( (ACTION), SIGNAL( triggered() ), (RECEIVER), SLOT( RCVR_SLOT ) )


PhotoView::Actions::Actions( PhotoView * photoView ):
    _photoView( photoView )
{
    //
    // Zoom
    //

    noZoom = createAction( tr( "No Zoom (100% / &1:1)" ), Qt::Key_1, NoZoom );
    CONNECT_ACTION( noZoom, photoView, setZoomMode() );

    zoomIn = createAction( tr( "Zoom In (Enlarge)" ), Qt::Key_Plus );
    CONNECT_ACTION( zoomIn, photoView, zoomIn() );

    zoomOut = createAction( tr( "Zoom Out (Shrink)" ), Qt::Key_Minus );
    CONNECT_ACTION( zoomOut, photoView, zoomOut() );

    zoomFitImage = createAction( tr( "Zoom to &Fit Window" ), Qt::Key_F, ZoomFitImage );
    addShortcut( zoomFitImage, Qt::Key_M );
    CONNECT_ACTION( zoomFitImage, photoView, setZoomMode() );

    zoomFitWidth = createAction( tr( "Zoom to Fit Window &Width" ), Qt::Key_W, ZoomFitWidth );
    CONNECT_ACTION( zoomFitWidth, photoView, setZoomMode() );

    zoomFitHeight = createAction( tr( "Zoom to Fit Window &Height" ), Qt::Key_H, ZoomFitHeight );
    CONNECT_ACTION( zoomFitHeight, photoView, setZoomMode() );

    zoomFitBest = createAction( tr( "&Best Zoom for Window Width or Height" ), Qt::Key_B, ZoomFitBest );
    CONNECT_ACTION( zoomFitBest, photoView, setZoomMode() );

    //
    // Navigation
    //

    loadNext = createAction( tr( "Load &Next Image" ), Qt::Key_Space, NavigateNext );
    addShortcut( loadNext, Qt::Key_Right );
    CONNECT_ACTION( loadNext, photoView->m_pMainWidget, slot_next() );

    loadPrevious = createAction( tr( "Load &Previous Image" ), Qt::Key_Backspace, NavigatePrevious );
    addShortcut( loadPrevious, Qt::Key_Left );
    CONNECT_ACTION( loadPrevious, photoView->m_pMainWidget, slot_prev() );

    loadFirst = createAction( tr( "Load &First Image" ), Qt::Key_Home, NavigateFirst );
    CONNECT_ACTION( loadFirst, photoView, navigate() );

    loadLast = createAction( tr( "Load &Last Image" ), Qt::Key_End, NavigateLast );
    CONNECT_ACTION( loadLast, photoView, navigate() );


    //
    // Misc
    //

    forceReload = createAction( tr( "Force &Reload" ), Qt::Key_F5 );
    CONNECT_ACTION( forceReload, photoView, forceReload() );

    toggleFullscreen = createAction( tr( "Toggle F&ullscreen" ), Qt::Key_Return );
    CONNECT_ACTION( toggleFullscreen, photoView, toggleFullscreen() );

    //quit = createAction( tr ( "&Quit" ), Qt::Key_Q );
    //addShortcut( quit, Qt::Key_Escape );
    //CONNECT_ACTION( quit, qApp, quit() );
}


QAction * PhotoView::Actions::createAction( const QString & text,
                                            QKeySequence    shortcut,
                                            QVariant        data )
{
    QAction * action = new QAction( text, _photoView );
    action->setData( data );

#if (QT_VERSION >= QT_VERSION_CHECK( 5, 10, 0 ))
    action->setShortcutVisibleInContextMenu( true );
#endif
    _photoView->addAction( action );

    if ( ! shortcut.isEmpty() )
        action->setShortcut( shortcut );

    return action;
}


void PhotoView::Actions::addShortcut( QAction * action, QKeySequence newShortcut ) const
{
    QList<QKeySequence> shortcuts = action->shortcuts();
    shortcuts << newShortcut;
    action->setShortcuts( shortcuts );
}


void PhotoView::rotateImage(int rotation)
{
    currentRotation += rotation;
    //qDebug()<<"rotateImage"<<rotation<<currentRotation;
    // normalize between 360 and 0
    //currentRotation = (currentRotation % 360 + 360) % 360;
    QTransform transform;
    QImage transformedImage;

    transform.translate(_canvas->pixmap().width() / 2, _canvas->pixmap().height() / 2); // 将原点移动到中心
    transform.rotate(currentRotation); // 绕中心点旋转
    transform.translate(-_canvas->pixmap().width() / 2, -_canvas->pixmap().height() / 2); // 将原点移回原位


    _canvas->setTransform(transform);
}

void PhotoView::showExifPanel(bool bShow)
{
    if (_exifPanel == nullptr)
        return;

    if (bShow)
    {
        _exifPanel->show();
    }
    else
    {
        _exifPanel->hide();
        _rightBorder->hide();
    }
}
