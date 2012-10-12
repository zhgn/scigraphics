
// ================================================================

#include "graphics/qt4/qt4plot.h"

#include <QtGui>
#include <QDebug>

// ================================================================

//typedef graphics::mouse mouse;

// ----------------------------------------------------------------

void qt4plotView::dragEnterEvent( QDragEnterEvent *Event )
{
  if ( DragEnabled )
    Event->acceptProposedAction();
}
    
// ----------------------------------------------------------------

void qt4plotView::dragMoveEvent( QDragMoveEvent *Event )
{

  if ( DragEnabled )
    Event->acceptProposedAction();
}

// ----------------------------------------------------------------

void qt4plotView::dropEvent( QDropEvent *Event )
{
  if ( DragEnabled )
  {
    QString String;
    String = "";
    QList<QUrl> List = Event->mimeData()->urls();
    foreach ( QUrl Url, List )
      String += Url.toString() + "\n";
    Event->acceptProposedAction();
    emit dropped( String );
  }
}

// ================================================================
    
QRect  drawerQt::rectangleQt( const graphics::wrectangle& Rect ) 
{ 
  return QRect( Rect.left(), Rect.down(), Rect.width(), Rect.height() );
}

// ----------------------------------------------------------------

QPen drawerQt::penQt( const graphics::lineStyle& Style )
{
  QPen Pen;
  Pen.setColor( colorQt(Style.getColor()) );
  Qt::PenStyle PenStyle = Qt::NoPen;
  switch ( Style.getStyle() )
  {
    case graphics::lineStyle::Solid:
      PenStyle = Qt::SolidLine;
      break;
    case graphics::lineStyle::Dash:
      PenStyle = Qt::DashLine;
      break;
    default:
      PenStyle = Qt::NoPen;
      break;
  }
  Pen.setStyle( PenStyle );
  Pen.setWidth( Style.width() );
  return Pen;
}

// ----------------------------------------------------------------

QBrush drawerQt::brushQt( const graphics::brushStyle &Style )
{
  QBrush Brush;
  Brush.setColor( colorQt( Style.getColor() ) );
  Qt::BrushStyle BrushStyle = Qt::SolidPattern;
  switch ( Style.getStyle() )
  {
    case graphics::brushStyle::Solid:
      BrushStyle = Qt::SolidPattern;
      break;
    default:
      BrushStyle = Qt::NoBrush;
      break;
  }
  Brush.setStyle( BrushStyle );
  return Brush;
}

// ----------------------------------------------------------------

QFont drawerQt::fontQt( const graphics::textStyle &Style ) 
{ 
  QString FontName = Style.getFontName().empty() ? "Times" : Style.getFontName().c_str();
  return QFont( FontName, Style.getFontSize() ); 
}

// ----------------------------------------------------------------
    
QPolygon drawerQt::polygonQt( const std::vector<graphics::wpoint> &Points )
{
  QPolygon Polygon;
  for ( unsigned i = 0; i < Points.size(); i++ )
    Polygon.append( pointQt( Points[i] ) );
  return Polygon;
}

// ----------------------------------------------------------------

void drawerQt::eraseAll()
{
  if ( Painter == NULL )
    return;
  Painter->eraseRect( 0, 0, Painter->viewport().width(), Painter->viewport().height() );
}

// ----------------------------------------------------------------

void drawerQt::eraseRectangle( const graphics::wrectangle& Rectangle )
{
  if ( Painter == NULL )
    return;
  Painter->eraseRect( rectangleQt(Rectangle) ); 
}

// ----------------------------------------------------------------

void drawerQt::drawLine( const graphics::wpoint &A, const graphics::wpoint &B, const graphics::lineStyle &Style )
{
  if ( Painter == NULL )
    return;
  Painter->setPen( penQt(Style) );
  Painter->drawLine( pointQt(A), pointQt(B) );
}

// ----------------------------------------------------------------

