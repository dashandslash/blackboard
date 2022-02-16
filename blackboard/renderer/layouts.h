#pragma once
#include <bgfx/bgfx.h>
#include <bx/pixelformat.h>
#include <span>

#include <array>

namespace blackboard::renderer::layouts {

struct Position_color
{
    std::array<float, 3> pos;
    uint32_t color;

    inline static bgfx::VertexLayout layout()
    {
        static bool is_init{false};

        if (!is_init)
        {
            init();
            is_init = true;
        }
        return m_layout;
    }

private:
    inline static void init()
    {
        m_layout.begin()
          .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
          .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
          .end();
    };

    inline static bgfx::VertexLayout m_layout;
};

struct Position
{
    float x;
    float y;
    float z;

    inline static bgfx::VertexLayout layout()
    {
        static bool is_init{false};

        if (!is_init)
        {
            init();
            is_init = true;
        }
        return m_layout;
    }

private:
    inline static void init()
    {
        m_layout.begin().add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float).end();
    };

    inline static bgfx::VertexLayout m_layout;
};

struct Position_normal
{
    std::array<float, 3> pos;
    uint32_t normal;

    inline static bgfx::VertexLayout layout()
    {
        static bool is_init{false};

        if (!is_init)
        {
            init();
            is_init = true;
        }
        return m_layout;
    }

private:
    inline static void init()
    {
        m_layout.begin()
          .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
          .add(bgfx::Attrib::Normal, 4, bgfx::AttribType::Uint8, true, true)
          .end();
    }

    inline static bgfx::VertexLayout m_layout;
};

struct Position_normal_barycenter
{
    std::array<float, 3> pos;
    uint32_t normal;
    std::array<float, 3> bar;

    inline static bgfx::VertexLayout layout()
    {
        static bool is_init{false};

        if (!is_init)
        {
            init();
            is_init = true;
        }
        return m_layout;
    }

private:
    inline static void init()
    {
        m_layout.begin()
          .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
          .add(bgfx::Attrib::Normal, 4, bgfx::AttribType::Uint8, true, true)
          .add(bgfx::Attrib::Color1, 3, bgfx::AttribType::Float, true, false)
          .end();
    }
    inline static bgfx::VertexLayout m_layout;
};

uint32_t encode_normal_rgba8(float _x, float _y = 0.0f, float _z = 0.0f, float _w = 0.0f)
{
    const float src[] = {
      _x * 0.5f + 0.5f,
      _y * 0.5f + 0.5f,
      _z * 0.5f + 0.5f,
      _w * 0.5f + 0.5f,
    };
    uint32_t dst;
    bx::packRgba8(&dst, src);
    return dst;
}

std::tuple<std::vector<uint16_t>, std::vector<Position_normal_barycenter>>
  generate_position_normal_barycenter(const std::span<const uint16_t> &indices,
                                      const std::span<const Position_normal> &layout)
{
    std::vector<Position_normal_barycenter> tempV;
    tempV.reserve(indices.size() * 3);

    std::vector<uint16_t> tempI;
    tempI.reserve(indices.size());

    for (auto i = 0; i < indices.size(); i++)
    {
        const auto idx = i % 3;
        auto &position_normal_input = layout[indices[i]];

        Position_normal_barycenter pnb = {
          .pos = position_normal_input.pos,
          .normal = position_normal_input.normal,
          .bar = {(idx == 0) ? 1.0f : 0.0f, (idx == 1) ? 1.0f : 0.0f, (idx == 2) ? 1.0f : 0.0f}};
        tempV.push_back(std::move(pnb));
        tempI.emplace_back(i);
    }

    return {tempI, tempV};
}

