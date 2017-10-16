/*
 * filename: Embedded_Datastore.cpp
 * describe: This is an example application using the open source
 *           project - EDS (Embedded Data Store)
 * Author:   Jerry Sun <jerysun0818@gmail.com>
 * Date:     Octobler 15, 2017
 * Remark:   No dependency. Handy
 * Linkedin: http://nl.linkedin.com/in/jerysun
 * Website:  https://sites.google.com/site/geekssmallworld
 * Github:   https://github.com/jerysun/
 */

#include "stdafx.h"
#include "edatastore.h"
#include "AthleteOperations.h"

int main() {
  std::unique_ptr<std::unordered_multimap<int, st_ath>>
    athls{ std::make_unique<std::unordered_multimap<int, st_ath>>() };
  
  athls->insert(std::make_pair<int, st_ath>(2, { "John Wayne", 79, "2010-07-08" }));
  athls->insert(std::make_pair<int, st_ath>(7, { "Ronald Regan", 51, "2000-03-22" }));
  athls->insert(std::make_pair<int, st_ath>(1, { "Ford Harrison", 85, "2009-11-13" }));
  athls->insert(std::make_pair<int, st_ath>(2, { "John Wayne", 69, "2017-01-01" }));
  athls->insert(std::make_pair<int, st_ath>(7, { "Ronald Regan", 97, "2005-05-20" }));
  athls->insert(std::make_pair<int, st_ath>(3, { "Demi Lovato", 33, "2002-04-28" }));
  athls->insert(std::make_pair<int, st_ath>(1, { "Ford Harrison", 89, "2001-09-03" }));
  athls->insert(std::make_pair<int, st_ath>(5, { "Anna Paquin", 79, "2010-07-07" }));
  athls->insert(std::make_pair<int, st_ath>(7, { "Ronald Regan", 39, "2010-07-08" }));
  athls->insert(std::make_pair<int, st_ath>(4, { "Kate Winslet", 97, "2010-07-08" }));
  athls->insert(std::make_pair<int, st_ath>(3, { "Demi Lovato", 49, "2010-07-08" }));
  athls->insert(std::make_pair<int, st_ath>(6, { "Monica Bellucci", 59, "2010-07-08" }));
  athls->insert(std::make_pair<int, st_ath>(5, { "Anna Paquin", 95, "2010-07-08" }));
  athls->insert(std::make_pair<int, st_ath>(5, { "Anna Paquin", 37, "2010-07-08" }));
  athls->insert(std::make_pair<int, st_ath>(5, { "Anna Paquin", 89, "2010-05-11" }));
  athls->insert(std::make_pair<int, st_ath>(1, { "Ford Harrison", 59, "2010-07-08" }));
  athls->insert(std::make_pair<int, st_ath>(4, { "Kate Winslet", 56, "2010-07-08" }));
  athls->insert(std::make_pair<int, st_ath>(5, { "Anna Paquin", 89, "2010-04-17" }));
  athls->insert(std::make_pair<int, st_ath>(7, { "Ronald Regan", 74, "2010-07-08" }));
  athls->insert(std::make_pair<int, st_ath>(4, { "Kate Winslet", 51, "2010-07-08" }));
  athls->insert(std::make_pair<int, st_ath>(7, { "Ronald Regan", 91, "2010-07-08" }));
  athls->insert(std::make_pair<int, st_ath>(6, { "Monica Bellucci", 92, "2006-02-08" }));
  athls->insert(std::make_pair<int, st_ath>(2, { "John Wayne", 73, "2003-05-03" }));
  athls->insert(std::make_pair<int, st_ath>(3, { "Demi Lovato", 59, "2008-08-18" }));
  athls->insert(std::make_pair<int, st_ath>(6, { "Monica Bellucci", 84, "2001-03-12" }));

  athls->insert(std::make_pair<int, st_ath>(2, { "John Wayne", 11, "2010-07-08" }));
  athls->insert(std::make_pair<int, st_ath>(7, { "Ronald Regan", 11, "2000-03-22" }));
  athls->insert(std::make_pair<int, st_ath>(1, { "Ford Harrison", 11, "2009-11-13" }));
  athls->insert(std::make_pair<int, st_ath>(2, { "John Wayne", 13, "2017-01-01" }));
  athls->insert(std::make_pair<int, st_ath>(7, { "Ronald Regan", 13, "2005-05-20" }));
  athls->insert(std::make_pair<int, st_ath>(3, { "Demi Lovato", 11, "2002-04-28" }));
  athls->insert(std::make_pair<int, st_ath>(1, { "Ford Harrison", 13, "2001-09-03" }));
  athls->insert(std::make_pair<int, st_ath>(5, { "Anna Paquin", 11, "2010-07-07" }));
  athls->insert(std::make_pair<int, st_ath>(7, { "Ronald Regan", 15, "2010-07-08" }));
  athls->insert(std::make_pair<int, st_ath>(4, { "Kate Winslet", 11, "2010-07-08" }));
  athls->insert(std::make_pair<int, st_ath>(3, { "Demi Lovato", 13, "2010-07-08" }));
  athls->insert(std::make_pair<int, st_ath>(6, { "Monica Bellucci", 11, "2010-07-08" }));
  athls->insert(std::make_pair<int, st_ath>(5, { "Anna Paquin", 13, "2010-07-08" }));
  athls->insert(std::make_pair<int, st_ath>(5, { "Anna Paquin", 15, "2010-07-08" }));
  athls->insert(std::make_pair<int, st_ath>(5, { "Anna Paquin", 17, "2010-05-11" }));
  athls->insert(std::make_pair<int, st_ath>(1, { "Ford Harrison", 15, "2010-07-08" }));
  athls->insert(std::make_pair<int, st_ath>(4, { "Kate Winslet", 13, "2010-07-08" }));
  athls->insert(std::make_pair<int, st_ath>(5, { "Anna Paquin", 19, "2010-04-17" }));
  athls->insert(std::make_pair<int, st_ath>(7, { "Ronald Regan", 17, "2010-07-08" }));
  athls->insert(std::make_pair<int, st_ath>(4, { "Kate Winslet", 15, "2010-07-08" }));
  athls->insert(std::make_pair<int, st_ath>(7, { "Ronald Regan", 19, "2010-07-08" }));
  athls->insert(std::make_pair<int, st_ath>(6, { "Monica Bellucci", 13, "2006-02-08" }));
  athls->insert(std::make_pair<int, st_ath>(2, { "John Wayne", 15, "2003-05-03" }));
  athls->insert(std::make_pair<int, st_ath>(3, { "Demi Lovato", 15, "2008-08-18" }));
  athls->insert(std::make_pair<int, st_ath>(6, { "Monica Bellucci", 15, "2001-03-12" }));

  athls->insert(std::make_pair<int, st_ath>(2, { "John Wayne", 10, "2010-07-08" }));
  athls->insert(std::make_pair<int, st_ath>(7, { "Ronald Regan", 10, "2000-03-22" }));
  athls->insert(std::make_pair<int, st_ath>(1, { "Ford Harrison", 10, "2009-11-13" }));
  athls->insert(std::make_pair<int, st_ath>(2, { "John Wayne", 12, "2017-01-01" }));
  athls->insert(std::make_pair<int, st_ath>(7, { "Ronald Regan", 12, "2005-05-20" }));
  athls->insert(std::make_pair<int, st_ath>(3, { "Demi Lovato", 10, "2002-04-28" }));
  athls->insert(std::make_pair<int, st_ath>(1, { "Ford Harrison", 12, "2001-09-03" }));
  athls->insert(std::make_pair<int, st_ath>(5, { "Anna Paquin", 10, "2010-07-07" }));
  athls->insert(std::make_pair<int, st_ath>(7, { "Ronald Regan", 14, "2010-07-08" }));
  athls->insert(std::make_pair<int, st_ath>(4, { "Kate Winslet", 10, "2010-07-08" }));
  athls->insert(std::make_pair<int, st_ath>(3, { "Demi Lovato", 12, "2010-07-08" }));
  athls->insert(std::make_pair<int, st_ath>(6, { "Monica Bellucci", 10, "2010-07-08" }));
  athls->insert(std::make_pair<int, st_ath>(5, { "Anna Paquin", 12, "2010-07-08" }));
  athls->insert(std::make_pair<int, st_ath>(5, { "Anna Paquin", 14, "2010-07-08" }));
  athls->insert(std::make_pair<int, st_ath>(5, { "Anna Paquin", 16, "2010-05-11" }));
  athls->insert(std::make_pair<int, st_ath>(1, { "Ford Harrison", 14, "2010-07-08" }));
  athls->insert(std::make_pair<int, st_ath>(4, { "Kate Winslet", 12, "2010-07-08" }));
  athls->insert(std::make_pair<int, st_ath>(5, { "Anna Paquin", 18, "2010-04-17" }));
  athls->insert(std::make_pair<int, st_ath>(7, { "Ronald Regan", 16, "2010-07-08" }));
  athls->insert(std::make_pair<int, st_ath>(4, { "Kate Winslet", 14, "2010-07-08" }));
  athls->insert(std::make_pair<int, st_ath>(7, { "Ronald Regan", 18, "2010-07-08" }));
  athls->insert(std::make_pair<int, st_ath>(6, { "Monica Bellucci", 12, "2006-02-08" }));
  athls->insert(std::make_pair<int, st_ath>(2, { "John Wayne", 14, "2003-05-03" }));
  athls->insert(std::make_pair<int, st_ath>(3, { "Demi Lovato", 14, "2008-08-18" }));
  athls->insert(std::make_pair<int, st_ath>(6, { "Monica Bellucci", 14, "2001-03-12" }));

  std::unique_ptr<EDatastore> sport(std::make_unique<EDatastore>("SPORT"));
  {
    Athlete athl;
  }

  AddAthlete(std::move(athls));
  ListAthletes();
}