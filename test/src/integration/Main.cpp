#include "stdafx.h"

#include <gtest/gtest.h>

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    auto result = RUN_ALL_TESTS();
#ifdef _WINDOWS
    system("pause");
#else
    std::cin.ignore();
#endif
    return result;
}
