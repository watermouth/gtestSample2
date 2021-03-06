#include "gtest/gtest.h"

#include <vector>
#include <algorithm>
#include <numeric>
#include <functional>
#include <iterator>
#include <iostream>
#include <boost/assign.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/mem_fn.hpp>
#include "utility.h"

using namespace std;

template <class T>
T ReturnSelf(const T &x) { return x;}

struct AddOne {
  AddOne(int i) : v_(i) {}
  int operator()() { return v_++; }
private:
  int v_;
};

class FunctorsTest : public testing::Test {
protected:
  void SetUp() { }
  void TearDown() { }
  
  struct ReturnOne {
    int operator() () { return 1; }
  };

  struct IntSequence {
    IntSequence(int i) : value_(i) { }
    int operator() () { return ++value_; }
  private:
    int value_;
  };

  template <class T>
  struct PrintValue {
    void operator() (const T &x) { cout << x << endl; } 
  };

  /// template function
  template <class T, class BinaryOperator>
  bool Compare(const T&left, const T&right, BinaryOperator bo) {
    return bo(left, right);
  } 

  /// $B30It$+$i(BPredicate$B$r@_Dj2DG=$J%a%s%P4X?t$r;}$D(Bclass
  struct HogeWithCompare {
    template <class T, class BinaryOperator>
    bool comp(const T&left, const T&right, BinaryOperator bo) {
      return bo(left, right);
    } 
  };

  /// $B30It$+$i(BPredicate$B$r@_Dj2DG=$J%a%s%P4X?t$r;}$D(Btemplate class
  template <class T>
  struct TemplateHogeWithCompare {
    template <class BinaryOperator>
    bool comp(const T&left, const T&right, BinaryOperator bo) {
      return bo(left.GetValue(), right.GetValue());
    } 
    // default 
    bool comp(const T&left, const T&right) { 
      return comp(left, right, std::less<int>());
    } 
  };

  /// GetValue$B$r;}$D%/%i%9(B
  class FugaWithGetValue {
  public:
    explicit FugaWithGetValue(int v) : value_(v) { }
    int GetValue() const { return value_; }
  private:
    int value_;
  };

public:
  template <class T>
  T MemReturnSelf(T x) { return x;}
   
};

/// $B=`Hw(B:$B4JC1$J(Bfuntor$B$H(Bgenerate_n $B$H(B PRINT_ELEMENTS
TEST_F(FunctorsTest, Generate_n){
  typedef vector<double>::iterator VIT; 
  vector<double> x;
  generate_n(back_inserter(x), 10, ReturnOne()); 
  //for_each(x.begin(), x.end(), PrintValue<double>()); 
  PRINT_ELEMENTS(x);  

  generate_n(back_inserter(x), 10, IntSequence(42)); 
  //for_each(x.begin() + 10, x.end(), PrintValue<double>()); 
  PRINT_ELEMENTS(x);

  IntSequence seq(100);

  // pass by value
  generate_n(back_inserter(x), 5, seq);
  PRINT_ELEMENTS(x);

  // pass by reference 
  // qualify explicitly template arguments
  generate_n<back_insert_iterator<vector<double> >
            , int, IntSequence&>
  (back_inserter(x), 5, seq);
  PRINT_ELEMENTS(x);  

  // pass by value again
  generate_n(back_inserter(x), 5, seq);
  PRINT_ELEMENTS(x);  

  // pass by value again
  generate_n(back_inserter(x), 5, seq);
  PRINT_ELEMENTS(x);  
}

TEST_F(FunctorsTest, SetPredicate){
  EXPECT_EQ(true, Compare(1, 3, std::less<int>()));

  HogeWithCompare hogeObj;
  EXPECT_EQ(true, hogeObj.comp<int>(12, 10, std::greater<int>()));
  EXPECT_EQ(false, hogeObj.comp<int>(1.2, 1.0, std::greater<int>()));
  EXPECT_EQ(true, hogeObj.comp<double>(1.2, 1.0, std::greater<double>()));
  EXPECT_EQ(true, hogeObj.comp<std::string>("fa8", "aa", std::greater<std::string>()));

  TemplateHogeWithCompare<FugaWithGetValue> templateHogeObj;
  FugaWithGetValue fugaObj1(1);
  FugaWithGetValue fugaObj2(10);
  EXPECT_EQ(true
    ,templateHogeObj.comp(fugaObj1, fugaObj2, std::less<int>())); 
  EXPECT_EQ(true
    ,templateHogeObj.comp(fugaObj1, fugaObj2));
}

TEST_F(FunctorsTest, count_if) {
  vector<double> x;
  using namespace boost::assign;
  x += 1, 2, 3, 4, 5, 6, 3, 12, 13, 8, 2;
  // 10$B0J>e$NCM$N8D?t$r?t$($k(B 
  EXPECT_EQ(2,
    count_if(x.begin(), x.end(),
             bind2nd(greater_equal<double>(), 10)
    )
  );
  // boost::bind 
  EXPECT_EQ(2,
    count_if(x.begin(), x.end(),
             boost::bind(greater_equal<double>(), _1, 10)
    )
  );
  EXPECT_EQ(2,
    count_if(x.begin(), x.end(),
             boost::bind(less<double>(), 10, _1)
    )
  );
  // $B$3$s$J=q$-J}$b$G$-$k(B
  EXPECT_EQ(2,
    count_if(x.begin(), x.end(),
             boost::bind(greater_equal<double>(), _1, 10) == true
    )
  );
}

