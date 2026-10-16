#include <gtest/gtest.h>

#include <string>

#include "Keeko/LuaException.h"
#include "Keeko/LuaState.h"

TEST(LuaStateTest, validChunkParsesAndRuns)
{
    HighVoltage::LuaState state;

    EXPECT_NO_THROW(state.parseString("local x = 1 + 1"));
}

TEST(LuaStateTest, syntaxErrorThrowsWithAUsefulMessage)
{
    HighVoltage::LuaState state;

    try
    {
        state.parseString("this is not valid lua =", "bad_chunk");
        FAIL() << "expected LuaException";
    }
    catch(const HighVoltage::LuaException& e)
    {
        // The chunk name is what makes a parse error traceable back to a
        // specific auton file on the robot.
        EXPECT_NE(std::string(e.what()).find("bad_chunk"), std::string::npos);
    }
}

TEST(LuaStateTest, runtimeErrorThrows)
{
    HighVoltage::LuaState state;

    EXPECT_THROW(state.parseString("error('boom')"), HighVoltage::LuaException);
}

TEST(LuaStateTest, runFunctionCallsAGlobalDefinedEarlier)
{
    HighVoltage::LuaState state;

    state.parseString("called = false\nfunction myEntryPoint() called = true end");
    EXPECT_NO_THROW(state.runFunction("myEntryPoint"));
}

TEST(LuaStateTest, runFunctionOnAMissingGlobalThrows)
{
    HighVoltage::LuaState state;

    EXPECT_THROW(state.runFunction("doesNotExist"), HighVoltage::LuaException);
}

// Documents current behaviour, which is probably wrong. loadFile() builds an
// ifstream and reads it into a string without ever checking that the open
// succeeded, so a missing path yields an empty chunk, which Lua happily parses
// as a no-op. A typo'd auton filename therefore "runs" and does nothing
// instead of reporting an error.
TEST(LuaStateTest, loadFileOnAMissingPathSilentlySucceeds)
{
    HighVoltage::LuaState state;

    EXPECT_NO_THROW(state.loadFile("/nonexistent/path/to/auton.lua"));
}
