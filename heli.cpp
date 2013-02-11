/*
 * Helicopter Code - Keerthana
 */

#include <iostream>
#include <list>
#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <sys/time.h>
/*
 * Header files for X functions
 */
#include <X11/Xlib.h>
#include <X11/Xutil.h>

using namespace std;

const int Border = 5;
const int BufferSize = 10;
const int FPS = 60;
const int numColumns = 100;
int speed = 10;
int grader = 0;
const int bomb_size = 15;
int points = 0;
XSizeHints hints;

class Bomb;
list<Bomb*> bombs;
class Missile;
list<Missile*> missiles;
/*
 * Information to draw on the window.
 */
struct XInfo {
  Display        *display;
  int            screen;
  Window         window;
  GC gc[3];

  Pixmap pixmap;
  int   width;    // size of pixmap
  int   height;

  XFontStruct * font;
};

struct block {
  int width;
  int height;
} block;

/*
 * Function declarations
 */
void drawGameOverScreen(XInfo &xInfo);
void drawSplashScreen(XInfo &xInfo);
unsigned long now();

/*
 * An abstract class representing displayable things.
 */
class Displayable {
public:
  int target;
  virtual void paint(XInfo &xInfo) = 0;
  virtual void move(XInfo &xInfo) = 0;
  virtual void move(XInfo &xInfo, int key) = 0;
  virtual int getX() = 0;
  virtual int getY() = 0;
  virtual void resetX(XInfo &xInfo) = 0;
};


class Column : public Displayable {
public:

  int x, y, numBlocks;
  virtual void paint(XInfo &xInfo) {
    int i;
    y = 0;
    for(i=0; i<numBlocks; i++){
      XDrawRectangle(xInfo.display, xInfo.pixmap, xInfo.gc[0], x, xInfo.height-block.height*(i+1), block.width, block.height);
      y = y + block.height;
    }

    if (target) {
      XFillRectangle(xInfo.display, xInfo.pixmap, xInfo.gc[0], x+block.width/3, xInfo.height-block.height*(numBlocks)-block.height/1.5, block.width/3, block.height/1.5);
      y = y + block.height / 1.5;
    }
  }

 // constructor
  Column(int x, int y, int t):x(x), y(y)  {
    numBlocks = rand()%10 + 1;
    //cout << "x,y:" << x << ", " << y ;
    numBlocks = y + 1;
    //cout << ", t:" << t << endl;
    target = t;
  }

  void move(XInfo &xInfo) {
    x = x - (speed/10);
   }

  void move(XInfo &xInfo, int key){
  }

  int getX(){
    return x;
  }

  int getY(){
    return y;
  }

  void resetX(XInfo &xInfo){
    x = x + numColumns*block.width;
  }

  void setTargetDead() {
    target = 0; // false
  }

};




  Bomb(int xbomb, int ybomb) : xbomb(xbomb), ybomb(ybomb){
    wbomb = bomb_size;
    hbomb = bomb_size / 2;
    bombLost = 0;
  }

  int crashed(XInfo &xInfo, int x, int y, int xbomb, int ybomb,
               int xbody, int ybody, int target) {

    int targetHit = 0;
    if (xbomb > x - bomb_size && xbomb < x + bomb_size) {
     if (ybomb + block.height/3 > xInfo.height - y && ybomb < xInfo.height) {
         if (target == 1) {
            targetHit = 1;
         } else {
            targetHit = 2;
         }
         bombLost = 1;
     }
   }
   if (xbomb > xInfo.width || ybomb > xInfo.height) {
     xbomb = xbody;
     ybomb = ybody;
   }
   return targetHit;
  }
};



class Missile : public Displayable{

  int xmissile, ymissile, wmissile, hmissile;

public:
  int missileLost;
  virtual void paint(XInfo& xInfo){
    XFillArc(xInfo.display, xInfo.pixmap, xInfo.gc[0], xmissile - (wmissile/2), ymissile - (hmissile/2), wmissile, hmissile, 0, 360*64);
  }

  virtual void move(XInfo& xInfo){
    xmissile += block.height;
    ymissile += 5;
  }

  virtual void move(XInfo &xInfo, int key){

  }
  virtual int getX(){
    return xmissile;
  }
  virtual int getY(){
    return ymissile;
  }
  virtual void resetX(XInfo &xInfo){
  }

  Missile(int xmissile, int ymissile) : xmissile(xmissile), ymissile(ymissile){
    wmissile = bomb_size * 2;
    hmissile = bomb_size / 2;
    missileLost = 0;
  }

