#include <iostream>
#include <fstream>
#include <stdio.h>
#include <cstdlib>
#include <stack>
#include <string>
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
