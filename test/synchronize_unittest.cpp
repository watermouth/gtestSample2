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
    // mutex$B$N8z2L$r8+$k$?$a$K(B, $B$3$N%9%l%C%I$r(Bsleep$B$5$;$k(B.
    int temp = balance;
    temp += x;
    boost::this_thread::sleep_for(boost::chrono::milliseconds(1)); 
    balance = temp;
    //mtx_.unlock();
  }
  void WithDraw(int x) {
    boost::lock_guard<boost::mutex> guard(mtx_);
    //mtx_.lock();
    // mutex$B$N8z2L$r8+$k$?$a$K(B, $B$3$N%9%l%C%I$r(Bsleep$B$5$;$k(B.
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

TEST_F(SynchronizationTest, ExternalLocking) {
  BankAccountLockable joes;
/* $B%@%a(B
  boost::function<void(void)> func_bank_agent = 
    boost::bind(bankAgent<BankAccountLockable>, boost::ref(joes));
  boost::function<void(void)> func_withdraw  = 
    boost::bind(withDraw<BankAccountLockable>, boost::ref(joes));
  boost::function<void(void)> func_lock = 
    boost::bind(&BankAccountLockable::Lock, boost::ref(joes));
  boost::function<void(void)> func_unlock = 
    boost::bind(&BankAccountLockable::Unlock, boost::ref(joes));
  // Lock(), Unlock()$B$r$^$H$a$?4X?t$r:n@.$9$k(B
  boost::signals2::signal<void(void)> sig1, sig2;
  // $B4JC1$N$?$a0l%9%l%C%IC10L$G$^$H$a$F$7$^$&!J(BInternalLocking$B$NNc$HHf$Y$k$H(Block$BC10L$,Bg$-$$!K(B 
  sig1.connect(func_lock);
  sig1.connect(func_bank_agent);
  sig1.connect(func_unlock);
  sig2.connect(func_lock);
  sig2.connect(func_withdraw);
  sig2.connect(func_unlock);
  boost::thread th1(sig1);
  boost::thread th2(sig2);
*/
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
