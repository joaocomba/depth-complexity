#include "timer.h"

#include <cstdio>
#include <sys/time.h>

// Get the value of a system timer with a millisecond precision.
// for a Unix-like OS (Linux, Solaris, BSD, MacOSX, Irix, ...).
inline unsigned long time() {
  struct timeval st_time;
  gettimeofday(&st_time,0);
  return (unsigned long)(st_time.tv_usec/1000 + st_time.tv_sec*1000);
}

// Implement a tic/toc mechanism to display elapsed time of algorithms.
inline unsigned long tictoc(const bool is_tic, const char *message="") {
  static unsigned long t0 = 0;
  const unsigned long t = time();
  if (is_tic) return (t0 = t);
  const unsigned long dt = t>=t0?(t - t0):~0UL;
  const unsigned int
    edays = (unsigned int)(dt/86400000.0),
    ehours = (unsigned int)((dt - edays*86400000.0)/3600000.0),
    emin = (unsigned int)((dt - edays*86400000.0 - ehours*3600000.0)/60000.0),
    esec = (unsigned int)((dt - edays*86400000.0 - ehours*3600000.0 - emin*60000.0)/1000.0),
    ems = (unsigned int)(dt - edays*86400000.0 - ehours*3600000.0 - emin*60000.0 - esec*1000.0);
  if (!edays && !ehours && !emin && !esec)
    std::printf("[TIME: %s] Elapsed time : %u ms\n", message, ems);
  else {
    if (!edays && !ehours && !emin)
      std::printf("[TIME: %s] Elapsed time : %u sec %u ms\n", message, esec,ems);
    else {
      if (!edays && !ehours)
        std::printf("[TIME: %s] Elapsed time : %u min %u sec %u ms\n", message, emin,esec,ems);
      else{
        if (!edays)
          std::printf("[TIME: %s] Elapsed time : %u hours %u min %u sec %u ms\n", message, ehours,emin,esec,ems);
        else{
          std::printf("[TIME: %s] Elapsed time : %u days %u hours %u min %u sec %u ms\n", message, edays,ehours,emin,esec,ems);
        }
      }
    }
  }
  return t;
}

unsigned long tic() {
  return tictoc(true);
}

unsigned long toc(const char *message) {
  return tictoc(false, message);
}