static const std::array<Position_normal, 36> cube_position_normal = {
  Position_normal{.pos = {1.0f, 1.0f, 1.0f}, .normal = encode_normal_rgba8(0, 0, 1)},
  {{-1.0f, 1.0f, 1.0f}, encode_normal_rgba8(0, 0, 1)},
  {{-1.0f, -1.0f, 1.0f}, encode_normal_rgba8(0, 0, 1)},
  {{-1.0f, -1.0f, 1.0f}, encode_normal_rgba8(0, 0, 1)},
  {{1.0f, -1.0f, 1.0f}, encode_normal_rgba8(0, 0, 1)},
  {{1.0f, 1.0f, 1.0f}, encode_normal_rgba8(0, 0, 1)},
  {{1.0f, 1.0f, 1.0f}, encode_normal_rgba8(1, 0, 0)},
  {{1.0f, -1.0f, 1.0f}, encode_normal_rgba8(1, 0, 0)},
  {{1.0f, -1.0f, -1.0f}, encode_normal_rgba8(1, 0, 0)},
  {{1.0f, -1.0f, -1.0f}, encode_normal_rgba8(1, 0, 0)},
  {{1.0f, 1.0f, -1.0f}, encode_normal_rgba8(1, 0, 0)},
  {{1.0f, 1.0f, 1.0f}, encode_normal_rgba8(1, 0, 0)},
  {{1.0f, 1.0f, 1.0f}, encode_normal_rgba8(0, 1, 0)},
  {{1.0f, 1.0f, -1.0f}, encode_normal_rgba8(0, 1, 0)},
  {{-1.0f, 1.0f, -1.0f}, encode_normal_rgba8(0, 1, 0)},
  {{-1.0f, 1.0f, -1.0f}, encode_normal_rgba8(0, 1, 0)},
  {{-1.0f, 1.0f, 1.0f}, encode_normal_rgba8(0, 1, 0)},
  {{1.0f, 1.0f, 1.0f}, encode_normal_rgba8(0, 1, 0)},
  {{-1.0f, 1.0f, 1.0f}, encode_normal_rgba8(-1, 0, 0)},
  {{-1.0f, 1.0f, -1.0f}, encode_normal_rgba8(-1, 0, 0)},
  {{-1.0f, -1.0f, -1.0f}, encode_normal_rgba8(-1, 0, 0)},
  {{-1.0f, -1.0f, -1.0f}, encode_normal_rgba8(-1, 0, 0)},
  {{-1.0f, -1.0f, 1.0f}, encode_normal_rgba8(-1, 0, 0)},
  {{-1.0f, 1.0f, 1.0f}, encode_normal_rgba8(-1, 0, 0)},
  {{-1.0f, -1.0f, -1.0f}, encode_normal_rgba8(0, -1, 0)},
  {{1.0f, -1.0f, -1.0f}, encode_normal_rgba8(0, -1, 0)},
  {{1.0f, -1.0f, 1.0f}, encode_normal_rgba8(0, -1, 0)},
  {{1.0f, -1.0f, 1.0f}, encode_normal_rgba8(0, -1, 0)},
  {{-1.0f, -1.0f, 1.0f}, encode_normal_rgba8(0, -1, 0)},
  {{-1.0f, -1.0f, -1.0f}, encode_normal_rgba8(0, -1, 0)},
  {{1.0f, -1.0f, -1.0f}, encode_normal_rgba8(0, 0, -1)},
  {{-1.0f, -1.0f, -1.0f}, encode_normal_rgba8(0, 0, -1)},
  {{-1.0f, 1.0f, -1.0f}, encode_normal_rgba8(0, 0, -1)},
  {{-1.0f, 1.0f, -1.0f}, encode_normal_rgba8(0, 0, -1)},
  {{1.0f, 1.0f, -1.0f}, encode_normal_rgba8(0, 0, -1)},
  {{1.0f, -1.0f, -1.0f}, encode_normal_rgba8(0, 0, -1)}};

