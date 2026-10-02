#include "motion_command.hpp"
#include <cstring>

namespace robotarm
{
    namespace
    {
        bool consume(const char*& text, const char* token)
        {
            const auto length = std::strlen(token);
            if (std::strncmp(text, token, length) != 0)
                return false;
            text += length;
            return true;
        }

        bool integer(const char*& text, int limit, bool allowNegative, int& value)
        {
            bool negative = allowNegative && *text == '-';
            if (negative)
                ++text;
            if (*text < '0' || *text > '9')
                return false;
            int result = 0;
            while (*text >= '0' && *text <= '9')
            {
                int digit = *text++ - '0';
                if (result > (limit - digit) / 10)
                    return false;
                result = result * 10 + digit;
            }
            value = negative ? -result : result;
            return true;
        }
    }

    bool parseStatusCommand(const char* text, int& id)
    {
        int parsedId;
        if (!consume(text, "@STATUS(#") ||
            !integer(text, 2147483647, false, parsedId) || parsedId == 0 ||
            !consume(text, ")") || *text != '\0')
            return false;
        id = parsedId;
        return true;
    }

    bool parseMotionCommand(const char* text, MotionCommand& command)
    {
        MotionCommand parsed;
        if (!consume(text, "@MOVE(#") ||
            !integer(text, 2147483647, false, parsed.id) || parsed.id == 0 ||
            !consume(text, ") x=") || !integer(text, 1000, true, parsed.x) ||
            !consume(text, " y=") || !integer(text, 1000, true, parsed.y) ||
            !consume(text, " z=") || !integer(text, 1000, true, parsed.z))
            return false;

        while (*text != '\0')
        {
            std::optional<int>* angle = nullptr;
            if (consume(text, " tilt="))
                angle = &parsed.tilt;
            else if (consume(text, " rotation="))
                angle = &parsed.rotation;
            else if (consume(text, " gripper="))
                angle = &parsed.gripper;
            else
                return false;

            int value;
            if (angle->has_value() || !integer(text, 180, false, value))
                return false;
            *angle = value;
        }

        command = parsed;
        return true;
    }
}
