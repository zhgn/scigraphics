
// ============================================================

#if _WIN32
#  define _CRT_SECURE_NO_WARNINGS 1
#endif

#include "graphics/cursorpoint.h"
#include <sstream>
#include <cstdio>

// ============================================================

const scigraphics::wrectangle scigraphics::cursorPositionViewer::InitCursorRectangle( wpoint(30,30), wpoint(30,30) );

// ============================================================
      
scigraphics::cursorPositionViewer::cursorPositionViewer() : 
  floatRectangle(InitCursorRectangle)
{
}

// ------------------------------------------------------------

void scigraphics::cursorPositionViewer::draw( painter &Painter, const pairScales &Scales, const wpoint &Point )
{
  if ( ! isVisible() )
    return;

  std::string Text = pointText( Painter, Scales, Point );

  updateRectangle( Painter, Text );
  drawBackground( Painter );
  drawText( Painter, Text );
}

// ------------------------------------------------------------

void scigraphics::cursorPositionViewer::updateRectangle( painter &Painter, const std::string &Text )
{
  unsigned TextWidth  = Painter.textWidth( Text, getTextStyle() );
  unsigned TextHeight = Painter.textHeight( Text, getTextStyle() );
  
  wpoint LegendRightDownCorner = wpoint( std::max<wcoord>( getRectangle().left() + TextWidth + 2*textHorizontalIndent(), getRectangle().right() ), 
                                         std::min<wcoord>( getRectangle().up() - TextHeight - 2*textVerticalIndent(),  getRectangle().down()  ) );
  setRectangle( getRectangle().leftUp(), LegendRightDownCorner );
}

// ------------------------------------------------------------

void scigraphics::cursorPositionViewer::drawText( painter &Painter, const std::string &Text )
{
  wpoint Point = getRectangle().leftUp();
  Point.moveY( -(int)textVerticalIndent() );
  Point.moveX( +(int)textHorizontalIndent() );
  Painter.drawTextW( Text, Point, drawer::HLeft|drawer::VDown, getTextStyle() );
}

// ------------------------------------------------------------
      
std::string scigraphics::cursorPositionViewer::pointText( painter &Painter, const pairScales &Scales, const wpoint &Point ) const
{
  npoint NPoint = Scales.wpoint2npoint( Point, Painter );

  char Buffer[ 256 ];
  Buffer[ 0 ] = Buffer[ sizeof(Buffer)-1 ] = '\0';
  snprintf( Buffer, sizeof(Buffer)-1, "( %g : %g )", NPoint.x(), NPoint.y() );

  return Buffer;
}

// ============================================================



