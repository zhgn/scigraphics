
#pragma once

#include "graphics/datamap.h"
#include "graphics/numbers.h"
#include "graphics/painter.h"

namespace scigraphics 
{

  class scale;

  // ============================================================

  class graphViewMap 
  {
    private:
      bool Visible;

    public:
      graphViewMap() : Visible(true) {}
      virtual ~graphViewMap() {}

      void setVisible( bool V ) { Visible = V; }
      bool isVisible() const { return Visible; }

      virtual void draw( painter &Painter, const pairScales& Scales, const dataMap &Data ) const = 0;
      virtual void drawLegendExample( painter &Painter, const wrectangle &Rectangle, const numberLimits &DataLimits ) const = 0;
      
      virtual wcoord legendExampleWidth() const  { return 80; }
      virtual wcoord legendExampleHeight() const { return 200; }
  };

  // ============================================================

  class graphViewRectangleMap : public graphViewMap
  { 
    public:
      class pointColorStrategy
      {
        protected:
          static double relativePointValue( const dataMap::point &Point, interval<number> IntervalZ );
          static double relativeValueToUnitInterval( double Value );

        public:
          virtual color relativeValueColor( double RelativeValue ) const = 0;
          virtual color invalidValueColor() const;
          virtual color pointColor( const dataMap::point &Point, interval<number> IntervalZ ) const;
          virtual ~pointColorStrategy() {}
      };

      class grayscalePointColorStrategy : public pointColorStrategy
      {
        public:
          color relativeValueColor( double RelativeValue ) const;
      };

      class redYellowBluePointColorStrategy : public pointColorStrategy
      {
        public:
          color relativeValueColor( double RelativeValue ) const;
      };

      class yellowRedBluePointColorStrategy : public pointColorStrategy
      {
        public:
          color relativeValueColor( double RelativeValue ) const;
      };


    private:
      pointColorStrategy *PointColorStrategy;
      scale *ScaleZ;

    protected:
      virtual void drawPoint( painter &Painter, const pairScales& Scales, const dataMap::point &Point, interval<number> IntervalZ ) const;

      virtual void drawRainbowRectangleBorder( painter &Painter, const wrectangle &Rectangle ) const;
      virtual void drawRainbowRectangle( painter &Painter, const wrectangle &Rectangle ) const;
      virtual void drawRainbowMarkers( painter &Painter, const wrectangle &Rectangle, const numberLimits &DataLimits ) const;
      wcoord rainbowRectangleWidth() const { return legendExampleWidth()/3; }

    public:
      graphViewRectangleMap();
      ~graphViewRectangleMap();

      void setColorStrategy( pointColorStrategy *Strategy );

      void draw( painter &Painter, const pairScales& Scales, const dataMap &Data ) const;
      void drawLegendExample( painter &Painter, const wrectangle &Rectangle, const numberLimits &DataLimits ) const;
  };

// ============================================================

}
