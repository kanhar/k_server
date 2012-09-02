#include <iostream>
#include <fstream>
#include <stdio.h>
#include <cstdlib>
#include <stack>
#include <string>
#include <cstring>
#include <math.h>
#include <sstream>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

/***************************************************************/
/**************************Classes******************************/
/***************************************************************/

/***************************************************************/
// Class Used a data structure to hold a Cartesian Point (x,y)
/***************************************************************/
class Point {
public:
   int x;
   int y;

   Point(){
       x=-1;
       y=-1;
   }

   Point(int pX, int pY){
       x = pX;
       y = pY;
   }


   Point(const Point& p1)
   {
        x =      p1.x;
        y =      p1.y;
   }


   bool IsLessThan(Point p)
   {
       if (x < p.x)
           return true;
       else
           return false;
   }

   string GetFull()
   {
       string tmp= "";
       tmp+= "P";
       tmp+= "(";
       tmp+= toString(x);
       tmp+= ",";
       tmp+= toString(y);
       tmp +=")";

       return tmp;
   }

   string toString(int num)
   {
        std::stringstream ss;
        ss << num;
        return ss.str();
   }

};




/***************************************************************/
// Class Used to represent an Edge(or Line) between two Points**
/***************************************************************/

class Line  {
public:
    Point start;
    Point end;
    float distance;

    Line()
    {
        start = Point();
        end = Point();
        distance = 0.0;
    }

    Line(Point a, Point b)
    {
        start = a;
        end =b;
    }

    Line(const Line& l1)
    {
        start = Point(l1.start);
        end = Point(l1.end);
        distance = l1.distance;
    }

    float CalculateDistance(Point a, Point b){
        return sqrt (pow(b.y - a.y, 2) + pow(b.x - a.x,2) );
    }

    void CalculateDistance()
    {
        distance = CalculateDistance(start,end);
    }

    string GetLine()
    {
        string tmp = "";
        tmp+= "Line \t";
        tmp+= start.GetFull();
        tmp+= "\t";
        tmp+= end.GetFull();
        tmp+= "\t\t DIST: ";
        tmp+= toString(distance);
        return tmp;
    }


    string toString(float num)
    {
         std::stringstream ss;
         ss << num;
         return ss.str();
    }

};




/***************************************************************/
// Class Used to represent the state of servers at time t
/***************************************************************/

class State  {
public:
    int A;
    int B;
    int C;
    int t;
    int lastState;
    float cost;

    State()
    {
        A = -1;
        B = -1;
        C = -1;
        t = -1;
        cost = 0;
        lastState = -1;
    }

    State(const State& s)
    {
        A = s.A;
        B = s.B;
        C = s.C;
        t = s.t;
        cost = s.cost;
        lastState = s.lastState;
    }

    State(int pT, int pA, int pB, int pC, int pLastState, float pCost)
    {
        t = pT;
        A = pA;
        B = pB;
        C = pC;
        lastState = pLastState;
        cost = pCost;
    }

    void Print()
    {
        printf("State: \t %d \t%d\t\t%d\t%d\t%d \t\t %f \n",  lastState, t, A, B, C, cost);
    }

};


//Graphics Code
char *win_name_string       =   (char*)"K-Server Problem";
char *icon_name_string      =   (char*)"K-Server";
char *incorrect_usage       =   (char*)"Usage: g++ main.cpp -lX11 -lm -L/usr/X11R6/lib \n";
const bool VERBOSE          =   true;

Display *display_ptr;
Screen *screen_ptr;
int screen_num;
char *display_name = NULL;
int display_width, display_height;

Window win;
int border_width;
int win_width, win_height;
int win_x, win_y;

XWMHints *wm_hints;
XClassHint *class_hints;
XSizeHints *size_hints;
XTextProperty win_name, icon_name;

XEvent event;

GC gc, gc_yellow, gc_red, gc_grey;
unsigned long valuemask = 0;
XGCValues gc_values, gc_yellow_values, gc_red_values, gc_grey_values;
Colormap color_map;
XColor tmp_color1, tmp_color2;

//Graphic Functions
void Draw();
void InitXLib(int argc, char **argv);
void DrawLine(GC pGC, int a, int b, int c, int d);
void DrawPoint(GC pGC, int a, int b);
void DrawLine(GC pGC, Line e);

void DrawFullPoint(GC pGC, Point p, string msg);
void DrawPoint(GC pGC,Point a);
Point GetMouseClick();
void ClearArea();
//Core Code
int MAX_H=350;
int MAX_W=580;

