#ifndef APIDATA_H
#define APIDATA_H

#include <map>
#include <time.h>

struct APIData {
  String name;
  String url;
  bool isActive;
  unsigned long refreshTime;
  unsigned long lastRefreshed;

  std::map<String, String> data;
  // parseRules: 
  // <Key, Value>
  // value: array of size 4, where [lengthOfValue, xPos, yPos, isNumber]
  std::map<String, std::array<int, 4>> parseRules;


  APIData() {}

  APIData(String name, String url, long time) : name(name), url(url), refreshTime(time) {
    lastRefreshed = 0;
  }
};

#endif
