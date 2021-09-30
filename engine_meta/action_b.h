#pragma once
namespace b {
struct Action
{
    int value{0};
    Action& get()
    {
        return *this;
    }
};
}
