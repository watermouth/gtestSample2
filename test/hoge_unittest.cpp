#include "add.h"
#include "gtest/gtest.h"

TEST(AddTest, NegativeOnly){
  EXPECT_EQ(-3,  add(-1,-2));
  EXPECT_EQ(-10, add(-4,-6));
  EXPECT_EQ(-11,  add(-3,-8));
}