int p_ptr, s_ptr;
Point serverA, serverB, serverC, *points;
State *states;

//Core Functions
void AddPoint(Point p);
void AddState(State s);

void AddPointAndState(Point p);
void PrintStates();
float CalculateDistance(Point a, Point b);
Point GetPoint(int index);
State GetState(int index);

void DrawOptimumPath();
void DrawNearestPath();
void RenderAllPoints();
void PrintState(State curState);
void DrawLineBetweenStates(State a, State b);
void MoveClosestServer(Point p);

//Begin Program
int main(int argc, char **argv)
{       
    InitXLib(argc, argv);

    points = new Point[100];
    states = new State[99999999];

    p_ptr=0;
    s_ptr=0;

    serverA =Point(10,       MAX_H);
    serverB =Point(MAX_W,    MAX_H);
    serverC =Point(MAX_W/2,  10);

    AddPoint(serverA);
    AddPoint(serverB);
    AddPoint(serverC);

    //Add Seed State
    AddState(State(0, 0,1,2,-1, 0));

    PrintStates();
    Draw();
    return 0;
}

void AddPointAndState(Point p)
{
    State oldState, tminus1State;
    int i, newPoint, lenStates;
    float curDist;

    AddPoint(p);
    MoveClosestServer(p);
    curDist=-1;

    oldState = states[s_ptr-1];
    newPoint = p_ptr-1;
    lenStates = s_ptr;

    //Add States
    //Loop through each state for t = t-1, and insert new State for every position of A,B,C
    for(i=0;i<lenStates;i++)
    {
        tminus1State = states[i];

        if (tminus1State.t == oldState.t)
        {
            curDist = CalculateDistance(GetPoint(newPoint),GetPoint(tminus1State.A));
            AddState(State(oldState.t+1, newPoint,      tminus1State.B,     tminus1State.C, i, curDist  + tminus1State.cost));

            curDist = CalculateDistance(GetPoint(newPoint),GetPoint(tminus1State.B));
            AddState(State(oldState.t+1, tminus1State.A,    newPoint,       tminus1State.C, i, curDist   + tminus1State.cost));

            curDist = CalculateDistance(GetPoint(newPoint),GetPoint(tminus1State.C));
            AddState(State(oldState.t+1, tminus1State.A,    tminus1State.B,     newPoint,   i, curDist  + tminus1State.cost));
        }
    }

}

//Draws all Obstacles, Points, and Edges
void Draw()
{
    while(1)
    {
        XNextEvent( display_ptr, &event );

        if (event.type == Expose)
        {
            RenderAllPoints();
        }

        if (event.type == ButtonPress)
        {
            Point p = GetMouseClick();

            if (event.xbutton.button == 3)
            {
                //RenderAllPoints();
                DrawOptimumPath();
            }
            else
            {
                AddPointAndState(p);

                event.type = ButtonRelease;
                event.xbutton.state = 0x100;
                RenderAllPoints();

            }

        }



    }
}

void DrawOptimumPath()
{
    int i, posMIN;
    float MIN;
    bool first;
    State lastState;

    posMIN = 0;
    MIN = 99999;
    first=true;

    //Find Lowest Distance
    for(i=1;i<s_ptr;i++)
    {
        if (states[i].cost < MIN && states[s_ptr-1].t == states[i].t)
        {
            MIN = states[i].cost;
            posMIN = i;
        }
    }

    while(posMIN != -1)
    {

        if (!first)
        {
            DrawLineBetweenStates(lastState, GetState(posMIN));
        }

        lastState = GetState(posMIN);
        lastState.Print();
        posMIN = lastState.lastState;
        first=false;
    }

    printf("PosMIN = %d \n", posMIN);

}


void MoveClosestServer(Point p)
{
    float distA, distB, distC;

    distA = CalculateDistance(p, serverA);
    distB = CalculateDistance(p, serverB);
    distC = CalculateDistance(p, serverC);

    if (distA < distB && distA < distC)
    {
        DrawLine(gc_grey,Line(serverA, p));
        serverA = Point(p);
    }

    if (distB < distA && distB < distC)
    {
        DrawLine(gc_grey,Line(serverB, p));
        serverB = Point(p);

    }
    if (distC < distA && distC < distB)
    {
        DrawLine(gc_grey,Line(serverC, p));
        serverC = Point(p);
    }

}



State GetState(int index)
{
    return states[index];
}

Point GetPoint(int index)
{
    return points[index];
}

