/*
 * filename: dst_util.cpp
 * describe: Frequently used routines for string, time operations
 * Author:   Jerry Sun <jerysun0818@gmail.com>
 * Date:     Octobler 1, 2017
 * Remark:   No dependency. Handy
 * Linkedin: http://nl.linkedin.com/in/jerysun
 * Website:  https://sites.google.com/site/geekssmallworld
 * Github:   https://github.com/jerysun/
 */
#include "stdafx.h"
#include <iomanip>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <string>
#include <locale>

#include "dst_util.h"

/*
  01. Time string convertion
 */
std::time_t get_epoch_time_w(const std::wstring& dateTime) {
  // Let's consider we are getting all the input in
  // this format: '2014-07-25T20:17:22Z' (T denotes
  // start of Time part, Z denotes UTC zone).
  // A better approach would be to pass in the format as well.
  static const std::wstring dateTimeFormat{ L"%Y-%m-%dT%H:%M:%SZ" };

  std::wistringstream ss{ dateTime };
  std::tm dt;
  ss >> std::get_time(&dt, dateTimeFormat.c_str());

  return std::mktime(&dt);
}

std::time_t string_to_time_t(const std::string& dateTime) {
  static const std::string dateTimeFormat{"%Y-%m-%d"};
  std::istringstream ss{dateTime};
  std::tm dt;

  ss >> std::get_time(&dt, dateTimeFormat.c_str());
  return std::mktime(&dt);
}

std::string time_t_to_string(const std::time_t &tmt) {
  std::tm *ltm = localtime(&tmt);
  std::ostringstream oss;

  oss << ltm->tm_year + 1900 << "-"
      << ltm->tm_mon  + 1 << "-"
      << ltm->tm_mday;

  return oss.str();
}


/*
 * 02. Trim family
 */
std::string trimLeft(const std::string &s) {
  auto temp = s;
  temp.erase(std::begin(temp), 
             std::find_if(std::begin(temp), std::end(temp), 
             [](char c){return !std::isspace(c, std::locale()); 
  }));
  return temp;
}

std::string trimRight(const std::string &s) {
  auto temp = s;
  temp.erase(std::find_if(std::rbegin(temp), std::rend(temp), 
             [](char c){return !std::isspace(c, std::locale()); }).base(), 
  std::end(temp));
  return temp;
}

std::string trim(const std::string &s) {
  return trimLeft(trimRight(s));
}

