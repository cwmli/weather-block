#ifndef APIDATA_H
#define APIDATA_H

#include <time.h>

struct APIData {
  String name;
  String url;
  long refreshTime;
  long lastRefreshed;

  APIData() {}

  APIData(String name, String url, long time) : name(name), url(url), refreshTime(time) {
    lastRefreshed = 0;
  }
};

#endif
