//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/json
//

// Test that header file is self-contained.
#include <boost/json/to_json.hpp>

#include <boost/json/serializer.hpp>
#include <boost/json/value.hpp> // prevent intellisense bugs

#include <string>
#include <vector>

#include "test_suite.hpp"

namespace to_json_test_ns {

//----------------------------------------------------------

// traits
struct T1
{
    int i = 42;
};
} // to_json_test_ns
namespace boost {
namespace json {
template<>
struct to_json_traits<::to_json_test_ns::T1>
{
    static
    value
    construct(
        ::to_json_test_ns::T1 const& t, storage_ptr sp)
    {
        return value(t.i, std::move(sp));
    }
};
} // json
} // boost
namespace to_json_test_ns {

//----------------------------------------------------------

// member function
struct T2
{
    bool b = false;

    ::boost::json::value
    to_json(::boost::json::storage_ptr sp) const
    {
        return ::boost::json::value(b, std::move(sp));
    }
};
BOOST_STATIC_ASSERT(::boost::json::detail::has_to_json_mf<T2>::value);

//----------------------------------------------------------

// traits
struct T3
{
    T1 t1;
    T2 t2;
};
} // to_json_test_ns
namespace boost {
namespace json {
template<>
struct to_json_traits<::to_json_test_ns::T3>
{
    static
    value
    construct(
        ::to_json_test_ns::T3 const& t, storage_ptr sp)
    {
        return ::boost::json::array({
            to_json(t.t1, sp),
            to_json(t.t2, sp)}, sp);
    }
};
} // json
} // boost
namespace to_json_test_ns {

//----------------------------------------------------------

// member function
// uses generic algorithms
struct T4
{
    std::vector<int> v;
    std::string s;

    T4()
        : v({1,2,3})
        , s("test")
    {
        ::boost::json::detail::to_json_impl_4<std::vector<int>>(std::vector<int>{}, {});
        ::boost::json::detail::to_json_impl_4<T4>(T4{}, {});
    }
#if 0
    ::boost::json::value
    to_json(::boost::json::storage_ptr sp) const
    {
        return {
            ::boost::json::to_json(v, sp),
            ::boost::json::to_json(s, sp) };
    }
#endif
};
BOOST_STATIC_ASSERT(! std::is_constructible<::boost::json::value, T4>::value );
BOOST_STATIC_ASSERT(! ::boost::json::detail::has_to_json_traits<T4>::value);
BOOST_STATIC_ASSERT(! ::boost::json::detail::has_to_json_mf<T4>::value);
BOOST_STATIC_ASSERT(! ::boost::json::has_to_json<T4>::value);

//----------------------------------------------------------
} // to_json_test_ns

template<class T>
static
void
check(
    ::boost::json::string_view s,
    T const& t)
{
    {
        auto const jv = to_json(t,
            ::boost::json::storage_ptr{});
        auto const js =
            ::boost::json::to_string(jv);
        BOOST_TEST(js == s);
    }
    {
        auto const jv =
            ::boost::json::to_json(t);
        auto const js =
            ::boost::json::to_string(jv);
        BOOST_TEST(js == s);
    }
}

template<class T>
static
void
testValueCtor()
{
    BOOST_TEST(
        ::boost::json::to_string(
            ::boost::json::to_json(T{})) ==
        ::boost::json::to_string(
            ::boost::json::value(T{})));
}

namespace boost {
namespace json {

class to_json_test
{
public:
    void
    static
    testValueCtors()
    {
        // to_json supports every value constructor

        testValueCtor<value const&>();
    }

    void
    run()
    {
        check("42", ::to_json_test_ns::T1{});
        check("false", ::to_json_test_ns::T2{});
        check("[42,false]", ::to_json_test_ns::T3{});
        check("[[1,2,3],\"test\"]", ::to_json_test_ns::T4{});

        testValueCtors();
    }
};

TEST_SUITE(to_json_test, "boost.json.to_json");

} // json
} // boost
