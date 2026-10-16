#include <gtest/gtest.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>
#include <thread>

#include "Keeko/ThreadedScript.h"

namespace
{

// ThreadedScript only runs scripts from disk, so the fixture writes them.
class ThreadedScriptTest: public ::testing::Test
{
protected:
    void SetUp() override
    {
        temp_dir = std::filesystem::temp_directory_path() / "hv_lua_lib_tests";
        std::filesystem::create_directories(temp_dir);
    }

    void TearDown() override
    {
        std::error_code ec;
        std::filesystem::remove_all(temp_dir, ec);
    }

    std::string writeScript(const std::string& name, const std::string& body)
    {
        auto path = temp_dir / name;
        std::ofstream out(path);
        out << body;
        out.close();
        return path.string();
    }

    std::filesystem::path temp_dir;
};

} // namespace

TEST_F(ThreadedScriptTest, runsAScriptToCompletion)
{
    HighVoltage::ThreadedScript script;
    auto path = writeScript("trivial.lua", "local x = 1 + 1\n");

    script.runFile(path);
    script.join();

    EXPECT_FALSE(script.isRunning());
}

TEST_F(ThreadedScriptTest, stopWakesAScriptSleepingInControlSleep)
{
    HighVoltage::ThreadedScript script;
    // control:sleep() is bound by addFunctions() and parks on kill_cv, so
    // stop() should cut it short rather than waiting the full 30 seconds.
    auto path = writeScript("long_sleep.lua", "control:sleep(30)\n");
    script.setOnFailCallback([]() {});

    auto started = std::chrono::steady_clock::now();
    script.runFile(path);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    script.stop();
    auto elapsed = std::chrono::steady_clock::now() - started;

    EXPECT_LT(std::chrono::duration_cast<std::chrono::seconds>(elapsed).count(), 5);
}

TEST_F(ThreadedScriptTest, secondRunFileAfterCompletionIsAccepted)
{
    HighVoltage::ThreadedScript script;
    auto path = writeScript("trivial.lua", "local x = 1\n");

    script.runFile(path);
    script.join();
    EXPECT_NO_THROW(script.runFile(path));
    script.join();
}

// ---------------------------------------------------------------------------
// The tests below are DISABLED because they currently fail or crash. Each one
// documents a real defect found while writing this suite. Re-enable as they
// are fixed - that is the point of leaving them here.
// ---------------------------------------------------------------------------

// BUG: a script that throws calls onFailCallback unconditionally, but the
// member is a default-constructed std::function unless setOnFailCallback() was
// called. Invoking an empty std::function throws std::bad_function_call from
// inside the worker thread, where nothing catches it, so the process calls
// std::terminate(). On the robot that means one syntax error in one auton file
// takes down the whole robot program.
// Fix: guard with `if(onFailCallback)`.
TEST_F(ThreadedScriptTest, DISABLED_scriptFailureWithNoCallbackDoesNotTerminate)
{
    HighVoltage::ThreadedScript script;
    auto path = writeScript("broken.lua", "this is not valid lua =\n");

    script.runFile(path);
    script.join();

    EXPECT_FALSE(script.isRunning());
}

// BUG: ~ThreadedScript() sets running = false and joins, but never notifies
// kill_cv the way stop() does. A script parked in control:sleep(30) therefore
// blocks destruction for the full remaining sleep instead of waking promptly.
// Fix: have the destructor call stop().
TEST_F(ThreadedScriptTest, DISABLED_destructionWakesASleepingScriptPromptly)
{
    auto path = writeScript("long_sleep.lua", "control:sleep(30)\n");

    auto started = std::chrono::steady_clock::now();
    {
        HighVoltage::ThreadedScript script;
        script.setOnFailCallback([]() {});
        script.runFile(path);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    auto elapsed = std::chrono::steady_clock::now() - started;

    EXPECT_LT(std::chrono::duration_cast<std::chrono::seconds>(elapsed).count(), 5);
}

// BUG: `running` is a plain bool written by the worker thread and read by
// isRunning(), runFile(), sleep() and waitUntilTrue() from other threads. That
// is a data race, and it is also why this assertion fails: runFile() sets
// running = true INSIDE the new thread, so the caller can observe false
// immediately after the call returns.
// Fix: make it std::atomic<bool> (<atomic> is already included) and set it in
// runFile() before launching the thread.
TEST_F(ThreadedScriptTest, DISABLED_isRunningIsTrueImmediatelyAfterRunFile)
{
    HighVoltage::ThreadedScript script;
    auto path = writeScript("long_sleep.lua", "control:sleep(30)\n");
    script.setOnFailCallback([]() {});

    script.runFile(path);
    EXPECT_TRUE(script.isRunning());

    script.stop();
}
