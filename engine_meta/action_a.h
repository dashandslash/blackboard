#pragma once
namespace a {
struct Action
{
    int value{0};
    Action& get()
    {
        return *this;
    }
};
}
