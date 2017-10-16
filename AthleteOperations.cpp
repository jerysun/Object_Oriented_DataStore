/*
 * filename: AthleteOperations.cpp
 * describe: This is the implementation file of the operations of class Athlete,
 *           an "object" talbe, which is a part of the example application
 *           for my open source project - EDS (Embedded Data Store)
 * Author:   Jerry Sun <jerysun0818@gmail.com>
 * Date:     Octobler 15, 2017
 * Remark:   No dependency. Handy
 * Linkedin: http://nl.linkedin.com/in/jerysun
 * Website:  https://sites.google.com/site/geekssmallworld
 * Github:   https://github.com/jerysun/
 */

#include "stdafx.h"
#include "AthleteOperations.h"

// add athlete objects
void AddAthlete(std::unique_ptr<std::unordered_multimap<int, st_ath>> atls) {
  Athlete *athlete;
  AthleteId id = 0;

  for (std::unordered_multimap<int, st_ath>::iterator it = atls->begin();
    it != atls->end(); ++it) {
    athlete = Athlete::Get(++id);
    if (athlete->ObjectExists()) {
      /*
      Athlete::Header();
      std::cout << *athlete << std::endl;
      std::cerr << "This Athlete is already on datastore\n";
      */
    } else {
      athlete->InputANo(it->first);
      athlete->Input(std::move(it->second));
      if (!athlete->AddObject()) {
        std::cerr << "Add disallowed\n";
      }
    }
    delete athlete;
  }
}

static AthleteId GetAno() {
  AthleteId ano;
  std::cout << "Please input the Athlete Number:\n";
  std::cin >> ano;
  return ano;
}

static std::string GetName() {
  std::string name;
  std::cout << "Please input the new name:\n";
  std::cin >> name;
  return name;
}

static unsigned short GetRecord() {
  unsigned short rcd;
  std::cout << "Please input the Athlete Number:\n";
  std::cin >> rcd;
  return rcd;
}

static std::string GetRDate() {
  std::string rdt;
  std::cout << "Please input the new name:\n";
  std::cin >> rdt;
  return rdt;
}

// change an existing athlete object
void ChangeAthlete(AthleteId aid, Athlete::fields f) {
  Athlete *athlete = Athlete::GetExisting(aid);
  if (athlete) {
    switch (f) {
    case Athlete::ANO:
      Athlete::ChangeAno(GetAno());
      break;
    case Athlete::NAME:
      Athlete::ChangeName(std::move(GetName()));
      break;
    case Athlete::RECORD:
      Athlete::ChangeRecord(GetRecord());
      break;
    case Athlete::RDATE:
      Athlete::ChangeRDate(std::move(GetRDate()));
      break;
    default:
      break;
    }
    delete athlete;
  }
}

// delete an existing athlete object
void DeleteAthlete(AthleteId aid) {
  Athlete *athlete = Athlete::GetExisting(aid);
  if (athlete) {
    if (!athlete->DeleteObject()) {
      std::cerr << "Delete disallowed\n";
    }
    delete athlete;
  }
}

void ListAthletes() {
  Athlete athl;
  athl.FirstObject();
  Athlete::Header();
  while (athl.ObjectExists()) {
    std::cout << athl << std::endl;
    athl.NextObject();
  }
}
