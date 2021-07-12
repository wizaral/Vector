#include <vector>
#include <numeric>

#include "vector.hpp"

#define TEST_FRAMEWORK_PROFILER
#include "test_framework.hpp"

void defalt_ctor() {
    al::vector<int> v_int;

    ASSERT_EQUAL(v_int.data(), nullptr);
    ASSERT_EQUAL(v_int.capacity(), 0ull);
    ASSERT_EQUAL(v_int.size(), 0ull);

    class Any final {
        int m_i;

    public:
        Any() = delete;

        Any(int i)
        : m_i(i) {}
    };

    al::vector<Any> v_any;

    ASSERT_EQUAL(v_any.data(), nullptr);
    ASSERT_EQUAL(v_any.capacity(), 0ull);
    ASSERT_EQUAL(v_any.size(), 0ull);
}

void it_ctor() {
    size_t size = 100'000'000;

    std::vector<int> v1(size);
    std::iota(v1.begin(), v1.end(), 0);

    al::vector<int> v2(v1.begin(), v1.end());

    ASSERT(std::equal(v1.begin(), v1.end(), v2.begin()));
    ASSERT_EQUAL(v1.capacity(), v2.capacity());

    v1.push_back(size);
    v2.push_back(size);

    ASSERT(std::equal(v1.begin(), v1.end(), v2.begin()));
    ASSERT_EQUAL(v1.capacity(), v2.capacity());
}

void cref_ctor() {
    size_t size = 100'000'000;

    std::vector<int> v1(size, 42);
    al::vector<int> v2(size, 42);

    ASSERT(std::equal(v1.begin(), v1.end(), v2.begin()));
    ASSERT_EQUAL(v1.capacity(), v2.capacity());

    v1.push_back(size);
    v2.push_back(size);

    ASSERT(std::equal(v1.begin(), v1.end(), v2.begin()));
    ASSERT_EQUAL(v1.capacity(), v2.capacity());
}

void copy_ctor() {
    size_t size = 100'000'000;

    al::vector<int> v1(size, 42);
    al::vector<int> v2{v1};

    ASSERT(std::equal(v1.begin(), v1.end(), v2.begin()));
    ASSERT_EQUAL(v1.capacity(), v2.capacity());

    v1.push_back(size);
    v2.push_back(size);

    ASSERT(std::equal(v1.begin(), v1.end(), v2.begin()));
    ASSERT_EQUAL(v1.capacity(), v2.capacity());
}

void move_ctor() {
    size_t size = 100'000'000;

    al::vector<int> v1(size, 42);
    al::vector<int> v2(size, 42);
    al::vector<int> v3{std::move(v1)};

    ASSERT_EQUAL(v1.data(), nullptr);
    ASSERT_EQUAL(v1.capacity(), 0ull);
    ASSERT_EQUAL(v1.size(), 0ull);

    ASSERT(std::equal(v2.begin(), v2.end(), v3.begin()));
    ASSERT_EQUAL(v2.capacity(), v3.capacity());

    v2.push_back(size);
    v3.push_back(size);

    ASSERT(std::equal(v2.begin(), v2.end(), v3.begin()));
    ASSERT_EQUAL(v2.capacity(), v3.capacity());
}

int main() {
    al::TestFramework tf(false);

    tf.RUN_TEST(defalt_ctor);
    tf.RUN_TEST(it_ctor);
    tf.RUN_TEST(cref_ctor);
    tf.RUN_TEST(copy_ctor);
    tf.RUN_TEST(move_ctor);
}
