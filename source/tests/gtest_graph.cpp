
// =========================================================

#include <algorithm>
#include <cstring>
#include <cmath>
#include <typeinfo>

#include <gtest/gtest.h>

#include "scigraphics/graph.h"
#include "scigraphics/graphsequence.h"
#include "scigraphics/graphviewcollection.h"

namespace scigraphics {};
using namespace scigraphics;

// =========================================================

TEST( test_graphSequenceVector, getView )
{
  graphSequenceVector Graph;

  ASSERT_TRUE( Graph.getViews().getView<graphViewLine>() != NULL );
  ASSERT_EQ( std::string() + typeid(graphViewLine).name(), std::string() + typeid(*Graph.getViews().getView<graphViewLine>()).name() );
}

// ---------------------------------------------------------

TEST( test_graphSequenceVector, setViewVisible )
{
  graphSequenceVector Graph;

  ASSERT_TRUE( Graph.getViews().getView<graphViewLine>()->isVisible() );
  
  Graph.setVisibleLines( false );
  ASSERT_FALSE( Graph.getViews().getView<graphViewLine>()->isVisible() );
}

// ---------------------------------------------------------

TEST( test_graphSequenceVector, isViewExist )
{
  graphSequenceVector Graph;

  ASSERT_TRUE( Graph.getViews().isViewExist<graphViewLine>() );
}

// ---------------------------------------------------------

TEST( test_graphSequenceVector, limitsX )
{
  graphSequenceVector Graph;
  Graph.append( 0, 0 );
  Graph.append( 2, 2 );
  Graph.append( -1, -1 );


  ASSERT_NEAR( -1, Graph.limitsX().totalLimits().min(), 1e-5 );
  ASSERT_NEAR( -1, Graph.limitsX().negativeLimits().max(), 1e-5 );
  ASSERT_NEAR( 2, Graph.limitsX().positiveLimits().min(), 1e-5 );
  ASSERT_NEAR( 2, Graph.limitsX().totalLimits().max(), 1e-5 );
}

// ---------------------------------------------------------

TEST( test_graphSequenceVector, limitsY )
{
  graphSequenceVector Graph;
  Graph.append( 0, 0 );
  Graph.append( 2, 2 );
  Graph.append( -1, -1 );
  
  ASSERT_NEAR( -1, Graph.limitsY( interval<number>(-2,3) ).totalLimits().min(), 1e-5 );
  ASSERT_NEAR( 2,  Graph.limitsY( interval<number>(-2,3) ).totalLimits().max(), 1e-5 );
}

// ---------------------------------------------------------

TEST( test_graphSequenceVector, legend )
{
  const std::string Legend = "Legend";
  graphSequenceVector Graph( Legend, color::Black );

  ASSERT_EQ( Legend, Graph.legend() );
  ASSERT_TRUE( Graph.showLegend() ); 
  
  Graph.setLegend( Legend + "x" );
  ASSERT_EQ( Legend + "x", Graph.legend() );

  Graph.setShowLegend(false);
  ASSERT_FALSE( Graph.showLegend() );
}

// =========================================================

