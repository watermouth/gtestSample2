#include "gtest/gtest.h"

#include <iostream>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include "utility.h"

using namespace std;

class BankAccount {
public:
  BankAccount() : account(0) { }
  void Deposit(int x) { account += x; }
  int WithDraw(int x) { account -= x; return account;}
private:
  int account;
};

void bankAgent(BankAccount &a) {
  vector<int> account;
  account.reserve(10);
  for (int i=0; i!=10; ++i) {
    a.Deposit(500);
  }
}

void withDraw(BankAccount &a) {
  vector<int> account;
  account.reserve(10);
  for (int i=0; i!=10; ++i) {
    cout << a.WithDraw(100) << " ";
  }
  cout << endl; 
  //PRINT_ELEMENTS(account, "account: "); 
}

class SynchronizationTest : public testing::Test {
protected:

};

TEST_F(SynchronizationTest, NoLocking) {
  BankAccount joes;
  boost::thread th1( 
    boost::bind(bankAgent, joes)
  );
  boost::thread th2( 
    boost::bind(withDraw, joes)
  );
  th1.join();
  th2.join();
} 
