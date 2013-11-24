#include "gtest/gtest.h"

#include <iostream>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

using namespace std;

void print_hello() {
  for (size_t i=0; i<10; ++i) {
    cout << "Hello, " << endl; 
  }
}

void print_world() {
  for (size_t i=0; i<10; ++i) {
    cout << "World." << endl; 
  }
}

void print_input(const string &s) {
  for (size_t i=0; i<10; ++i) {
    cout << s << endl; 
  }
}

TEST(ThreadTest, ThreadAndJoin) { 
  const string s = "hoge";
  boost::thread th1(print_hello);
  boost::thread th2(print_world);
  boost::thread th3(boost::bind(print_input, "fufa"));
  th3.join();
  th1.join();
  th2.join();
}

