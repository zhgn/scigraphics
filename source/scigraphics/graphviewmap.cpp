
// ============================================================

#include "graphics/graphviewmap.h"
#include "graphics/painter.h"
#include "graphics/scale.h"
#include "graphics/numberstyle.h"

#include <iostream>
#include <cmath>
#include <cassert>
#include <stdexcept>

#if _WIN32
#  pragma warning( disable : 4244 )
#endif

// ============================================================

double scigraphics::graphViewRectangleMap::pointColorStrategy::relativePointValue( const dataMap::point &Point, interval<number> IntervalZ )
{
  if ( ! Point.isValid() )
    return invalidNumber();

  double RelValue = IntervalZ.partOfDistance(Point.z());
  RelValue = relativeValueToUnitInterval( RelValue );
  return RelValue;
}

// ------------------------------------------------------------

double scigraphics::graphViewRectangleMap::pointColorStrategy::relativeValueToUnitInterval( double Value )
{
  if ( ! isValidNumber(Value) )
    return 0;
  Value = std::max<double>( Value, 0 );
  Value = std::min<double>( Value, 1 );
  return Value;
}

// ------------------------------------------------------------

scigraphics::color scigraphics::graphViewRectangleMap::pointColorStrategy::invalidValueColor() const
{
  return color( 0xFF, 0xFF, 0xFF, 0xFF );
}

// ------------------------------------------------------------

scigraphics::color scigraphics::graphViewRectangleMap::pointColorStrategy::pointColor( const dataMap::point &Point, interval<number> IntervalZ ) const
{
  if ( ! Point.isValid() )
    return invalidValueColor();

  double RelativeValue = relativePointValue(Point,IntervalZ);
  return relativeValueColor( RelativeValue );
}

// ------------------------------------------------------------

scigraphics::color scigraphics::graphViewRectangleMap::grayscalePointColorStrategy::relativeValueColor( double Value ) const
{
  if ( Value < 0 || 1 < Value )
    return invalidValueColor();
  
  int ColorValue = Value * 0xFF;
  return color( ColorValue, ColorValue, ColorValue );
}

// ------------------------------------------------------------

scigraphics::color scigraphics::graphViewRectangleMap::redYellowBluePointColorStrategy::relativeValueColor( double Value ) const
{
  if ( Value < 0 || 1 < Value )
    return invalidValueColor();

  int HsvHue = (1-Value)*240; 
  int HsvSaturation = 100;
  int HsvValue = 80;

  return color::fromHSV( HsvHue, HsvSaturation, HsvValue );
}

// ------------------------------------------------------------

scigraphics::color scigraphics::graphViewRectangleMap::yellowRedBluePointColorStrategy::relativeValueColor( double Value ) const
{
  
  if ( Value < 0 || 1 < Value )
    return invalidValueColor();

  int HsvHue = 360 + 60 - (1-Value)*180; 
  int HsvSaturation = 100;
  int HsvValue = 80;

  return color::fromHSV( HsvHue % 360, HsvSaturation, HsvValue );

}

// ------------------------------------------------------------
      
scigraphics::graphViewRectangleMap::graphViewRectangleMap() : 
  PointColorStrategy(NULL),
  ScaleZ(NULL)
{
  PointColorStrategy = new redYellowBluePointColorStrategy();
  ScaleZ = new scaleLinear();
}

// ------------------------------------------------------------

scigraphics::graphViewRectangleMap::~graphViewRectangleMap()
{
  delete PointColorStrategy;
  delete ScaleZ;
}

// ------------------------------------------------------------

void scigraphics::graphViewRectangleMap::setColorStrategy( pointColorStrategy *Strategy )
{
  if ( Strategy == NULL )
    throw std::invalid_argument( "Strategy argument must be not NULL" );

  delete PointColorStrategy;
  PointColorStrategy = Strategy;
}

// ------------------------------------------------------------

void scigraphics::graphViewRectangleMap::draw( painter &Painter, const pairScales& Scales, const dataMap &Data ) const
{
  assert( PointColorStrategy != NULL );

  dataMap::iterator Begin = Data.begin();
  dataMap::iterator End   = Data.end();

  interval<number> IntervalZ = Data.limitsZ().totalLimits();

  for ( dataMap::iterator Point = Begin; Point != End; ++Point )
    drawPoint( Painter, Scales, *Point, IntervalZ );
}

// ------------------------------------------------------------
      
