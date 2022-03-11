#include "meta.h"

namespace blackboard::core::meta {

const Meta_infos &get_reflected_components_infos()
{
    return impl::meta_components_infos;
}

}    // namespace blackboard::core::meta
