#include <gtest/gtest.h>

// No HAL_Initialize() here, unlike a robot project's test main - this library
// does not touch WPILib, so the tests need no HAL.
int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