void scigraphics::graphViewRectangleMap::drawPoint( painter &Painter, const pairScales& Scales, const dataMap::point &Point, interval<number> IntervalZ ) const
{
  if ( ! Point.isValid() )
    return;

  color Color = PointColorStrategy->pointColor( Point, IntervalZ );

  fpoint Point0 = Scales.npoint2fpoint( npoint( Point.x0(), Point.y0() ) );
  fpoint Point1 = Scales.npoint2fpoint( npoint( Point.x1(), Point.y1() ) );

  Painter.drawRectangleF( frectangle(Point0,Point1), brushStyle(Color) );
}

// ------------------------------------------------------------

void scigraphics::graphViewRectangleMap::drawLegendExample( painter &Painter, const wrectangle &Rectangle, const numberLimits &DataLimits ) const
{
  wpoint LeftUp = Rectangle.leftUp();
  wpoint RightDown = Rectangle.rightDown();

  LeftUp.moveXY( 5, -5 );
  RightDown.moveXY( -5, 5 );

  wrectangle EffRectangle = wrectangle( LeftUp, RightDown );

  drawRainbowRectangle( Painter, EffRectangle );
  drawRainbowRectangleBorder( Painter, EffRectangle );
  drawRainbowMarkers( Painter, EffRectangle, DataLimits );
}

// ------------------------------------------------------------
      
void scigraphics::graphViewRectangleMap::drawRainbowRectangleBorder( painter &Painter, const wrectangle &Rectangle ) const
{
  lineStyle BorderLineStyle( color::Black );
  brushStyle BrushStyle( color::Yellow, brushStyle::None );
  wpoint LeftUp = Rectangle.leftUp();
  wpoint RightDown = wpoint( Rectangle.left() + rainbowRectangleWidth(), Rectangle.down() );
  Painter.drawRectangleW( wrectangle( LeftUp, RightDown ), BrushStyle, BorderLineStyle );
}

// ------------------------------------------------------------

void scigraphics::graphViewRectangleMap::drawRainbowRectangle( painter &Painter, const wrectangle &Rectangle ) const
{
  assert( PointColorStrategy != NULL );
  assert( Rectangle.up() >= Rectangle.down() );
  
  const unsigned NumOfRainbowLines = std::min<unsigned>( 200, Rectangle.height() );
  lineStyle NoneLineStyle( lineStyle::None );

  const wcoord RainbowRectangleWidth = rainbowRectangleWidth();

  for ( unsigned i = 0; i < NumOfRainbowLines; i++ )
  {
    double Part0 = (double)i/(NumOfRainbowLines);
    double Part1 = (double)(i+1)/(NumOfRainbowLines);
    
    wcoord Y0 = Rectangle.up() - Rectangle.height() * Part0;
    wcoord Y1 = Rectangle.up() - Rectangle.height() * Part1;
    wcoord X0 = Rectangle.left();
    wcoord X1 = Rectangle.left() + RainbowRectangleWidth;

    color Color = PointColorStrategy->relativeValueColor( Part0 );
    brushStyle BrushStyle( Color );
    NoneLineStyle.setColor( Color );
    Painter.drawRectangleW( wrectangle( wpoint(X0,Y0), wpoint(X1,Y1) ), BrushStyle, NoneLineStyle );
  }
}

// ------------------------------------------------------------
      
void scigraphics::graphViewRectangleMap::drawRainbowMarkers( painter &Painter, const wrectangle &Rectangle, const numberLimits &DataLimits ) const
{
  assert( ScaleZ != NULL );
 
  ScaleZ->setNumberLimits( DataLimits );
  ScaleZ->reset();
  ScaleZ->setNumberOfMarks( 7, 18 );
  
  std::vector<number> Marks = ScaleZ->marks();

  wcoord RainbowRectangleWidth = rainbowRectangleWidth();

  generalNumberStyle NumberStyle;
  lineStyle LineStyle( color::Black );
  textStyle TextStyle( 13 );

  for ( unsigned i = 0; i < Marks.size(); i++ )
  {
    double Part = ScaleZ->numberToFraction( Marks[i] );
    
    wcoord X0 = Rectangle.left();
    wcoord X1 = Rectangle.left() + RainbowRectangleWidth;
    wcoord Y  = Rectangle.up() - Rectangle.height() * Part;
    
    Painter.drawLineW( wpoint(X0,Y), wpoint(X1,Y), LineStyle );
    Painter.drawTextW( NumberStyle.numberText(Marks[i]), wpoint( X1 + 4, Y ), drawer::HLeft|drawer::VCenter, TextStyle );
  }
}

// ============================================================