void AddState(State s)
{
    states[s_ptr++] = s;
}

void AddPoint(Point p)
{
    points[p_ptr++] = p;
}

float CalculateDistance(Point a, Point b){
    return sqrt (pow(b.y - a.y, 2) + pow(b.x - a.x,2) );
}


/***************************************************************/
/****************Graphics Xlix Code******************************/
/***************************************************************/
void InitXLib(int argc, char **argv)
{
/* opening display: basic connection to X Server */
  if( (display_ptr = XOpenDisplay(display_name)) == NULL )
    { printf("Could not open display. \n"); exit(-1);}

  printf("Connected to X server  %s\n", XDisplayName(display_name) );

  screen_num 	 = DefaultScreen( display_ptr );
  screen_ptr 	 = DefaultScreenOfDisplay( display_ptr );
  color_map  	 = XDefaultColormap( display_ptr, screen_num );
  display_width  = DisplayWidth( display_ptr, screen_num );
  display_height = DisplayHeight( display_ptr, screen_num );

  printf("Width %d, Height %d, Screen Number %d\n",
           display_width, display_height, screen_num);

  /* creating the window */
  border_width = 10;
  win_x = 100; win_y = 100;
  win_width = display_width/2;
  win_height = (int) (win_width / 1.7); /*rectangular window*/


  win= XCreateSimpleWindow( display_ptr, RootWindow( display_ptr, screen_num),
                            win_x, win_y, win_width, win_height, border_width,
                            BlackPixel(display_ptr, screen_num),
                            WhitePixel(display_ptr, screen_num) );

  /* now try to put it on screen, this needs cooperation of window manager */
  size_hints 	= XAllocSizeHints();
  wm_hints 	= XAllocWMHints();
  class_hints 	= XAllocClassHint();

  if( size_hints == NULL || wm_hints == NULL || class_hints == NULL )
    { printf("Error allocating memory for hints. \n"); exit(-1);}

  size_hints -> flags = PPosition | PSize | PMinSize  ;
  size_hints -> min_width = 60;
  size_hints -> min_height = 60;

  XStringListToTextProperty( &win_name_string,1,&win_name);
  XStringListToTextProperty( &icon_name_string,1,&icon_name);

  wm_hints -> flags = StateHint | InputHint ;
  wm_hints -> initial_state = NormalState;
  wm_hints -> input = False;

  class_hints -> res_name = (char*)"x_use_example";
  class_hints -> res_class = (char*)"examples";

  XSetWMProperties( display_ptr, win, &win_name, &icon_name, argv, argc,
                    size_hints, wm_hints, class_hints );

  /* what events do we want to receive */
  XSelectInput( display_ptr, win,            ExposureMask | StructureNotifyMask | ButtonPressMask );

  /* finally: put window on screen */
  XMapWindow( display_ptr, win );

  XFlush(display_ptr);

  /* create graphics context, so that we may draw in this window */
  gc = XCreateGC( display_ptr, win, valuemask, &gc_values);
  XSetForeground( display_ptr, gc, BlackPixel( display_ptr, screen_num ) );
  XSetLineAttributes( display_ptr, gc, 1,LineOnOffDash, CapRound, JoinRound);

  //Yellow
  gc_yellow = XCreateGC( display_ptr, win, valuemask, &gc_yellow_values);
  XSetLineAttributes(display_ptr, gc_yellow, 1, LineSolid,CapRound, JoinRound);
  if( XAllocNamedColor( display_ptr, color_map, "yellow", &tmp_color1, &tmp_color2 ) == 0 )
    {printf("failed to get color yellow\n"); exit(-1);}
  else
    XSetForeground( display_ptr, gc_yellow, tmp_color1.pixel );

  //Red
  gc_red = XCreateGC( display_ptr, win, valuemask, &gc_red_values);
  XSetLineAttributes( display_ptr, gc_red, 2, LineSolid, CapRound, JoinRound);
  if( XAllocNamedColor( display_ptr, color_map, "red",&tmp_color1, &tmp_color2 ) == 0 )
    {printf("failed to get color red\n"); exit(-1);}
  else
    XSetForeground( display_ptr, gc_red, tmp_color1.pixel );

  //Grey
  gc_grey = XCreateGC( display_ptr, win, valuemask, &gc_grey_values);
  if( XAllocNamedColor( display_ptr, color_map, "light grey", &tmp_color1, &tmp_color2 ) == 0 )
    {printf("failed to get color grey\n"); exit(-1);}
  else
    XSetForeground( display_ptr, gc_grey, tmp_color1.pixel );

}


