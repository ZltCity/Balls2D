#pragma once

#include <chrono>

class Timer {
public:
  Timer();

  float reset();

private:
  //  ������ ��� ����������� �����������, ����� �� ������ ������������ ����������� ���������� ������.
  typedef std::chrono::high_resolution_clock        Clock;
  typedef std::chrono::time_point<Clock>            TimePoint;
  typedef std::chrono::duration<float, std::milli>  Duration;

  TimePoint last;
};
