/*
 * filename: Athlete.cpp
 * describe: This is the implementation file of class Athlete, an "object"
 *           table, which is a part of the example application for my open
 *           source project - EDS (Embedded Data Store)
 * Author:   Jerry Sun <jerysun0818@gmail.com>
 * Date:     Octobler 15, 2017
 * Remark:   No dependency. Handy
 * Linkedin: http://nl.linkedin.com/in/jerysun
 * Website:  https://sites.google.com/site/geekssmallworld
 * Github:   https://github.com/jerysun/
 */

#include "stdafx.h"
#include <iomanip>
#include "Athlete.h"

static Athlete *This;

Athlete::Athlete(AthleteId aid) : aky(aid) {
  // if class not derived from Athlete
  if (aid != -1) {
    LoadObject();
  }
  This = this;
}

Athlete::~Athlete() {
  // if class not derived from Athlete
  if (aky.KeyValue() != -1) {
    SaveObject();
  }
}

// read a persitent athlete object
void Athlete::Read() {
  AthleteId aid;
  ReadObject(aid);
  aky.SetKeyValue(aid);
  ReadObject(ano);
  ReadObject(name);
  ReadObject(record);
  ReadObject(r_date);
}

// write a persitent athlete object
void Athlete::Write() {
  WriteObject(aky.KeyValue());
  WriteObject(ano);
  WriteObject(name);
  WriteObject(record);
  WriteObject(r_date);
}

// display aid athlete object
std::ostream& operator<< (std::ostream& os, const Athlete &at) {
  os.setf(std::ios::right, std::ios::adjustfield);
  os << std::setw(5) << std::setfill('0') << at.aky.KeyValue() << ' ';
  os << std::setw(5) << std::setfill('0') << at.ano << ' ';
  os.setf(std::ios::left, std::ios::adjustfield);
  os << std::setw(20) << std::setfill(' ') << at.name.substr(0, 20) << ' ';
  os << std::setw(6) << std::setfill(' ') << at.record << ' ';
  os << std::setw(10) << std::setfill(' ') << at.r_date << std::endl;
  os.flush();
  return os;
}

// try to change aid athlete object
void Athlete::TryChange() {
  if (!ChangeObject()) {
    std::cerr << "Change disallowed";
  }
}

// s is the string member address of a serialize class
// in is the input string member
// len is the length requirement
void UserInput(std::string &s, std::string &&in, int len) {
  s = std::move(in);
  s.resize(len);
}

void Athlete::ChangeAno(AthleteId no) {
  This->InputANo(no);
}

void Athlete::ChangeName(std::string &&in) {
  This->InputName(std::move(trim(in)));
}

void Athlete::ChangeRecord(unsigned short rcd) {
  This->InputRecord(rcd);
}

void Athlete::ChangeRDate(std::string &&rdt) {
  This->InputRDate(std::move(trim(rdt)));
}

void Athlete::InputANo(AthleteId no) {
  ano = no;
  if (ObjectExists()) {
    TryChange();
  }
}

// input athlete name
void Athlete::InputName(std::string &&in) {
  UserInput(name, std::move(in));
  if (ObjectExists()) {
    TryChange();
  }
}

void Athlete::InputRecord(unsigned short rcd) {
  record = rcd;
  if (ObjectExists()) {
    TryChange();
  }
}

void Athlete::InputRDate(std::string &&rdt, int len) {
  r_date = std::move(rdt);
  r_date.resize(len);
  if (ObjectExists()) {
    TryChange();
  }
}

// athlete input
void Athlete::Input(st_ath &&atl) {
  InputName(std::move(atl.name));
  InputRecord(atl.record);
  InputRDate(std::move(atl.r_date));
}

// get an existing athlete object
Athlete* Athlete::GetExisting(AthleteId aid) {
  Athlete *athlete = Get(aid);
  if (athlete) {
    if (athlete->ObjectExists()) {
      Header();
      std::cout << *athlete;
    }
    else {
      std::cerr << "No such athlete on file\n";
      delete athlete;
      athlete = 0;
    }
  }
  return athlete;
}

// get an athlete object
Athlete* Athlete::Get(AthleteId aid) {
  Athlete *athlete = 0;
  if (aid != 0) {
    athlete = new Athlete(aid);
  }
  return athlete;
}

// display athlete object header in report
void Athlete::Header(std::ostream &os) {
  os << std::endl;
  os << "AtId# AtNo# Athlete Name         Record RecordDate" << std::endl;
  os << "----- ----- -------------------- ------ ----------" << std::endl;
  os.flush();
}