void DrawPoint(GC pGC,Point a)
{
    DrawPoint(pGC, a.x, a.y);
}

void DrawFullPoint(GC pGC, Point p, string msg)
{
    char* buffer = new char[50];
    int len;

    len = sprintf (buffer, "%s(%d,%d)", msg.c_str(), p.x, p.y);

    XDrawImageString(display_ptr, win, pGC, p.x, win_height - p.y, buffer,len);    

}

//Draws a line between point(a,b) and point(c,d)
void DrawLine(GC pGC, int a, int b, int c, int d)
{
    b = win_height - b;

    XDrawLine(display_ptr, win, pGC,a, b,c,win_height -  d);
}

//Draws a line between point(a,b) and point(c,d)
void DrawLine(GC pGC, Line e)
{
    DrawLine(pGC, e.start.x, e.start.y, e.end.x, e.end.y);
    if (VERBOSE)
    {
        DrawFullPoint(gc_grey,    e.start,     e.start.GetFull() );
        DrawFullPoint(gc_grey,    e.end,       e.end.GetFull() );
    }
}

//Draws a circle centered at P(a,b)
//Note: The bounding rectangle of an arc refers to the smallest rectangle that completely contains the arc)
void DrawPoint(GC pGC, int a, int b)
{
    b = win_height -  b;

    //Two unsigned integers to specify the width and height of the bounding rectangle of the arc
    unsigned int widthBoundingRect = 10;
    unsigned int heightBoundingRect = 10;

    //Two integers that specify the upper left corner of the bounding rectangle of the arc
    //Note: as the function is required to center the circle at a,b, we need to calculate the values of the upperlefCorner
    // as below
    int upperLeftCornerBoundingRecta = a-widthBoundingRect/2;
    int upperLeftCornerBoundingRectb = b-widthBoundingRect/2;

    //finally two integers to specify the start and extent of the arc.
    int startArc = 0;
    int extentArc = 360*64;

    XFillArc( display_ptr, win, pGC, upperLeftCornerBoundingRecta,upperLeftCornerBoundingRectb,widthBoundingRect,heightBoundingRect, startArc, extentArc);
}



void PrintStates()
{
    int i, oldTime;
    State curState;
    printf("State: \t i \t Last \t t \t\tA \t B \t C \t\t Cost \n\n");

    oldTime = 0;

    for(i=0;i<s_ptr;i++)
    {
        curState = states[i];
        if (curState.t != oldTime)
        {
            printf("\n");
            oldTime = curState.t;
        }
        printf("State: \t %d \t %d \t%d\t\t%d\t%d\t%d \t\t %f \n", i, curState.lastState, curState.t, curState.A, curState.B, curState.C, curState.cost);
    }
}




void ClearArea()
{
    XClearArea(display_ptr, win, 0, 0, win_width, win_height, false);
}



void DrawLineBetweenStates(State a, State b)
{
    Line l;

    if (a.A == b.A && a.B == b.B)
    {
        printf("1Line between Points %d and %d \n", a.C, b.C);
        l = Line(GetPoint(a.C), GetPoint(b.C));
    }

    if (a.B == b.B && a.C == b.C)
    {
        printf("2Line between Points %d and %d \n", a.A, b.A);
        l = Line(GetPoint(a.A), GetPoint(b.A));
    }

    if (a.A == b.A && a.C == b.C)
    {
        printf("3Line between Points %d and %d \n", a.B, b.B);
        l = Line(GetPoint(a.B), GetPoint(b.B));
    }

    DrawLine(gc_red, l);

}


void RenderAllPoints()
{
    int i;
    Point curPoint;
    for(i=0;i<p_ptr;i++)
    {
        curPoint = points[i];
        DrawPoint(gc_yellow, curPoint);
        DrawFullPoint(gc_grey, curPoint, curPoint.toString(i));
    }
}


Point GetMouseClick()
{
    XQueryPointer(display_ptr,
                  RootWindow(display_ptr, DefaultScreen(display_ptr)),
                  &event.xbutton.root,
                  &event.xbutton.window,
                  &event.xbutton.x_root,
                  &event.xbutton.y_root,
                  &event.xbutton.x,
                  &event.xbutton.y,
                  &event.xbutton.state);

    int x = event.xbutton.x - 100;
    int y = 500 - event.xbutton.y;

    Point p = Point(x,y);
    return p;
}
