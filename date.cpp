/*
 * filename: date.cpp
 * describe: This is the implementation of class Date, a helper 
 *           used by my open source project EDS (Embedded Data
 *           Store) for the normalization of date operations
 * Author:   Jerry Sun <jerysun0818@gmail.com>
 * Date:     November 08, 2007
 * Remark:   No dependency. Handy
 * Linkedin: http://nl.linkedin.com/in/jerysun
 * Website:  https://sites.google.com/site/geekssmallworld
 * Github:   https://github.com/jerysun/
 */
 
#include "stdafx.h"
#include "date.h"

// --- overloaded relational less-than
int Date::operator< (const Date &dt) const {
	if (yr < dt.yr)
		return 1;
	if (yr == dt.yr) {
		if (mo < dt.mo)
			return 1;
		if (mo == dt.mo)
			return da < dt.da;
	}
	return 0;
}

// --- ostream insertion operator
std::ostream& operator<< (std::ostream &os, const Date &dt) {
	os.setf(std::ios::right);
	os.fill(' ');
	os.width(2);
	os << static_cast<unsigned int>(dt.mo) << '/';
	os.fill('0');
	os.width(2);
	os << static_cast<unsigned int>(dt.da) << '/';
	os.fill('0');
	os.width(4);
	os << dt.yr;
	return os;
}