static const std::array<Position_normal_barycenter, 36> cube_position_normal_barycenter = {
  Position_normal_barycenter{.pos = {1.0f, 1.0f, 1.0f},
                             .normal = encode_normal_rgba8(0, 0, 1),
                             .bar = {1.0f, 0.0f, 0.0f}},
  {.pos = {-1.0f, 1.0f, 1.0f}, .normal = encode_normal_rgba8(0, 0, 1), .bar = {0.0f, 1.0f, 0.0f}},
  {.pos = {-1.0f, -1.0f, 1.0f}, .normal = encode_normal_rgba8(0, 0, 1), .bar = {0.0f, 0.0f, 1.0f}},
  {.pos = {-1.0f, -1.0f, 1.0f}, .normal = encode_normal_rgba8(0, 0, 1), .bar = {1.0f, 0.0f, 0.0f}},
  {.pos = {1.0f, -1.0f, 1.0f}, .normal = encode_normal_rgba8(0, 0, 1), .bar = {0.0f, 1.0f, 0.0f}},
  {.pos = {1.0f, 1.0f, 1.0f}, .normal = encode_normal_rgba8(0, 0, 1), .bar = {0.0f, 0.0f, 1.0f}},
  {.pos = {1.0f, 1.0f, 1.0f}, .normal = encode_normal_rgba8(1, 0, 0), .bar = {1.0f, 0.0f, 0.0f}},
  {.pos = {1.0f, -1.0f, 1.0f}, .normal = encode_normal_rgba8(1, 0, 0), .bar = {0.0f, 1.0f, 0.0f}},
  {.pos = {1.0f, -1.0f, -1.0f}, .normal = encode_normal_rgba8(1, 0, 0), .bar = {0.0f, 0.0f, 1.0f}},
  {.pos = {1.0f, -1.0f, -1.0f}, .normal = encode_normal_rgba8(1, 0, 0), .bar = {1.0f, 0.0f, 0.0f}},
  {.pos = {1.0f, 1.0f, -1.0f}, .normal = encode_normal_rgba8(1, 0, 0), .bar = {0.0f, 1.0f, 0.0f}},
  {.pos = {1.0f, 1.0f, 1.0f}, .normal = encode_normal_rgba8(1, 0, 0), .bar = {0.0f, 0.0f, 1.0f}},
  {.pos = {1.0f, 1.0f, 1.0f}, .normal = encode_normal_rgba8(0, 1, 0), .bar = {1.0f, 0.0f, 0.0f}},
  {.pos = {1.0f, 1.0f, -1.0f}, .normal = encode_normal_rgba8(0, 1, 0), .bar = {0.0f, 1.0f, 0.0f}},
  {.pos = {-1.0f, 1.0f, -1.0f}, .normal = encode_normal_rgba8(0, 1, 0), .bar = {0.0f, 0.0f, 1.0f}},
  {.pos = {-1.0f, 1.0f, -1.0f}, .normal = encode_normal_rgba8(0, 1, 0), .bar = {1.0f, 0.0f, 0.0f}},
  {.pos = {-1.0f, 1.0f, 1.0f}, .normal = encode_normal_rgba8(0, 1, 0), .bar = {0.0f, 1.0f, 0.0f}},
  {.pos = {1.0f, 1.0f, 1.0f}, .normal = encode_normal_rgba8(0, 1, 0), .bar = {0.0f, 0.0f, 1.0f}},
  {.pos = {-1.0f, 1.0f, 1.0f}, .normal = encode_normal_rgba8(-1, 0, 0), .bar = {1.0f, 0.0f, 0.0f}},
  {.pos = {-1.0f, 1.0f, -1.0f}, .normal = encode_normal_rgba8(-1, 0, 0), .bar = {0.0f, 1.0f, 0.0f}},
  {.pos = {-1.0f, -1.0f, -1.0f}, .normal = encode_normal_rgba8(-1, 0, 0), .bar = {0.0f, 0.0f, 1.0f}},
  {.pos = {-1.0f, -1.0f, -1.0f}, .normal = encode_normal_rgba8(-1, 0, 0), .bar = {1.0f, 0.0f, 0.0f}},
  {.pos = {-1.0f, -1.0f, 1.0f}, .normal = encode_normal_rgba8(-1, 0, 0), .bar = {0.0f, 1.0f, 0.0f}},
  {.pos = {-1.0f, 1.0f, 1.0f}, .normal = encode_normal_rgba8(-1, 0, 0), .bar = {0.0f, 0.0f, 1.0f}},
  {.pos = {-1.0f, -1.0f, -1.0f}, .normal = encode_normal_rgba8(0, -1, 0), .bar = {1.0f, 0.0f, 0.0f}},
  {.pos = {1.0f, -1.0f, -1.0f}, .normal = encode_normal_rgba8(0, -1, 0), .bar = {0.0f, 1.0f, 0.0f}},
  {.pos = {1.0f, -1.0f, 1.0f}, .normal = encode_normal_rgba8(0, -1, 0), .bar = {0.0f, 0.0f, 1.0f}},
  {.pos = {1.0f, -1.0f, 1.0f}, .normal = encode_normal_rgba8(0, -1, 0), .bar = {1.0f, 0.0f, 0.0f}},
  {.pos = {-1.0f, -1.0f, 1.0f}, .normal = encode_normal_rgba8(0, -1, 0), .bar = {0.0f, 1.0f, 0.0f}},
  {.pos = {-1.0f, -1.0f, -1.0f}, .normal = encode_normal_rgba8(0, -1, 0), .bar = {0.0f, 0.0f, 1.0f}},
  {.pos = {1.0f, -1.0f, -1.0f}, .normal = encode_normal_rgba8(0, 0, -1), .bar = {1.0f, 0.0f, 0.0f}},
  {.pos = {-1.0f, -1.0f, -1.0f}, .normal = encode_normal_rgba8(0, 0, -1), .bar = {0.0f, 1.0f, 0.0f}},
  {.pos = {-1.0f, 1.0f, -1.0f}, .normal = encode_normal_rgba8(0, 0, -1), .bar = {0.0f, 0.0f, 1.0f}},
  {.pos = {-1.0f, 1.0f, -1.0f}, .normal = encode_normal_rgba8(0, 0, -1), .bar = {1.0f, 0.0f, 0.0f}},
  {.pos = {1.0f, 1.0f, -1.0f}, .normal = encode_normal_rgba8(0, 0, -1), .bar = {0.0f, 1.0f, 0.0f}},
  {.pos = {1.0f, -1.0f, -1.0f}, .normal = encode_normal_rgba8(0, 0, -1), .bar = {0.0f, 0.0f, 1.0f}}};

static const std::array<uint16_t, 36> cube_indices = {0,  1,  2,  3,  4,  5,    // front
                                                      6,  7,  8,  9,  10, 11,    // right
                                                      12, 13, 14, 15, 16, 17,    // top
                                                      18, 19, 20, 21, 22, 23,    // left
                                                      24, 25, 26, 27, 28, 29,    // bottom
                                                      30, 31, 32, 33, 34, 35};    // back
}    // namespace blackboard::renderer::layouts
