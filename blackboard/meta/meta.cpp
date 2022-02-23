#include "meta.h"

namespace blackboard::meta {

const std::vector<info> &get_reflected_components_infos()
{
    return impl::reflected_components_infos;
}

}    // namespace blackboard::meta
