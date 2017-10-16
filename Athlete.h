/*
 * filename: Athlete.h
 * describe: This is the definition file of class Athlete, an "object" talbe,
 *           which is a part of the example application for my open source
 *           project - EDS (Embedded Data Store)
 * Author:   Jerry Sun <jerysun0818@gmail.com>
 * Date:     Octobler 15, 2017
 * Remark:   No dependency. Handy
 * Linkedin: http://nl.linkedin.com/in/jerysun
 * Website:  https://sites.google.com/site/geekssmallworld
 * Github:   https://github.com/jerysun/
 */

#ifndef ATHLETE_H
#define ATHLETE_H

#include <iostream>
#include <unordered_map>
#include <memory>
#include <utility>
#include "edatastore.h"
#include "dst_util.h"

typedef int AthleteId;

struct st_ath {
  st_ath() {}
  st_ath(std::string &&nm, unsigned short rcd, std::string &&rdt) :
    name(std::move(trim(nm))), record(rcd), r_date(std::move(trim(rdt))) {}

  std::string name;      // Athlete Name
  unsigned short record; // 0 ~ 100
  std::string r_date;
};

class Athlete : public Serialize {
public:
  Athlete(AthleteId aid = 0);
  virtual ~Athlete();

  AthleteId GetAId() const;
  const std::string& GetName() const;
  AthleteId GetANo() const;
  unsigned short GetRecord() const;
  const std::string& GetRDate() const;

  void SetAId(AthleteId aid);
  void SetANo(AthleteId ano);
  void SetName(std::string &&nm);
  void SetRecord(unsigned short rcd);
  void SetRDate(std::string &&nm);

  // user interface functions
  static void Header(std::ostream& os = std::cout);
  static Athlete* Get(AthleteId aid);
  static Athlete* GetExisting(AthleteId aid);

  static void ChangeAno(AthleteId no);
  static void ChangeName(std::string &&in);
  static void ChangeRecord(unsigned short);
  static void ChangeRDate(std::string &&rdt);

  void Input(st_ath &&atl);
  void InputANo(AthleteId no);
  void InputName(std::string &&in);
  void InputRecord(unsigned short rcd);
  void InputRDate(std::string &&rdt, int len=10);

  void TryChange();

  friend std::ostream& operator<< (std::ostream &os, const Athlete &em);  
  friend void UserInput(std::string &s, std::string &&in, int len=20);

protected:
  virtual void Read();
  virtual void Write();
public:
  enum fields { ANO, NAME, RECORD, RDATE };
private:
  Key<AthleteId> aky;    // table key
  AthleteId ano;         // Athlete Number
  std::string name;      // Athlete Name
  unsigned short record; // 0 ~ 100 
  std::string r_date;    // year-months-date: 2000-02-30
};

inline AthleteId Athlete::GetAId() const {
  return aky.KeyValue();
}

inline AthleteId Athlete::GetANo() const {
  return ano;
}

inline const std::string& Athlete::GetName() const {
  return name;
}

inline unsigned short Athlete::GetRecord() const {
  return record;
}

inline const std::string& Athlete::GetRDate() const {
  return r_date;
}

inline void Athlete::SetAId(AthleteId aid) {
  aky.KeyValue() = aid;
}

inline void Athlete::SetANo(AthleteId ano) {
  this->ano = ano;
  ChangeObject();
}

inline void Athlete::SetName(std::string &&nm) {
  name = std::move(nm);
  ChangeObject();
}

inline void Athlete::SetRecord(unsigned short rcd) {
  record = rcd;
  ChangeObject();
}

inline void Athlete::SetRDate(std::string &&nm) {
  r_date = std::move(nm);
  ChangeObject();
}

#endif //ATHLETE_H