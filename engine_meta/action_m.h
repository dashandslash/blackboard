#pragma once

#include <memory>

namespace m {
struct Action
{
    std::unique_ptr<int> value;
};
}
