// Copyright (c) 2024 Hubert Gruniaux
// This file is part of SparseMemory which is released under the MIT license.
// See file LICENSE.txt for full license details.

#include <gtest/gtest.h>

#include "memory.h"

TEST(RamTest, ram_create) {
  ram_t *ram = ram_create();
  ASSERT_NE(ram, nullptr);
  ram_destroy(ram);
}

TEST(RamTest, low_address) {
  ram_t *ram = ram_create();
  ASSERT_NE(ram, nullptr);

  ram_set(ram, 512, 158);
  ram_set(ram, 8652, 326);

  EXPECT_EQ(ram_get(ram, 512), 158);
  EXPECT_EQ(ram_get(ram, 8652), 326);

  ram_destroy(ram);
}

TEST(RamTest, high_address) {
  ram_t *ram = ram_create();
  ASSERT_NE(ram, nullptr);

  ram_set(ram, 1147483647, 84852);

  EXPECT_EQ(ram_get(ram, 1147483647), 84852);

  ram_destroy(ram);
}

TEST(RamTest, many_access) {
  ram_t *ram = ram_create();
  ASSERT_NE(ram, nullptr);

  // Creates approximately 37 memory pages
  for (addr_t i = 52; i < 47483647; i += 1284852) {
    ram_set(ram, i, i);
  }

  // Read data back
  for (addr_t i = 52; i < 47483647; i += 1284852) {
    EXPECT_EQ(ram_get(ram, i), i);
  }

  ram_destroy(ram);
}
