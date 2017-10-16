/*
 * filename: currency.h
 * describe: This is the definition file of class Currency, a helper 
 *           used by class Cons and others for currency operations
 * Author:   Jerry Sun <jerysun0818@gmail.com>
 * Date:     May 30, 2001
 * Remark:   No dependency. Handy
 * Linkedin: http://nl.linkedin.com/in/jerysun
 * Website:  https://sites.google.com/site/geekssmallworld
 * Github:   https://github.com/jerysun/
 */

#ifndef CURRENCY_H
#define CURRENCY_H

#include <iostream>

class Currency {
public:
  Currency(float v=0, unsigned char w = 6);

  float& Value();
  void setValue(float val);
  bool operator < (const Currency& m) const;
  bool operator==(const Currency& m) const;
  bool operator!=(const Currency& m) const;
  bool operator>(const Currency& m) const;
  bool operator<=(const Currency& m) const;
  bool operator>=(const Currency& m) const;

  friend std::ostream& operator<<(std::ostream& os, const Currency& m);
private:
  float value;
  unsigned char wd; // display width
};

#endif
