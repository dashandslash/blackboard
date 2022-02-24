#include "meta.h"

namespace blackboard::meta {

const Meta_infos &get_reflected_components_infos()
{
    return impl::meta_components_infos;
}

}    // namespace blackboard::meta
