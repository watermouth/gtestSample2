#include "gtest/gtest.h"

#include <vector>
#include <functional>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <boost/assign.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/mem_fn.hpp>
#include "utility.h"

using namespace std;

  template <class T>
  T ReturnSelf(T x) { return x;}

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

  /// 外部からPredicateを設定可能なメンバ関数を持つclass
  struct HogeWithCompare {
    template <class T, class BinaryOperator>
    bool comp(const T&left, const T&right, BinaryOperator bo) {
      return bo(left, right);
    } 
  };

  /// 外部からPredicateを設定可能なメンバ関数を持つtemplate class
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

  /// GetValueを持つクラス
  class FugaWithGetValue {
  public:
    explicit FugaWithGetValue(int v) : value_(v) { }
    int GetValue() const { return value_; }
  private:
    int value_;
  };

  template <class T>
  T MemReturnSelf(T x) { return x;}
   
};

/// 準備:簡単なfuntorとgenerate_n と PRINT_ELEMENTS
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
  EXPECT_EQ(true, hogeObj.comp(12, 10, std::greater<double>()));

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
  // 10以上の値の個数を数える 
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
  // こんな書き方もできる
  EXPECT_EQ(2,
    count_if(x.begin(), x.end(),
             boost::bind(greater_equal<double>(), _1, 10) == true
    )
  );
}

TEST_F(FunctorsTest, transform){
  // transformの練習: 関数オブジェクトをどのように用意するか
  vector<double> x;
  using namespace boost::assign;
  x += 1, 2, 3, 4, 5, 6, 3, 12, 13, 8, 2;
  vector<double> y; 

  PRINT_ELEMENTS(x, "elements: ");
  // 要素抽出(自分自身を返す)
  transform(x.begin(), x.end()// source
    , back_inserter(y)//destination: 一回目なのでback_inserter
     , ReturnSelf<double>); // OK: global functionである。メンバ関数ではない。
  PRINT_ELEMENTS(y, "returned: ");

  // 要素抽出(自分自身を返す)
  // boost::functionを使ってみる
  boost::function<double(double) > fObj = ReturnSelf<double>;
  transform(x.begin(), x.end()// source
    , (y.begin())//destination
    , fObj);
  PRINT_ELEMENTS(y, "returned: ");

  // 要素抽出(自分自身を返す)
  // このテストクラスのメンバ関数を使ってみる 
  fObj = boost::bind(
      boost::mem_fn(&FunctorsTest::MemReturnSelf<double>)
    , this, _1);  
  transform(x.begin(), x.end()// source
    , (y.begin())//destination
    , fObj);
  PRINT_ELEMENTS(y, "returned: ");
  // またはbind1stを使うと
  fObj = bind1st( 
      boost::mem_fn(&FunctorsTest::MemReturnSelf<double>)
    , this);  
  transform(x.begin(), x.end()// source
    , (y.begin())//destination
    , fObj);
  PRINT_ELEMENTS(y, "returned: ");
}

  // 特定の条件を満たす要素に対する何らかの処理の戻り値を得る
