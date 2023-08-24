#ifndef ASTROVENTURE_PLANETARRAY_H
#define ASTROVENTURE_PLANETARRAY_H

#include <vector>

class Planet;

/**
 * Helper class that represents array of `Planet*`.
 */
class PlanetArray {
public:
    using value_t = Planet*;
    using vector_t = std::vector<Planet*>;

public:
    explicit PlanetArray(int size);

    PlanetArray(const PlanetArray&) = delete;
    PlanetArray& operator=(const PlanetArray&) = delete;

    PlanetArray(PlanetArray &&that) noexcept;
    PlanetArray& operator=(PlanetArray &&that) noexcept;

    /**
     * @return The actual size of the underlying vector.
     */
    size_t size() const;

    /**
     * Counts valid planets, i.e. planets which are not null.
     * @return valid planet count.
     */
    size_t count() const;

    /**
     * Sets the value at the given index.
     * @param index The index of insertion. If the index >= size,
     * underlying vector will be automatically resized.
     * @param value The value to be inserted.
     * @return The reference to the inserted value.
     */
    value_t& set(int index, value_t value);

    /**
     * Returns the value at the given index.
     * @param index The index. If the index >= size,
     * nullptr will be returned.
     * @return The value at the given index.
     */
    value_t get(int index) const;

    /**
     * @param index The index. If the index >= size,
     * nullptr will be returned.
     * @return The value at the given index.
     */
    value_t operator[](int index) const;

    /**
     * Returns reference to the value at the given index.
     * @param index The index. If the index >= size,
     * this function is equivalent to `set(index, nullptr)`
     * @return The value at the given index.
     */
    value_t& operator[](int index);

    vector_t::iterator erase(vector_t::iterator first, vector_t::iterator last);

    value_t front() const noexcept;
    value_t back() const noexcept;

    vector_t::iterator begin();
    vector_t::const_iterator begin() const;
    vector_t::const_iterator cbegin() const;

    vector_t::iterator end();
    vector_t::const_iterator end() const;
    vector_t::const_iterator cend() const;

    /**
     * Iterates over non-null planets.
     * @tparam C The type of the consumer callable.
     * @param consumer Consumer that accepts `Planet*` as an argument.
     */
    template<typename C>
    void forEach(C &&consumer) const {
        static_assert(std::is_invocable_r_v<void, C, Planet*>);

        for (auto planet : *this) {
            if (planet != nullptr) {
                consumer(planet);
            }
        }
    }

private:
    vector_t m_planets;
};

#endif //ASTROVENTURE_PLANETARRAY_H
