#ifndef APIDATA_H
#define APIDATA_H

#include <map>
#include <time.h>

#include "APIParseRule.h"

struct APIData {
  String name;
  String url;
  bool isActive;
  unsigned long refreshTime;
  unsigned long lastRefreshed;

  std::map<String, String> data;
  std::map<String, APIParseRule> parseRules;


  APIData() {}

  APIData(String name, String url, long time) : name(name), url(url), refreshTime(time) {
    lastRefreshed = 0;
  }
};

#endif