  int crashed(XInfo &xInfo, int x, int y, int xmissile, int ymissile,
               int xbody, int ybody, int target) {

    int targetHit = 0;
    if (xmissile > x - bomb_size && xmissile < x + bomb_size) {
     if (ymissile + block.height/3 > xInfo.height - y && ymissile < xInfo.height) {
         if (target == 1) {
            targetHit = 1;
         } else {
            targetHit = 2;
         }
         missileLost = 1;
     }
   }
   if (xmissile > xInfo.width || ymissile > xInfo.height) {
     xmissile = xbody;
     ymissile = ybody;
   }
   return targetHit;
  }
};




class Helicopter : public Displayable {

public:
  int xbody, ybody, xprop, yprop, xtail, ytail;
  int xbomb, ybomb;
  int xmissile, ymissile;

  virtual void paint(XInfo &xInfo) {
    xprop = xbody + 10;
    yprop = ybody - 15;
    xtail = xbody - 40;
    ytail = ybody;

   // Draw the helicopter
    XDrawArc(xInfo.display, xInfo.pixmap, xInfo.gc[0],
      xbody-25, ybody-8, 50, 16, 0, 360*64);
    XFillArc(xInfo.display, xInfo.pixmap, xInfo.gc[0],
      xprop-15, yprop- 1.5, 15, 3, 0, 360*64);
    XFillArc(xInfo.display, xInfo.pixmap, xInfo.gc[0],
      xprop, yprop - 1.5, 15, 3, 0, 360*64);
    XFillArc(xInfo.display, xInfo.pixmap, xInfo.gc[0],
      xtail - 22, ytail - 9, 13, 3, 0, 360*64);
    XFillArc(xInfo.display, xInfo.pixmap, xInfo.gc[0],
      xtail - 18, ytail - 13, 3, 13, 0, 360*64);
    XDrawLine(xInfo.display, xInfo.pixmap, xInfo.gc[0],
      xtail, ytail, xbody - 25, ybody);
    XDrawLine(xInfo.display, xInfo.pixmap, xInfo.gc[0],
      xtail, ytail, xtail - 12, ytail - 4);
    XDrawLine(xInfo.display, xInfo.pixmap, xInfo.gc[0],
      xbody+5, ybody - 8, xprop, yprop);
  }


  // constructor
  Helicopter(XInfo &xInfo) {
    xbomb = xbody = xInfo.width/7;
    ybomb = ybody = xInfo.height/2;
    xmissile = xbody = xInfo.width/7;
    ymissile = ybody = xInfo.height/2;
  }

  void move(XInfo &xInfo){

  }

  void move(XInfo &xInfo, int key) {
    switch(key) {
      case 'w':
      case 'W':
        if(ybody >= 35)
          ybody = ybody - block.height;
        break;
      case 's':
      case 'S':
        if(ybody <= xInfo.height - 30)
          ybody = ybody + block.height;
        break;
      case 'a':
      case 'A':
        if(xbody >= 0)
          xbody = xbody - (speed/10);
        break;
      case 'd':
      case 'D':
        if(xbody <= xInfo.width - 30)
          xbody = xbody + (speed/10);
        break;
    }
  }
 void dropBomb(XInfo &xInfo){
    Bomb *bb = new Bomb(xbody, ybody);
    bombs.push_back(bb);
    while (bombs.front()->getY() > xInfo.height) {
       bombs.pop_front();
       resetX(xInfo);
    }
    return;
  }

  void dropMissile(XInfo &xInfo){
    Missile *bb = new Missile(xbody, ybody);
    missiles.push_back(bb);
    while (missiles.front()->getY() > xInfo.height) {
       missiles.pop_front();
       resetX(xInfo);
    }
    return;
  }
  int getX(){
    return xbomb;
  }

  int getY(){
    return ybomb;
  }

  void resetX(XInfo &xInfo){
    xbomb = xbody;
    ybomb = ybody;
    xmissile = xbody;
    ymissile = ybody;
  }

  void crashed(XInfo &xInfo, int x, int y, int target) {
    if (xbody > x - 25 && xbody < x + 25) {
     if (ybody + 10 + block.height/3 > xInfo.height - y) {
         drawGameOverScreen(xInfo);
      }
    }
  }
};


list<Displayable *> dList; // Initialize Columns
list<Helicopter *> heli;   // Initialize Helicopter

void setup_heli(XInfo &xInfo) {
  heli.push_back(new Helicopter(xInfo));
}

