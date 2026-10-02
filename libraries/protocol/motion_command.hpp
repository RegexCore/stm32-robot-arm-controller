#pragma once

#include <optional>

namespace robotarm
{
    struct MotionCommand
    {
        int id = 0;
        int x = 0;
        int y = 0;
        int z = 0;
        std::optional<int> tilt;
        std::optional<int> rotation;
        std::optional<int> gripper;
    };

    // Optional tilt/rotation/gripper integer degrees follow the required x/y/z fields.
    bool parseMotionCommand(const char* text, MotionCommand& command);
    bool parseStatusCommand(const char* text, int& id);
}
