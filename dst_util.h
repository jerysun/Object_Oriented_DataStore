/*
 * filename: dst_util.h
 * describe: Frequently used routines for string, time operations
 * Author:   Jerry Sun <jerysun0818@gmail.com>
 * Date:     Octobler 1, 2017
 * Remark:   No dependency. Handy
 * Linkedin: http://nl.linkedin.com/in/jerysun
 * Website:  https://sites.google.com/site/geekssmallworld
 * Github:   https://github.com/jerysun/
 */

#ifndef DST_UTIL_H
#define DST_UTIL_H

#include <ctime>

/*
 * 01. Time string convertion
 */
std::time_t get_epoch_time_w(const std::wstring& dateTime);
std::string time_t_to_string(const std::time_t & tmt);
std::time_t string_to_time_t(const std::string& dateTime);

/*
 * 02. Trim family
 */
std::string trim(const std::string &s);
std::string trimLeft(const std::string &s);
std::string trimRight(const std::string &s);

#endif //DST_UTIL_H
