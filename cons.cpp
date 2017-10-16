/*
 * filename: cons.cpp
 * describe: This is the implementation file of CONS (Console
 *           User Interface) classes
 * Author:   Jerry Sun <jerysun0818@gmail.com>
 * Date:     June 1, 2001
 * Remark:   No dependency. Handy
 * Linkedin: http://nl.linkedin.com/in/jerysun
 * Website:  https://sites.google.com/site/geekssmallworld
 * Github:   https://github.com/jerysun/
 */

#include "stdafx.h"
#include <ctype.h>
#include <conio.h>
#include "cons.h"

#pragma warning (disable: 4244)
CONS *CONS::cons;

// Menu user interface
Menu::Menu() throw(NoCONS) {
  selection = 0;
  cmdno = 0;
  if (CONS::cons == 0) {
    throw NoCONS();
  }
}

void Menu::Select(bool clearscreen) {
  for (;;) {
    unsigned char sl = CONS::cons->GetKBChar();
    if (isprint(sl)) {
      std::cout << sl << '\b';
    }

    if (sl == ESC) {
      selection = ESC;
      break;
    }

    // search for a match among the selections
    for (selection = 0; selection < cmdno; selection++) {
      if (selcodes[selection] == tolower(sl)) {
        // execute the command
        if (clearscreen) {
          CONS::cons->ClearScreen();
        }
        (*sels[selection])();
        break;
      }
    }
    if (selection < cmdno) break;
    std::cout << '\a';
  }
}

// full screen menu user interface
ScreenMenu::ScreenMenu(const char *ttl, ...) throw(BadMenu) {
  title = ttl;
  va_list ap;
  va_start(ap, ttl);
  char cc = '1';
  // gather the menu from the caller's parameters
  while (cmdno < MAXCOMMANDS) {
    char *sp = va_arg(ap, char *);
    if (sp == 0) break;

    labels[cmdno] = sp;
    selcodes[cmdno] = cc++;
    sels[cmdno] = va_arg(ap, Mfunc);
    if (sels[cmdno++] == 0) throw BadMenu();
  }
}

void ScreenMenu::Execute() {
  selection = 0;
  int i;
  while (selection != ESC) {
    CONS::cons->ClearScreen();
    CONS::cons->SetCursor(menux, menuy);
    // display the menu
    std::cout << title;
    for (i = 0; i < cmdno; i++) {
      CONS::cons->SetCursor(menux + 2, menuy + 2 + i);
      std::cout << (i + 1) << ": " << labels[i];
    }
    CONS::cons->SetCursor(menux, menuy + 2 + i);
    std::cout << "Esc: Return";

    // get the user's selection
    CONS::cons->SetCursor(menux, menuy + 4 + i);
    std::cout << "Select> ";
    Select(true);
  }
}

// one line menu user interface
OneLineMenu::OneLineMenu(const char *menu, ...) throw(BadMenu) {
  menuline = menu;
  // collect and count the menu commands
  const char *cp = menu;
  cmdct = 0;
  for (int sel = 0; sel < MAXCOMMANDS && *cp; sel++) {
    for (; *cp && isspace(*cp); ++cp);

    selcodes[cmdct++] = tolower(*cp);
    for (; *cp && !isspace(*cp); ++cp);
  }
  // gather the menu functions
  va_list ap;
  va_start(ap, menu);

  while (cmdno < cmdct) {
    sels[cmdno] = va_arg(ap, Mfunc);
    if (sels[cmdno++] == 0) throw BadMenu();
  }
}

void OneLineMenu::Execute() {
  // display the menu
  std::cout << std::endl << menuline << ": ";

  selection = 0;
  // get the user's selection
  Select();
}

// std::string input
void CONS::UserInput(std::string *s, const char *prompt, int len) {
  std::cout << std::endl << prompt << ": ";
  std::cout.flush();
  char *bf = new char[len + 1];
  std::cin >> ws;
  std::cin.getline(bf, len + 1);
  *s = bf;
  s->resize(len);
}

// character input
void CONS::UserInput(char *c, const char *prompt) {
  std::cout << std::endl << prompt << ": ";
  std::cout.flush();
  *c = GetKBChar();
}

// int integer input
void CONS::UserInput(int *i, const char *prompt) {
  std::cout << std::endl << prompt << ": ";
  std::cout.flush();
  std::cin >> ws >> *i;
}

// long integer input
void CONS::UserInput(long int *l, const char *prompt) {
  std::cout << std::endl << prompt << ": ";
  std::cout.flush();
  std::cin >> ws >> *l;
}

// Date input
void CONS::UserInput(Date *dt, const char *prompt) {
  std::cout << std::endl << prompt << " (mm dd yy): ";
  std::cout.flush();
  unsigned int mo, da, yr;
  std::cin >> ws >> mo >> da >> yr;
  *dt = Date(mo, da, yr);
}

// Currency input
void CONS::UserInput(Currency *c, const char *prompt) {
  float value;
  std::cout << std::endl << prompt << ": ";
  std::cout.flush();
  std::cin >> ws >> value;
  c->setValue(value);
}

// yes/no user interface
bool CONS::YesNo(const char *question) {
  char c = ' ';
  while (c != 'y' && c != 'n') {
    std::cout << std::endl << question << "? (y/n) ";
    std::cout.flush();
    c = GetKBChar();
    c = tolower(c);
    if (c != 'y' && c != 'n')
      std::cout << '\a';
  }
  std::cout.flush();
  return c == 'y';
}

// error message
void CONS::Error(const char *message) const {
  std::cout << std::endl << '\a' << message << std::endl;
  AnyKey();
}

// get a keyboard char
unsigned char CONS::GetKBChar() const {
  int c;
  if (putback) {
    c = putback;
    (const_cast<CONS*>(this))->putback = 0;
  } else {
    c = _getch();
    if (c == 0) {
      // convert function key
      c = _getch() | 0x80;
    } else {
      // strip scan code
      c &= 0xff;
    }
  }
  return static_cast<unsigned char>(c);
}

// put back a keyboard char
void CONS::PutBack(char c) { putback = c; }

// Display a status line
void CONS::StatusLine(const std::string &s) const {
  SetCursor(0, SCREENHEIGHT - 1);
  std::cout << s;
  std::cout.flush();
  int len = (int)strlen(s.c_str());
  while (len++ < SCREENWIDTH - 1) {
    std::cout << ' ';
  }
  std::cout.flush();
}

// Prompt for and read any key
void CONS::AnyKey() const {
  std::cout << std::endl << "Any key..." << std::endl;
  GetKBChar();
}

// Position the cursor
void CONS::SetCursor(int x, int y) const {
  std::cout << "\033[" << (y + 1) << ';' << (x + 1) << 'H';
  std::cout.flush();
}

// Clear the screen
void CONS::ClearScreen() const {
  std::cout << "\033[2J";
  std::cout.flush();
}

// Write a single character to the screen
void CONS::WriteChar(char c, int x, int y) const {
  SetCursor(x, y);
  std::cout << c;
  std::cout.flush();
}

// Test for a keyboard character waiting
bool CONS::KBCharWaiting() const { return _kbhit() != 0; }