void drawerQt::drawRectangle( const graphics::wrectangle& Rectangle, const graphics::brushStyle& BrushStyle, const graphics::lineStyle &LineStyle )
{
  if ( Painter == NULL )
    return;

  QPen Pen = penQt( LineStyle );
  QBrush Brush = brushQt( BrushStyle );

  Painter->setPen( Pen );
  Painter->setBrush( Brush );
  Painter->drawRect( rectangleQt(Rectangle) );
}

// ----------------------------------------------------------------
    
void drawerQt::drawPolygon( const std::vector<graphics::wpoint> &Points, const graphics::brushStyle& BrushStyle )
{
  if ( Painter == NULL )
    return;

  QPen Pen( Qt::NoPen );
  QBrush Brush = brushQt( BrushStyle );
  QPolygon Polygon = polygonQt( Points );

  Painter->setPen( Pen );
  Painter->setBrush( Brush );
  Painter->drawPolygon( Polygon ); 
}

// ----------------------------------------------------------------
    
void drawerQt::drawText( const std::string &Text, const graphics::wrectangle& Rectangle, const graphics::textStyle &Style, double Angle )
{
  const bool DrawDebugRectangle = false;
  if ( Painter == NULL )
    return;

  Painter->setFont( fontQt(Style) );
  Painter->setPen( penQt( graphics::lineStyle( Style.getColor() ) ) );
  Painter->setBrush( Qt::NoBrush );
  Painter->save();
  Painter->rotate( Angle );
  QPoint Center = Painter->transform().inverted().map( QPoint(Rectangle.center().x(),Rectangle.center().y()) );
  QRect TextRectangle = QRect( QPoint( Center.x()-Rectangle.width()/2,Center.y()-Rectangle.height()/2) , QSize( Rectangle.width(), Rectangle.height() ) );
  Painter->drawText( TextRectangle, Qt::AlignHCenter|Qt::AlignVCenter, QString::fromUtf8(Text.c_str()) );
  if ( DrawDebugRectangle )
    Painter->drawRect( TextRectangle );
  Painter->restore();
}

// ----------------------------------------------------------------

graphics::wcoord drawerQt::textWidth( const std::string &Text, const graphics::textStyle &Style )
{
  QFontMetrics Metrics( fontQt(Style) );
  return Metrics.size( 0, Text.c_str() ).width();
}

// ----------------------------------------------------------------

graphics::wcoord drawerQt::textHeight( const std::string &Text, const graphics::textStyle &Style )
{
  QFontMetrics Metrics( fontQt(Style) );
  return Metrics.size( 0, Text.c_str() ).height();
}

// ----------------------------------------------------------------

void drawerQt::flush()
{
  PixmapItem->setPixmap( *PlotPixmap ); 
  View->update();
}

// ----------------------------------------------------------------

drawerQt::drawerQt( QWidget *Prnt ) : Parent(Prnt)
{
  QSize ScreenSize = QApplication::desktop()->screenGeometry().size();

  PlotPixmap = new QPixmap( ScreenSize );
  Painter = new QPainter( PlotPixmap );

  Scene = new QGraphicsScene();
  PixmapItem = new QGraphicsPixmapItem( 0, Scene );
  PixmapItem->setOffset( 0, 0 );
  
  View  = new qt4plotView( Parent );
  View->setScene( Scene );
  View->move( Parent->x(), Parent->y() );
  View->resize( width(), height() );
  View->show();
}

// ================================================================

qt4plot::qt4plot( QWidget* Parent, Qt::WindowFlags Flags ) : QWidget(Parent,Flags), plot()
{
  setMouseCallBack( new qt4plotMouseCallBack(*this) );

  drawerQt *Drawer = new drawerQt(this); 
  setDrawer( Drawer );

  qt4plotView *View = Drawer->view();

  connect( View, SIGNAL(resized()), SLOT(resizePlot()) );
  connect( View, SIGNAL(dropped(QString)), SIGNAL(dropped(QString)) );
 
  connect( View, SIGNAL(mousePressed(QMouseEvent*)),  SLOT(mousePressed(QMouseEvent*)) );
  connect( View, SIGNAL(mouseMoved(QMouseEvent*)),    SLOT(mouseMoved(QMouseEvent*)) );
  connect( View, SIGNAL(mouseReleased(QMouseEvent*)), SLOT(mouseReleased(QMouseEvent*)) );
  connect( View, SIGNAL(mouseDoubleClick(QMouseEvent*)), SLOT(mouseDoubleClicked(QMouseEvent*)) );
  connect( View, SIGNAL(mouseWheel(QWheelEvent*)),    SLOT(mouseWheel(QWheelEvent*)) );
  //connect( View, SIGNAL(keyReleased(QKeyEvent*)),     SLOT(keyReleased(QKeyEvent*)) );

  resize( 700, 800 );

  printTestCornerRectangles();
}