void setup_columns(XInfo & xInfo) {
  XWindowAttributes windowInfo;
  XGetWindowAttributes(xInfo.display, xInfo.window, &windowInfo);
  dList.clear();
  int i;
  int currentX = windowInfo.width/2;
  printf("currentX %d\n",currentX);
  for(i=0; i<numColumns; i++){
    int temp = rand()%6 - 3;
    if (temp < 0) {
      temp  = 0;
    } else {
      temp = 1;
    }
 dList.push_back(new Column(currentX, rand()%10, temp));
    currentX += windowInfo.width / 20;
  }
}

/*
 * Function to put out a message on error exits.
 */
void error( string str ) {
  cerr << str << endl;
  exit(0);
}

/*
 * Create a window
 */
void initX(int argc, char *argv[], XInfo &xInfo) {
  unsigned long white, black;

  /*
   * Display opening uses the DISPLAY   environment variable.
   * It can go wrong if DISPLAY isn't set, or you don't have permission.
   */
  xInfo.display = XOpenDisplay( "" );
  if ( !xInfo.display ) {
    error( "Can't open display." );
  }

  /*
   * Find out some things about the display you're using.
   */
  xInfo.screen = DefaultScreen( xInfo.display );

  white = XWhitePixel( xInfo.display, xInfo.screen );
  black = XBlackPixel( xInfo.display, xInfo.screen );


  hints.x = 100;
  hints.y = 100;
  hints.width = 800;
  hints.height = 400;
  hints.flags = PPosition | PSize;

  block.width = hints.width / 20;
  block.height = hints.width / 20;

  xInfo.window = XCreateSimpleWindow(
                                     xInfo.display,                             // display where window appears
                                     DefaultRootWindow( xInfo.display ), // window's parent in window tree
                                     hints.x, hints.y,                  // upper left corner location
                                     hints.width, hints.height, // size of the window
                                     Border,                                            // width of window's border
                                     black,                                             // window border colour
                                     white );                                   // window background colour

  XSetStandardProperties(
                         xInfo.display,         // display containing the window
                         xInfo.window,          // window whose properties are set
                         "initial",     // window's title
                         "OW",                          // icon's title
                         None,                          // pixmap for the icon
                         argv, argc,                    // applications command line args
                         &hints );                      // size hints for the window


  /*
   * Create Graphics Contexts
   */
  int i = 0;
  xInfo.gc[i] = XCreateGC(xInfo.display, xInfo.window, 0, 0);
  XSetForeground(xInfo.display, xInfo.gc[i], BlackPixel(xInfo.display, xInfo.screen));
  XSetBackground(xInfo.display, xInfo.gc[i], WhitePixel(xInfo.display, xInfo.screen));
  XSetFillStyle(xInfo.display, xInfo.gc[i], FillSolid);
  XSetLineAttributes(xInfo.display, xInfo.gc[i],
                     1, LineSolid, CapButt, JoinRound);


 // Reverse Video
  i = 1;
  xInfo.gc[i] = XCreateGC(xInfo.display, xInfo.window, 0, 0);
  XSetForeground(xInfo.display, xInfo.gc[i], WhitePixel(xInfo.display, xInfo.screen));
  XSetBackground(xInfo.display, xInfo.gc[i], BlackPixel(xInfo.display, xInfo.screen));
  XSetFillStyle(xInfo.display, xInfo.gc[i], FillSolid);
  XSetLineAttributes(xInfo.display, xInfo.gc[i],
                       1, LineSolid, CapButt, JoinRound);

  int depth = DefaultDepth(xInfo.display, DefaultScreen(xInfo.display));
  xInfo.pixmap = XCreatePixmap(xInfo.display, xInfo.window, hints.width, hints.height, depth);
  xInfo.width = hints.width;
  xInfo.height = hints.height;

  XSelectInput(xInfo.display, xInfo.window, ButtonPressMask | KeyPressMask | PointerMotionMask
    | EnterWindowMask | LeaveWindowMask);

  /*
   * Put the window on the screen.
   */
  XMapRaised( xInfo.display, xInfo.window );

  XFlush(xInfo.display);
  sleep(2);     // let server get set up before sending drawing commands
}

