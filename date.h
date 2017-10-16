/*
 * filename: date.h
 * describe: This is the definition file of class Date, a helper 
 *           used by my open source project EDS (Embedded Data
 *           Store) for the normalization of date operations
 * Author:   Jerry Sun <jerysun0818@gmail.com>
 * Date:     November 08, 2007
 * Remark:   No dependency. Handy
 * Linkedin: http://nl.linkedin.com/in/jerysun
 * Website:  https://sites.google.com/site/geekssmallworld
 * Github:   https://github.com/jerysun/
 */
 
#ifndef DATE_H
#define DATE_H

#include <iostream>
#include <iomanip>

typedef unsigned char DtEl;

class Date {
public:
	Date(DtEl m = 0, DtEl d = 0, unsigned int y = 0) : mo(m), da(d), yr(y) {}

	DtEl Month() const { return mo; }
	DtEl Day() const { return da; }
	unsigned int Year() const { return yr; }
	
	void SetMonth(DtEl m) { mo = m; }
	void SetDay(DtEl d) { da = d; }
	void SetYear(unsigned int y) { yr = y; }

	int operator< (const Date &dt) const;
	int operator== (const Date &dt) const {
		return mo == dt.mo && da == dt.da && yr == dt.yr;
	}
	int operator!= (const Date &dt) const { return !(*this == dt); }
	int operator>  (const Date &dt) const { return !(*this == dt || *this < dt); }
	int operator<= (const Date &dt) const { return (*this == dt || *this < dt); }
	int operator>= (const Date &dt) const { return (*this == dt || *this > dt); }

	friend std::ostream& operator<< (std::ostream &os, const Date &dt);

private:
	DtEl mo, da;
	unsigned int yr;
};

#endif /* DATE_H*/