// ----------------------------------------------------------------

unsigned qt4plot::plotMouseModifiers( Qt::KeyboardModifiers Modifiers )
{
  unsigned Result = 0;

  if ( Modifiers & Qt::ShiftModifier )    Result |= graphics::mouse::Shift;
  if ( Modifiers & Qt::ControlModifier )  Result |= graphics::mouse::Ctrl;
  if ( Modifiers & Qt::AltModifier )      Result |= graphics::mouse::Alt;

  return Result;
}

// ----------------------------------------------------------------

unsigned qt4plot::plotMouseButtons( const QMouseEvent *Event ) 
{
  Q_ASSERT( Event != NULL );
  
  unsigned Result = 0;

  if ( Event->button() & Qt::LeftButton )    Result |= graphics::mouse::Left;
  if ( Event->button() & Qt::RightButton )   Result |= graphics::mouse::Right;
  if ( Event->button() & Qt::MidButton )     Result |= graphics::mouse::Middle;

  Result |= plotMouseModifiers( Event->modifiers() );

  return Result;
}

// ----------------------------------------------------------------

graphics::wpoint qt4plot::plotMousePoisition( const QMouseEvent *Event )
{
  Q_ASSERT( Event != NULL );
  return graphics::wpoint( Event->x(), Event->y() );
}

// ----------------------------------------------------------------

graphics::wpoint qt4plot::plotMousePoisition( const QWheelEvent *Event )
{
  Q_ASSERT( Event != NULL );
  return graphics::wpoint( Event->x(), Event->y() );
}

// ----------------------------------------------------------------

void qt4plot::printTestCornerRectangles()
{
  using namespace graphics;

  int W = getDrawerQt()->width();
  int H = getDrawerQt()->height();

  getDrawerQt()->eraseAll();
  getDrawerQt()->drawRectangle( wrectangle( wpoint(0,0), wpoint(10,10) ),     brushStyle(color::Black), lineStyle(1,color::Red) );
  getDrawerQt()->drawRectangle( wrectangle( wpoint(W-10,H-10), wpoint(W,H) ), brushStyle(color::White), lineStyle(1,color::Green) );
  getDrawerQt()->flush();
}

// ----------------------------------------------------------------

void qt4plot::resizePlot()
{
  getDrawerQt()->view()->resize( width(), height() );
  getDrawerQt()->scene()->setSceneRect( 0, 0, width()-sceneShift(), height()-sceneShift() );
  replot();
}

// ----------------------------------------------------------------

void qt4plot::mousePressed( QMouseEvent *Event )   
{ 
  graphics::wpoint Point = plotMousePoisition( Event );
  unsigned Buttons = plotMouseButtons(Event);
  mouseHandler().mousePressed( Point, Buttons ); 
  emitPositionObtained(Event); 
}

// ----------------------------------------------------------------

void qt4plot::mouseMoved( QMouseEvent *Event )     
{ 
  graphics::wpoint Point = plotMousePoisition( Event );
  mouseHandler().mouseMoved( Point ); 
}

// ----------------------------------------------------------------

void qt4plot::mouseReleased( QMouseEvent *Event )  
{ 
  graphics::wpoint Point = plotMousePoisition( Event );
  mouseHandler().mouseReleased( Point ); 
}

// ----------------------------------------------------------------

