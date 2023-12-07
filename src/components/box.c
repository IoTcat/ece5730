#ifndef __BOX_C__
#define __BOX_C__


#include "../config.h"


// Wall detection
#define hitBottom(b) (b>int2fix15(BOX_BOTTOM))
#define hitTop(b) (b<int2fix15(BOX_TOP))
#define hitLeft(a) (a<int2fix15(BOX_LEFT))
#define hitRight(a) (a>int2fix15(BOX_RIGHT))


// Draw the Boundary
void drawBoundary() {

  drawVLine(BOX_LEFT, BOX_TOP, BOX_BOTTOM - BOX_TOP, BOX_COLOR) ;
  drawVLine(BOX_RIGHT, BOX_TOP, BOX_BOTTOM - BOX_TOP, BOX_COLOR) ;
  drawHLine(BOX_LEFT, BOX_BOTTOM, BOX_RIGHT - BOX_LEFT, BOX_COLOR) ;

  // NOTE: SLOW CODE
  // dash line for the top
  for(int i = BOX_LEFT; i < BOX_RIGHT; i += 10){
    drawPixel(i, BOX_TOP, BOX_COLOR);
  }
}



#endif