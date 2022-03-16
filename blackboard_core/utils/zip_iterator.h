#pragma once
#include <iterator>
#include <tuple>
#include <utility>

namespace blackboard::core::utils {

namespace impl {
template<typename... T>
class zip_helper
{
public:
    class iterator :
        std::iterator<std::forward_iterator_tag, std::tuple<decltype(*std::declval<T>().begin())...>>
    {
    private:
        std::tuple<decltype(std::declval<T>().begin())...> iters_;

        template<std::size_t... I>
        auto deref(std::index_sequence<I...>) const
        {
            return typename iterator::value_type{*std::get<I>(iters_)...};
        }

        template<std::size_t... I>
        void increment(std::index_sequence<I...>)
        {
            auto l = {(++std::get<I>(iters_), 0)...};
        }

    public:
        explicit iterator(decltype(iters_) iters): iters_{std::move(iters)}
        {}

        iterator &operator++()
        {
            increment(std::index_sequence_for<T...>{});
            return *this;
        }

        iterator operator++(int)
        {
            auto saved{*this};
            increment(std::index_sequence_for<T...>{});
            return saved;
        }

        bool operator!=(const iterator &other) const
        {
            return iters_ != other.iters_;
        }

        auto operator*() const
        {
            return deref(std::index_sequence_for<T...>{});
        }
    };

    zip_helper(T &... seqs)
        : begin_{std::make_tuple(seqs.begin()...)}, end_{std::make_tuple(seqs.end()...)}
    {}

    iterator begin() const
    {
        return begin_;
    }
    iterator end() const
    {
        return end_;
    }

private:
    iterator begin_;
    iterator end_;
};
}    // namespace impl

// Sequences must be the same length.
template<typename... T>
decltype(auto) zip(T &&... seqs)
{
    return impl::zip_helper<T...>{seqs...};
}

}    // namespace blackboard::core::utils
