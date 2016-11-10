#include <iostream>
#include <gtest/gtest.h>

int factorial(int n) {
    int result = 1;
    for (int i = 1; i <= n; ++i) {
        result *= i;
    }
    return result;
}

/*
TEST(FactorialTest, Negative) {
    EXPECT_EQ(1, factorial(-5));
    EXPECT_EQ(1, factorial(-1));
    EXPECT_GT(factorial(-10), 0);
}

TEST(FactorialTest, Zero) {
    EXPECT_EQ(1, factorial(0));
}

TEST(FactorialTest, Positive) {
    EXPECT_EQ(1, factorial(1));
    EXPECT_EQ(2, factorial(2));
    EXPECT_EQ(6, factorial(3));
    EXPECT_EQ(40320, factorial(8));
}
*/

int main(int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}