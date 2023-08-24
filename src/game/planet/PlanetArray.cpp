#include "PlanetArray.h"

#include <algorithm>

PlanetArray::PlanetArray(int size)
    : m_planets(size) {}

PlanetArray::PlanetArray(PlanetArray &&that) noexcept {
    *this = std::move(that);
}

PlanetArray& PlanetArray::operator=(PlanetArray &&that) noexcept {
    if (this == &that)
        return *this;

    std::swap(m_planets, that.m_planets);

    return *this;
}

size_t PlanetArray::size() const {
    return m_planets.size();
}

size_t PlanetArray::count() const {
    return std::ranges::count_if(m_planets, [](Planet *planet) { return planet != nullptr; });
}

PlanetArray::value_t& PlanetArray::set(int index, value_t value) {
    auto size = m_planets.size();

    if (index == size)
        return m_planets.emplace_back(value);

    if (index > size)
        m_planets.resize(index + 1);

    return (m_planets[index] = value);
}

PlanetArray::value_t PlanetArray::get(int index) const {
    return index < m_planets.size() ? m_planets[index] : nullptr;
}

PlanetArray::value_t PlanetArray::operator[](int index) const {
    return index < m_planets.size() ? m_planets[index] : nullptr;
}

PlanetArray::value_t& PlanetArray::operator[](int index) {
    if (index < m_planets.size())
        return m_planets[index];
    return set(index, nullptr);
}

PlanetArray::vector_t::iterator PlanetArray::erase(vector_t::iterator first, vector_t::iterator last) {
    return m_planets.erase(first, last);
}

PlanetArray::value_t PlanetArray::front() const noexcept {
    return m_planets.front();
}

PlanetArray::value_t PlanetArray::back() const noexcept {
    return m_planets.back();
}

PlanetArray::vector_t::iterator PlanetArray::begin() {
    return m_planets.begin();
}

PlanetArray::vector_t::const_iterator PlanetArray::begin() const {
    return m_planets.begin();
}

PlanetArray::vector_t::const_iterator PlanetArray::cbegin() const {
    return m_planets.begin();
}

PlanetArray::vector_t::iterator PlanetArray::end() {
    return m_planets.end();
}

PlanetArray::vector_t::const_iterator PlanetArray::end() const {
    return m_planets.end();
}

PlanetArray::vector_t::const_iterator PlanetArray::cend() const {
    return m_planets.end();
}
