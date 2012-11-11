#ifndef TIMER_H
#define TIMER_H

#include <iostream>
#include <fstream>

/*CPU Frequency in GhZ*/
const double CPU_FREQ = 2.668;

/*Cycle Time in ns*/
const double CYCLE_TIME = 1.0/CPU_FREQ;


using namespace std;

typedef unsigned long long ticks;

/**
 * A Timer that can measure multiple runs and return an
 * average delta in nanoseconds.
 */
class Timer
{

public:

  Timer();

  inline void start();

  inline void stop();

  inline void reset() {n=deltaSum=0;}
  
  /*
   * Returns -1 if the delta is invalid.
   */
  inline double getLastDelta() {return (t1-t0)/CPU_FREQ;}

  inline double getAverageDelta() {return (deltaSum/n)/CPU_FREQ;}

  inline double getDeltaSum() {return deltaSum/CPU_FREQ;}

  inline bool timerInvalid() {return invalid;} 

private:

  inline ticks getTicks();

private:
  ticks t0;
  ticks t1;

  /*True if t0 and t1 are not
    taken from the same CPU*/
  bool invalid;
  
  /*To Store CPU ID*/
  unsigned procID;
  static unsigned c;

  double deltaSum;
  unsigned n;
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//IMPLEMENTATION OF INLINE METHODS
////////////////////////////////////////////////////////////////////////////////

inline void Timer::start() 
{
  t0=getTicks();
  procID = c;
//  cout << c << endl;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

inline void Timer::stop()
{
  t1=getTicks();
  n++;
  deltaSum += t1-t0;
//  cout << c << endl;
  invalid = (c != procID);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

inline ticks Timer::getTicks()
{
  unsigned a,d; 
  asm volatile("rdtscp" : "=a" (a), "=d" (d), "=c" (c));
  
  return ((ticks)a) | (((ticks)d) << 32); 
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#endif
