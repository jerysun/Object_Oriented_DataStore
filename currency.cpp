/*
 * filename: currency.cpp
 * describe: This is the implementation of class Currency, a helper 
 *           used by class Cons and others for currency operations
 * Author:   Jerry Sun <jerysun0818@gmail.com>
 * Date:     May 30, 2001
 * Remark:   No dependency. Handy
 * Linkedin: http://nl.linkedin.com/in/jerysun
 * Website:  https://sites.google.com/site/geekssmallworld
 * Github:   https://github.com/jerysun/
 */
 
#include "stdafx.h"
#include <cmath>
#include <iomanip>
#include "currency.h"

// construct Currency from float
// adjust to even cents
Currency::Currency(float v, unsigned char w) {
  float vl = fabs(v);
  long dol = (long) vl;
  int ct = (int)((vl - dol) * 100);
  value = ct * 1.0f;
  value /= 100;
  value += dol;
  if (v < 0)
    value *= -1;
  wd = w;
}

inline float& Currency::Value() {
  return value;
}

void Currency::setValue(float val) {
  value = val;
}

inline bool Currency::operator<(const Currency& m) const {
  return value < m.value;
}

inline bool Currency::operator==(const Currency& m) const {
  return value == m.value;
}

inline bool Currency::operator!=(const Currency& m) const {
  return value != m.value;
}

inline bool Currency::operator>(const Currency& m) const {
  return value > m.value;
}

inline bool Currency::operator<=(const Currency& m) const {
  return value <= m.value;
}

inline bool Currency::operator>=(const Currency& m) const {
  return value >= m.value;
}

// ---- std::ostream Currency insertion operator
std::ostream& operator<<(std::ostream& os, const Currency& m) {
  os << '$';
  os.width(m.wd);
  os.precision(2);
  os.setf(std::ios::fixed | std::ios::right);
  os << m.value;
  return os;
}
