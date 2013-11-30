#include "gtest/gtest.h"

#include <iostream>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/signals2.hpp>
#include "utility.h"

using namespace std;

class BankAccountInternalLock {
public:
  BankAccountInternalLock() : balance(0) { }
  void Deposit(int x) {
    boost::lock_guard<boost::mutex> guard(mtx_);
    //mtx_.lock();
    // mutexの効果を見るために, このスレッドをsleepさせる.
    int temp = balance;
    temp += x;
    boost::this_thread::sleep_for(boost::chrono::milliseconds(1)); 
    balance = temp;
    //mtx_.unlock();
  }
  void WithDraw(int x) {
    boost::lock_guard<boost::mutex> guard(mtx_);
    //mtx_.lock();
    // mutexの効果を見るために, このスレッドをsleepさせる.
    int temp = balance;
    temp -= x;
    boost::this_thread::sleep_for(boost::chrono::milliseconds(1)); 
    balance = temp;
    //mtx_.unlock();
  }
  int GetBalance() {
    boost::lock_guard<boost::mutex> guard(mtx_);
    //mtx_.lock();
    int temp = balance;
    //mtx_.unlock();
    return temp;
  }
private:
  int balance;
  boost::mutex mtx_;
};

class BankAccountLockable {
  boost::mutex mtx_; 
  int balance;
public:
  BankAccountLockable() : balance(0) { }
  void Deposit(int x) {
    int temp = balance;
    temp += x;
    boost::this_thread::sleep_for(boost::chrono::milliseconds(1)); 
    balance = temp;
  }
  void WithDraw(int x) {
    int temp = balance;
    temp -= x;
    boost::this_thread::sleep_for(boost::chrono::milliseconds(1)); 
    balance = temp;
  }
  int GetBalance() {
    int temp = balance;
    return temp;
  }
  void Lock() { mtx_.lock(); }
  void Unlock() { mtx_.unlock(); }
};

template <class T>
void bankAgent(T &a) {
  for (int i=0; i!=50; ++i) {
    a.Deposit(500);
  }
}

void bankAgentWithLock(BankAccountLockable &a) {
  for (int i=0; i!=50; ++i) {
    a.Lock();
    a.Deposit(500);
    a.Unlock();
  }
}

template <class T>
void withDraw(T &a) {
  for (int i=0; i!=50; ++i) {
    a.WithDraw(100);
    cout << a.GetBalance() << " ";
  }
  cout << endl; 
  //PRINT_ELEMENTS(account, "account: "); 
}

void withDrawWithLock(BankAccountLockable &a) {
  for (int i=0; i!=50; ++i) {
    a.Lock();
    a.WithDraw(100);
    cout << a.GetBalance() << " ";
    a.Unlock();
  }
  cout << endl; 
}

class SynchronizationTest : public testing::Test {
protected:

};

/// Without locking, you'll get wrong number.
TEST_F(SynchronizationTest, NoLocking) {
  BankAccountLockable joes;
  boost::function<void(void)> func1 = 
    boost::bind(bankAgent<BankAccountLockable>, boost::ref(joes));
  boost::function<void(void)> func2 = 
    boost::bind(withDraw<BankAccountLockable>, boost::ref(joes));
  boost::thread th1(func1);
  boost::thread th2(func2);
  th1.join();
  th2.join();
  cout << "Balance: " << joes.GetBalance() << endl;
  EXPECT_NE(20000, joes.GetBalance());
} 

/// With internal locking, you'll get correct number.
TEST_F(SynchronizationTest, InternalLocking) {
  BankAccountInternalLock joes;
  boost::function<void(void)> func1 = 
    boost::bind(bankAgent<BankAccountInternalLock>, boost::ref(joes));
  boost::function<void(void)> func2 = 
    boost::bind(withDraw<BankAccountInternalLock>, boost::ref(joes));
  boost::thread th1(func1);
  boost::thread th2(func2);
  th1.join();
  th2.join();
  cout << "Balance: " << joes.GetBalance() << endl;
  EXPECT_EQ(20000, joes.GetBalance());
}

/// With external locking, you'll get correct number.
TEST_F(SynchronizationTest, ExternalLocking) {
  BankAccountLockable joes;
  boost::function<void(void)> func1 = 
    boost::bind(bankAgentWithLock, boost::ref(joes));
  boost::function<void(void)> func2 = 
    boost::bind(withDrawWithLock, boost::ref(joes));
  boost::thread th1(func1);
  boost::thread th2(func2);
  th1.join();
  th2.join();
  cout << "Balance: " << joes.GetBalance() << endl;
  EXPECT_EQ(20000, joes.GetBalance());
} 
