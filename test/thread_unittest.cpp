#include "gtest/gtest.h"

#include <iostream>
#include <vector>
#include <numeric>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/bind/apply.hpp>
#include <boost/lambda/lambda.hpp>
#include <algorithm>
#include <functional>
#include "utility.h"

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

struct AddOne {
  int operator()() {
    return i++;
  }
  AddOne(int init) : i(init) {}
private:
  int i;
};

TEST(ThreadTest, ThreadAndJoin) { 
  const string s = "hoge";
  boost::thread th1(print_hello);
  boost::thread th2(print_world);
  boost::thread th3(boost::bind(print_input, "fufa"));
  th3.join();
  th1.join();
  th2.join();
}

/// Futuresの使用例
/// vectorに詰めた値の和を計算する.
TEST(ThreadTest, Futures) {
  size_t size = 10000;
  vector<int> coll(size,0);
  generate_n(coll.begin(), size, AddOne(1));
  EXPECT_EQ(size, coll.size());
  /// Single Threadでの比較用結果取得
  int correnct_answer = accumulate(coll.begin(),coll.end(),0); 
  cout << "answer: " << correnct_answer << endl;  
  typedef int (*AcumInt)(vector<int>::iterator, vector<int>::iterator, int) ;
  boost::function<int(void)> func 
    = boost::bind(static_cast<AcumInt>(accumulate), coll.begin(), coll.end(), 0); 
  cout << "used AcumInt: " << func() << endl;
  /// Futuresの使用例: 1スレッドで計算した結果を受け取る
  boost::packaged_task<int> pt(func);
  boost::unique_future<int> ft = pt.get_future();
  boost::thread th(pt.move());
  cout << "used pakged_task and unique_future: " << ft.get() << endl;
  /// 2つのスレッドに分割して計算させた結果を足し合わせる 
  boost::function<int(void)> front_half 
    = boost::bind(static_cast<AcumInt>(accumulate), coll.begin(), coll.begin() + size / 2, 0); 
  boost::function<int(void)> back_half 
    = boost::bind(static_cast<AcumInt>(accumulate), coll.begin() + size / 2, coll.end(), 0); 
  boost::packaged_task<int> pt1(front_half), pt2(back_half);
  boost::unique_future<int> ft1 = pt1.get_future();
  boost::unique_future<int> ft2 = pt2.get_future();
  boost::thread th1(pt1.move()), th2(pt2.move());
  cout << "th1: " << ft1.get() << ", th2: " << ft2.get() << endl;
  EXPECT_EQ(correnct_answer, ft1.get() + ft2.get());
}