TEST_F(FunctorsTest, transform){
  // transform$B$NN}=,(B: $B4X?t%*%V%8%'%/%H$r$I$N$h$&$KMQ0U$9$k$+(B
  vector<double> x;
  {
    using namespace boost::assign;
    x += 1, 2, 3, 4, 5, 6, 3, 12, 13, 8, 2;
  }
  vector<double> y; 

  PRINT_ELEMENTS(x, "elements: ");
  // $BMWAGCj=P(B($B<+J,<+?H$rJV$9(B)
  transform(x.begin(), x.end()// source
    , back_inserter(y)//destination: $B0l2sL\$J$N$G(Bback_inserter
     , ReturnSelf<double>); // OK: global function$B$G$"$k!#%a%s%P4X?t$G$O$J$$!#(B
  PRINT_ELEMENTS(y, "returned: ");

  // $BMWAGCj=P(B($B<+J,<+?H$rJV$9(B)
  // boost::function$B$r;H$C$F$_$k(B
  boost::function<double(double) > fObj = ReturnSelf<double>;
  transform(x.begin(), x.end()// source
    , (y.begin())//destination
    , fObj);
  PRINT_ELEMENTS(y, "returned: ");

  // $BMWAGCj=P(B($B<+J,<+?H$rJV$9(B)
  // $B$3$N%F%9%H%/%i%9$N%a%s%P4X?t$r;H$C$F$_$k(B 
  fObj = boost::bind(
      boost::mem_fn(&FunctorsTest::MemReturnSelf<double>)
    , this, _1);  
  transform(x.begin(), x.end()// source
    , (y.begin())//destination
    , fObj);
  PRINT_ELEMENTS(y, "returned: ");
  // $B$^$?$O(Bbind1st$B$r;H$&$H(B
  fObj = bind1st( 
      boost::mem_fn(&FunctorsTest::MemReturnSelf<double>)
    , this);  
  transform(x.begin(), x.end()// source
    , (y.begin())//destination
    , fObj);
  PRINT_ELEMENTS(y, "returned: ");
}

/// Boost::function$B$+$i(Bfunction$B$r:n$l$k$+!)(B
TEST_F(FunctorsTest, boostFunction) {
  // $B4X?t%*%V%8%'%/%H$NBeF~(B
  boost::function<int(void)> f
    = FunctorsTest::ReturnOne();
  // $BF0:n3NG'(B
  PrintValue<int>()((f()));
  
  // boost::function$B$NBeF~(Bboost::function$B$N:n@.(B
  boost::function<int(void)> g
    = f;
  // $BF0:n3NG'(B
  PrintValue<int>()((g()));

  // boost::function$B$rMQ$$$?4X?t9g@.(B
  // $B$H$$$&$h$j$b(B, bind$B$r4X?t%*%V%8%'%/%H$KE,MQ$9$kJ}K!(B 
  // $BLa$jCM$r;XDj$9$kI,MW$,$"$k(B.
  // $B$b$7$b4X?t%*%V%8%'%/%H$,(Bresult_type$B$H$$$&(Btypedef$B$r$7$F$$$l$P!"(B
  // $BLa$jCM$r>JN,$9$k$3$H$,$G$-$k(B.
  // std::less<int>$B$J$I$N(Bbinary_function$B$O$=$&$J$C$F$$$k(B.
  boost::function<void(void) > h
    = boost::bind<void>(PrintValue<int>(), 1); 
  // $BF0:n3NG'(B
  h();

}

/// bind$B$N;HMQK!(B
/// accumulate$B$r(Bbind$B$9$k(B. $B7?;XDj$,I,MW(B.
TEST_F(FunctorsTest, bindAccumulate){
  size_t size = 10;
  vector<int> coll(size,0);
  generate_n(coll.begin(), size, AddOne(1));
  EXPECT_EQ(size, coll.size());
  /// $B$=$N$^$^(Baccumulate
  int correnct_answer = accumulate(coll.begin(),coll.end(),0); 
  cout << "answer: " << correnct_answer << endl;  
  /// bind$B$9$k$?$a$N(Btypedef
  typedef int (*AcumInt)(vector<int>::iterator, vector<int>::iterator, int) ;
  boost::function<int(void)> func 
    = boost::bind(static_cast<AcumInt>(accumulate), coll.begin(), coll.end(), 0); 
  cout << "used AcumInt: " << func() << endl;
  EXPECT_EQ(correnct_answer, func());
} 

  // $BFCDj$N>r7o$rK~$?$9MWAG$KBP$9$k2?$i$+$N=hM}$NLa$jCM$rF@$k(B
