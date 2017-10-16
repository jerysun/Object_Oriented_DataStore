/*
 * filename: AthleteOperations.h
 * describe: This is the header file of the operations of class Athlete,
 *           an "object" talbe, which is a part of the example application
 *           for my open source project - EDS (Embedded Data Store)
 * Author:   Jerry Sun <jerysun0818@gmail.com>
 * Date:     Octobler 15, 2017
 * Remark:   No dependency. Handy
 * Linkedin: http://nl.linkedin.com/in/jerysun
 * Website:  https://sites.google.com/site/geekssmallworld
 * Github:   https://github.com/jerysun/
 */

// AthleteOperations
#ifndef ATHLETEOPERATIONS_H
#define ATHLETEOPERATIONS_H

#include <vector>
#include "Athlete.h"

void AddAthlete(std::unique_ptr<std::unordered_multimap<int, st_ath>> atls);
void ChangeAthlete(AthleteId aid, Athlete::fields f);
void DeleteAthlete(AthleteId aid);
void ListAthletes();

#endif //ATHLETEOPERATIONS_H