void drawGameOverScreen(XInfo &xInfo) {
  XClearWindow(xInfo.display, xInfo.window);
  XDrawRectangle(xInfo.display, xInfo.window, xInfo.gc[0], hints.width/4, hints.height/4, hints.width/2, hints.height/2);
  XDrawImageString(xInfo.display, xInfo.window, xInfo.gc[0], hints.width/3, hints.height/4, "GAME OVER", 9);
  char displayText[64] = "Thanks for playing game. Your Score is: ";
  char points_string[10];
  sprintf(points_string, "%d", points);
  strcat(displayText, points_string);
  XDrawImageString(xInfo.display, xInfo.window, xInfo.gc[0], hints.width/3, hints.height/2, displayText, strlen(displayText));
  XFlush(xInfo.display);
  sleep(5);


 exit(0);
}
void drawSplashScreen(XInfo &xInfo) {
  const char* displayText[] = {"             Keerthana Vijayan 20348304: Simple Side-scrolling XLib game",
  "             -----------------------------------------------------------",
  "Fly the helicopter over the buildings, and fire bombs/ missiles on the target.",
  "During the game", "f - pause game, q - quit game", "w- move helicopter up, s - move helicopter down",
  "a - move helicopter left, d - move helicopter right", "o - drop bomb, p - fire missile", "m - increase helicopter speed, n - decrease helicopter speed", ""};

  xInfo.font = XLoadQueryFont (xInfo.display, "fixed");
  XSetFont (xInfo.display, xInfo.gc[0], xInfo.font->fid);


  XClearWindow(xInfo.display, xInfo.window);
  XDrawRectangle(xInfo.display, xInfo.window, xInfo.gc[0], hints.width/8.2, hints.height/11, hints.width/1.5, hints.height/2);
  for(int i = 0; i < 10; i++) {
    XDrawImageString(xInfo.display, xInfo.window, xInfo.gc[0], hints.width/8, hints.height/8+i*14, displayText[i], strlen(displayText[i]));
  }
  XFlush(xInfo.display);
  sleep(2);
  XClearWindow(xInfo.display, xInfo.window);
}


unsigned long now() {
  timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec * 1000000 + tv.tv_usec;
}
/*
 * Function to repaint a display list
 */
void repaint(XInfo &xInfo, XWindowAttributes &windowInfo) {

 list<Displayable *>::const_iterator begin = dList.begin();
  list<Displayable *>::const_iterator end = dList.end();
  list<Bomb *>::const_iterator bomb_0 = bombs.begin();
  list<Bomb *>::const_iterator bomb_n = bombs.end();
  list<Missile *>::const_iterator missile_0 = missiles.begin();
  list<Missile *>::const_iterator missile_n = missiles.end();

  int score = 0;
  int gameover = 0;

  unsigned int height = windowInfo.height;
  unsigned int width = windowInfo.width;
  block.width = width / 20;
  block.height = height / 20;

  XFreePixmap(xInfo.display, xInfo.pixmap);
  int depth = DefaultDepth(xInfo.display, DefaultScreen(xInfo.display));
  xInfo.pixmap = XCreatePixmap(xInfo.display, xInfo.window, width, height, depth);
  xInfo.width = width;
  xInfo.height = height;

  XClearWindow( xInfo.display, xInfo.window );
  XFillRectangle(xInfo.display, xInfo.pixmap, xInfo.gc[1], 0, 0, width, height);

  heli.front()->paint(xInfo);
  while( begin != end ) {
    Displayable *d = *begin;
    d->paint(xInfo);
    heli.front()->crashed(xInfo, d->getX(), d->getY(), d->target);
    begin++;
  }





begin = dList.begin();
  end = dList.end();
  while(bomb_0 != bomb_n) {
    Bomb *b = *bomb_0;
    b->paint(xInfo);
    while(begin != end) {
      Displayable *d = *begin;
      score = b->crashed(xInfo, d->getX(), d->getY(), b->getX(), b->getY(), heli.front()->getX(), heli.front()->getY(), d->target);
      switch(score) {
        case 0:
          break;
        case 1:
          d->target = 0;
          int target;
          points+=20;
          break;
        case 2:
          points-=10;
          break;
      }
      begin++;
    }
    bomb_0++;
  }

  begin = dList.begin();
  end = dList.end();
  while(missile_0 != missile_n) {
    Missile *b = *missile_0;
    b->paint(xInfo);
    while(begin != end) {
      Displayable *d = *begin;
      score = b->crashed(xInfo, d->getX(), d->getY(), b->getX(), b->getY(), heli.front()->getX(), heli.front()->getY(), d->target);
      switch(score) {
        case 0:
          break;
        case 1:
          d->target = 0;
          int target;
          points+=20;
          break;
        case 2:
          points-=10;
          break;
      }
      begin++;
    }
    missile_0++;
  }

  begin = dList.begin();
  end = dList.end();
  while( begin != end ) {
    Displayable *d = *begin;
    if(d->target){
      gameover++;
    }
    begin++;
  }
  if (gameover == 0) {
    drawGameOverScreen(xInfo);
  }

  XCopyArea(xInfo.display, xInfo.pixmap, xInfo.window, xInfo.gc[0],
            0, 0, width, height, 10, 10);
  XFlush(xInfo.display);
}