void qt4plot::mouseDoubleClicked( QMouseEvent *Event )  
{
  graphics::wpoint Point = plotMousePoisition( Event );
  mouseHandler().mouseDoubleClicked( Point ); 
}

// ----------------------------------------------------------------

void qt4plot::mouseWheel( QWheelEvent *Event )     
{ 
  int Delta = Event->delta();
  unsigned Buttons = plotMouseButtons(Event);
  graphics::wpoint Point = plotMousePoisition( Event );
  mouseHandler().mouseWheel( Point, Delta, Buttons ); 
}

// ----------------------------------------------------------------

QPixmap qt4plot::createTranspPixmap( const int Width, const int Height )
{
  QPixmap Pixmap( Width, Height );
  Pixmap.fill(Qt::transparent);
  QBitmap Mask( Pixmap.size() );
  Mask.fill( Qt::color1 );
  Pixmap.setMask( Mask );
  return Pixmap;
}

// ----------------------------------------------------------------

void qt4plot::setCrossCursor( bool Set )
{
  if ( Set )
  {
    const int MaxCursorLength = 61;
    const QColor CursorColor  = Qt::black;
    QPixmap XYCursor = createTranspPixmap( MaxCursorLength, MaxCursorLength );
    QPainter Painter( &XYCursor );
    Painter.fillRect( 0, XYCursor.height()/2, XYCursor.width(), 1, CursorColor );
    Painter.fillRect( XYCursor.width()/2, 0,  1, XYCursor.height(), CursorColor );
    setCursor( QCursor(XYCursor) );
  } else {
    unsetCursor();
  }
}

// ----------------------------------------------------------------

void qt4plot::emitPositionObtained( const QMouseEvent *Event )
{
  assert( Event != NULL );
  if ( plotMouseButtons(Event) == graphics::mouse::Left )
  {
    graphics::wpoint MousePos = plotMousePoisition(Event);
    graphics::npoint Point = getBottomLeftPairScales().wpoint2npoint( MousePos, getPainter() );
    emit positionObtained( Point.x(), Point.y() );
  }
}

// ----------------------------------------------------------------

void qt4plot::emitPlotChangedByMouse()
{
  emit plotChangedByMouse();
}

// ================================================================

qt4plotMouseCallBack::qt4plotMouseCallBack( qt4plot &Plot ) : graphics::mouseCallBack(Plot) 
{
}

// ----------------------------------------------------------------

qt4plot& qt4plotMouseCallBack::getQt4Plot() 
{
  return static_cast<qt4plot&>( getPlot() );
}

// ----------------------------------------------------------------
    
void qt4plotMouseCallBack::onPressed( graphics::mouse::mouseActionHandler* )
{
  getQt4Plot().emitPlotChangedByMouse();
}

// ----------------------------------------------------------------

void qt4plotMouseCallBack::onMoved( graphics::mouse::mouseActionHandler *Handler )
{
  if ( isSelectionMouseAction(Handler) )
    getQt4Plot().emitSelectionChanged();
  getQt4Plot().emitPlotChangedByMouse();
}

// ----------------------------------------------------------------

void qt4plotMouseCallBack::onRelease( graphics::mouse::mouseActionHandler *Handler )
{
  if ( isSelectionMouseAction(Handler) )
  {
    getQt4Plot().emitSelectionChanged();
    getQt4Plot().emitSelectionChangingFinished();
  } 
  getQt4Plot().emitPlotChangedByMouse();
}

// ----------------------------------------------------------------
    
void qt4plotMouseCallBack::onWheel( graphics::mouse::mouseWheelHandler* )
{
  getQt4Plot().emitPlotChangedByMouse();
}

// ----------------------------------------------------------------

bool qt4plotMouseCallBack::isSelectionMouseAction( graphics::mouse::mouseActionHandler *Handler )
{
  return ( 
            dynamic_cast< graphics::mouse::selectAction* >(Handler)         != NULL   ||
            dynamic_cast< graphics::mouse::moveSelectionAction* >(Handler)  != NULL   ||
            dynamic_cast< graphics::mouse::resetSelectionAction* >(Handler) != NULL   
         );
}

// ================================================================
