/*
 * filename: cons.h
 * describe: This is the definition file of CONS (Console User 
 *           Interface) classes
 * Author:   Jerry Sun <jerysun0818@gmail.com>
 * Date:     May 30, 2001
 * Remark:   No dependency. Handy
 * Linkedin: http://nl.linkedin.com/in/jerysun
 * Website:  https://sites.google.com/site/geekssmallworld
 * Github:   https://github.com/jerysun/
 */

#ifndef CONS_H
#define CONS_H

#include <iostream>
#include <iomanip>
#include <cstdarg>
#include <cstring>
#include <string>
#include "date.h"
#include "currency.h"

#pragma warning (disable: 4290)

// exceptions
class BadMenu {};
class NoCONS {};

// screen dimensions
const int SCREENHEIGHT = 25;
const int SCREENWIDTH = 80;

// function key values
const int UPARROW = 200;
const int DOWNARROW = 208;
const int RIGHTARROW = 205;
const int LEFTARROW = 203;
const int ESC = 27;

// Console User Interface class
class CONS {
public:
  CONS() {
    putback = 0;
    cons = this;
  }
  ~CONS() { cons = 0; }

  // data field entry
  void UserInput(std::string *s, const char *prompt, int len);
  void UserInput(char *c, const char *prompt);
  void UserInput(int *i, const char *prompt);
  void UserInput(long int *i, const char *prompt);
  void UserInput(Date *dt, const char *prompt);
  void UserInput(Currency *c, const char *prompt);
  // standard dialogs
  bool YesNo(const char *question);
  void Error(const char *message) const;
  void AnyKey() const;
  // screen operations
  void SetCursor(int x, int y) const;
  void ClearScreen() const;
  void WriteChar(char c, int x, int y) const;
  void StatusLine(const std::string &s) const;
  // keyboard operations
  bool KBCharWaiting() const;
  unsigned char GetKBChar() const;
  void PutBack(char c);
  static CONS *cons;
private:
  char putback;
  int ws;
};

typedef void (*Mfunc)();
// maximum commands in a menu
const int MAXCOMMANDS = 9;
// where to display menus
const int menux = (SCREENWIDTH / 2 - SCREENWIDTH / 4);
const int menuy = (SCREENHEIGHT / 2 - SCREENHEIGHT / 4);

// Generic Menu Class
class Menu {
public:
  Menu() throw(NoCONS);
  virtual void Execute() = 0;
protected:
  Mfunc sels[MAXCOMMANDS];
  char selcodes[MAXCOMMANDS];
  int selection;
  int cmdno;
  void Select(bool clearscreen = false);
};

class ScreenMenu : public Menu {
public:
  ScreenMenu(const char *title, ...) throw(BadMenu);
  void Execute();
private:
  std::string title;
  std::string labels[MAXCOMMANDS];
};

class OneLineMenu : public Menu {
public:
  OneLineMenu(const char *menu, ...) throw(BadMenu);
  void Execute();
private:
  int cmdct;
  std::string menuline;
};

#endif