void handleKeyPress(XInfo &xInfo, XEvent &event, XWindowAttributes &windowInfo) {
  KeySym key;
  char text[BufferSize];

  /*
   * Exit when 'q' is typed.
   * This is a simplified approach that does NOT use localization.
   */
  unsigned long lastRepaint = 0;
  unsigned long endTime = 0;
  int i = XLookupString(
    (XKeyEvent *)&event,  // the keyboard event
    text,           // buffer when text will be written
    BufferSize,       // size of the text buffer
    &key,           // workstation-independent key symbol
    NULL );         // pointer to a composeStatus structure (unused)
  if ( i == 1) {
    switch(text[0]){
      case 'q':
      case 'Q':
      drawGameOverScreen(xInfo);
        break;
      case 'w':
      case 'W':
        heli.front()->move(xInfo, 'w');
        break;
      case 'a':
      case 'A':
        heli.front()->move(xInfo, 'a');
        break;
      case 's':
      case 'S':
        heli.front()->move(xInfo, 's');
        break;
      case 'd':
      case 'D':
        heli.front()->move(xInfo, 'd');
        break;
      case 'o':
      case 'O':
        heli.front()->dropBomb(xInfo);
        break;
      case 'p':
      case 'P':
        heli.front()->dropMissile(xInfo);
        break;
      case 'f':
      case 'F':
        drawSplashScreen(xInfo);
        break;
      case 'm':
      case 'M':
        if (grader == 1)
          speed +=5;
        break;
      case 'n':
      case 'N':
        if (grader == 1)
          speed -=5;
        break;
    }
  }
}

void eventLoop(XInfo &xInfo, XEvent &event) {
  // Add stuff to paint to the display list

  unsigned long lastRepaint = 0;

  XWindowAttributes windowInfo;
  XGetWindowAttributes(xInfo.display, xInfo.window, &windowInfo);

  while( true ) {
    if (XPending(xInfo.display) > 0) {
      XNextEvent( xInfo.display, &event );
      switch( event.type ) {
       case KeyPress:
        handleKeyPress(xInfo, event, windowInfo);
        repaint(xInfo, windowInfo);
         break;
        case Expose:
                  setup_columns(xInfo);
                  printf("expose");
                  repaint(xInfo, windowInfo);
              break;
      }
    }

    list<Displayable *>::const_iterator begin = dList.begin();
    list<Displayable *>::const_iterator end = dList.end();
    list<Bomb *>::const_iterator bomb_0 = bombs.begin();
    list<Bomb *>::const_iterator bomb_n = bombs.end();
    list<Missile *>::const_iterator missile_0 = missiles.begin();
    list<Missile *>::const_iterator missile_n = missiles.end();
    Displayable *d;

      // Handle rotation of the columns list
    if ((*begin)->getX() < 0) {
      d = dList.front();
      d->resetX(xInfo);
      dList.push_back(d);
      dList.pop_front();
    }

    unsigned long endTime = now();
    if (endTime - lastRepaint > 1000000/FPS) {
      while(begin != end) {
        (*begin)->move(xInfo);
        begin++;
      }

      while(bomb_0 != bomb_n) {
        if ((*bomb_0)->bombLost == 0) {
          (*bomb_0)->move(xInfo);
          bomb_0++;
        }
        else {
          bombs.pop_front();
          break;
        }
      }


      while(missile_0 != missile_n) {
        if ((*missile_0)->missileLost == 0) {
          (*missile_0)->move(xInfo);
          missile_0++;
        }
        else {
          missiles.pop_front();
          break;
        }
      }
      repaint(xInfo, windowInfo);
      lastRepaint = now();
    } else if (XPending(xInfo.display) == 0) {
      usleep(1000000/FPS - (endTime - lastRepaint));
    }

  }
}

/*
 * Start executing here.
 *       First initialize window.
 *       Next loop responding to events.
 *       Exit forcing window manager to clean up - cheesy, but easy.
 */
int main ( int argc, char *argv[] ) {

  if (strcmp(argv[1],"grader") == 0) {
      grader = 1;
  }

  XInfo xInfo;
  XEvent event;

  initX(argc, argv, xInfo);
  drawSplashScreen(xInfo);

  setup_columns(xInfo);
  setup_heli(xInfo);
  eventLoop(xInfo, event);

  // flush pending requests
  XFlush(xInfo.display);
  // wait for user input (^D) to quit
  int q;
  cin >> q;
  XCloseDisplay(xInfo.display);
}
