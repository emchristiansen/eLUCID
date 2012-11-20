// Author: Andrew Ziegler <andrewzieg@gmail.com>
// A timer utility class.

#ifndef LUCID_TIMER_H
#define LUCID_TIMER_H

#include <ctime>
#include <string>
#include <vector>

namespace lucid
{
  class Timer
  {
  public:
    static Timer global_timer;

    void start()
    {
      _start = std::clock();
    };
    
    void stop(const std::string& log_message)
    { 
      double elapsed_time = std::clock() / static_cast<double>(CLOCKS_PER_SEC);
      _timings.push_back(elapsed_time);
      _log.push_back(log_message);
    }

    /**
     * Prints the timings and the associated log messages
     * to the standard output.
     */
    void printLog();

  private:
    std::clock_t _start;
    std::vector<std::string> _log;
    std::vector<double> _timings;
  };

} // namespace lucid

#endif
