
#pragma once

#include "graphics/numbers.h"
#include "graphics/linestyle.h"
#include "graphics/painter.h"
#include "graphics/scale.h"

namespace graphics
{

// ============================================================

  class grid
  {
    public:
      static const lineStyle DefaultGridLineStyle;

    private:
      lineStyle LineStyle;

    protected:
      virtual void drawMajorGrids( painter &Painter, const scale &Scale, const std::vector<number> &Ticks );
      virtual void drawMajorGridOneLine( painter &Painter, const scale &Scale, const number Value ) = 0;

    public:
      grid() : LineStyle(DefaultGridLineStyle) {}

      const lineStyle& getLineStyle() const { return LineStyle; }

      virtual void draw( painter &Painter, const scale &Scale, const std::vector<number> &Ticks );

      virtual ~grid() {}
  };

  class gridX : public grid 
  {
    protected:
      void drawMajorGridOneLine( painter &Painter, const scale &Scale, const number Value );
  };
  
  class gridY : public grid 
  {
    protected:
      void drawMajorGridOneLine( painter &Painter, const scale &Scale, const number Value );
  };

// ============================================================

}
