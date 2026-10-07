#include "BitSet.hpp"
#include <gtest/gtest.h>

// Тест группы "BitSetTest", конкретный кейс "InitialState"
TEST(BitSetTest, InitialState) {
  sets::BitSet set(10);
  // Проверяем, что по умолчанию элементы отсутствуют
  EXPECT_FALSE(set.contains(0));
  EXPECT_FALSE(set.contains(9));
}

TEST(BitSetTest, SetAndContains) {
  sets::BitSet set(10);
  set.set(3, true);

  EXPECT_TRUE(set.contains(3));
  EXPECT_FALSE(set.contains(2));
}
