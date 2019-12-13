//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/json
//

#include <boost/json.hpp>

#include <complex>
#include <iostream>

#include "test_suite.hpp"

#if 0

namespace boost {
namespace json {

namespace {

void
usingStrings()
{
    {
        //[snippet_strings_1

        string str1; // empty string, default storage

        string str2( make_storage<pool>() ); // empty string, pool storage

        //]
    }
}

//----------------------------------------------------------

void
usingArrays()
{
    {
        //[snippet_arrays_1

        array arr1; // empty array, default storage

        array arr2( make_storage<pool>() ); // empty array, pool storage

        //]
    }
    {
        //[snippet_arrays_2

        array arr( { "Hello", 42, true } );

        //]
    }
    {
        //[snippet_arrays_3

        array arr;

        arr.emplace_back( "Hello" );
        arr.emplace_back( 42 );
        arr.emplace_back( true );

        //]

        //[snippet_arrays_4

        assert( arr[0].as_string() == "Hello" );

        // The following line throws std::out_of_range, since the index is out of range
        arr.at( 3 ) = nullptr;

        //]
    }
}

//----------------------------------------------------------

void
usingObjects()
{
    {
        //[snippet_objects_1

        object obj1; // empty object, default storage

        object obj2( make_storage<pool>() ); // empty object, pool storage

        //]
    }
    {
        //[snippet_objects_2

        object obj( {{"key1", "value1" }, { "key2", 42 }, { "key3", false }} );

        //]
    }
    {
        //[snippet_objects_3

        object obj;

        obj.emplace( "key1", "value1" );
        obj.emplace( "key2", 42 );
        obj.emplace( "key3", false );

        //]
    }
    {
        //[snippet_objects_4

        object obj;

        obj["key1"] = "value1";
        obj["key2"] = 42;
        obj["key3"] = false;

        // The following line throws std::out_of_range, since the key does not exist
        obj.at( "key4" );

        //]
    }
}

//----------------------------------------------------------

void
usingStorage()
{
    {
        //[snippet_storage_1

        value jv;                                   // uses the default storage
        storage_ptr sp;                             // uses the default storage
        object obj( sp );                           // uses the default storage

        assert( jv.storage().get() == sp.get() );   // same pointer
        assert( *jv.storage() == *sp );             // deep equality

        assert( jv.storage().get() ==
                    obj.storage().get() );          // same pointer

        //]
    }
}

//[snippet_storage_2

value parse_fast( string_view s )
{
    return parse( s, make_storage<pool>() );
}

//]

void do_json(value const&) {}

//[snippet_storage_3

void do_rpc( string_view cmd )
{
    scoped_storage<pool> sp;

    auto jv = parse( cmd, sp );

    do_json( jv );
}

//]

//[snippet_storage_4

struct Storage
{
    static constexpr std::uint64_t id = 0;
    static constexpr bool need_free = true;

    void* allocate( std::size_t bytes, std::size_t align );
    void deallocate( void* p, std::size_t bytes, std::size_t align );
};

//]

BOOST_STATIC_ASSERT(
    is_storage<Storage>::value);

//----------------------------------------------------------

void
usingParsing()
{
    {
        //[snippet_parsing_1

        value jv = parse( "[1,2,3,4,5]" );

        //]
    }
    {
        //[snippet_parsing_2

        error_code ec;
        value jv = parse( "[1,2,3,4,5]", ec );
        if( ec )
            std::cout << "Parsing failed: " << ec.message() << "\n";

        //]
    }
    {
        //[snippet_parsing_3

        value jv = parse( "[1,2,3,4,5]", make_storage<pool>() );

        //]
    }
    {
        //[snippet_parsing_4

        parser p;

        // This must be called once before parsing every new JSON.
        p.start();

        // Write the entire character buffer, indicating
        // to the parser that there is no more data.
        p.finish( "[1,2,3,4,5]", 11 );

        // Take ownership of the resulting value.
        value jv = p.release();

        // At this point the parser may be re-used by calling p.start() again.

        //]
    }
    {
        //[snippet_parsing_5

        parser p;
        error_code ec;

        // This must be called once before parsing every new JSON
        p.start();

        // Write the first part of the buffer
        p.write( "[1,2,", 5, ec);

        // Write the remaining part of the character buffer,
        // indicating to the parser that there is no more data.
        if(! ec )
            p.finish( "3,4,5]", 6, ec );

        // Take ownership of the resulting value.
        if(! ec)
            value jv = p.release();

        // At this point the parser may be re-used by calling p.start() again.

        //]
    }
    {
        //[snippet_parsing_6

        {
            parser p;
            error_code ec;

            // Declare a new, scoped instance of the block storage
            scoped_storage< pool > sp;

            // Use the scoped instance for the parsed value
            p.start( sp );

            // Write the entire JSON
            p.finish( "[1,2,3,4,5]", 11, ec );

            // The value will use the instance of block storage created above
            value jv = p.release();
        }

        //]
    }
}

//----------------------------------------------------------

void
usingSerializing()
{
    {
        //[snippet_serializing_1

        value jv = { 1, 2, 3 ,4 ,5 };

        std::cout << jv << "\n";

        //]
    }
    {
        //[snippet_serializing_2

        value jv = { 1, 2, 3 ,4 ,5 };

        string s = to_string( jv );

        //]
    }
}

//----------------------------------------------------------

//[snippet_exchange_1

struct customer
{
    std::uint64_t id;
    std::string name;
    bool delinquent;

    customer() = default;
    explicit customer( value const& );
    value to_json_hook( storage_ptr sp ) const;
};

//]

BOOST_STATIC_ASSERT(
    ::boost::json::detail::has_to_json_mf<customer>::value);

BOOST_STATIC_ASSERT(
    has_to_json<customer>::value);

//BOOST_STATIC_ASSERT(
//    has_from_json<customer>::value);

//[snippet_exchange_2

value customer::to_json_hook( storage_ptr sp ) const
{
    // Start with an empty object
    value jv( object_kind, std::move(sp) );
    object& obj = jv.get_object();

    // Each field has its own key/value pair in the object
    obj.emplace( "id", this->id );
    obj.emplace( "name", string_view( this->name ));
    obj.emplace( "delinquent", this->delinquent );

    return jv;
}

//]

//[snippet_exchange_3

customer::customer( value const& jv )
{
    // as_object() will throw if jv.kind() != kind::object
    auto const& obj = jv.as_object();

    // at() will throw if the key is not found,
    // and as_uint64() will throw if the value is
    // not an unsigned 64-bit integer.
    this->id = obj.at( "id" ).as_uint64();

    // as_string() will throw if jv.kind() != kind::string
    this->name = std::string( string_view( obj.at( "name" ).as_string() ) );

    // as_bool() will throw if kv.kind() != kind::bool
    this->delinquent = obj.at( "delinquent" ).as_bool();
}

//]

void
usingExchange1()
{
    //[snippet_exchange_4

    customer cust;
    cust.id = 1;
    cust.name = "John Doe";
    cust.delinquent = false;

    // Convert customer to value
    value jv = to_json( cust );

    // Store value in customer
    //customer cust2;
    //cust2 = value_cast<customer>( jv );

    //]
}

} // (anon)

} // json
} // boost
//[snippet_exchange_5

// Specializations of conversion must be
// declared in the namespace ::boost::json.
namespace boost {
namespace json {

template<>
struct traits< ::std::complex< double > >
{
    static ::boost::json::value to_json_hook(
        ::std::complex< double > const& t, ::boost::json::storage_ptr sp );
};

} // namespace json
} // namespace boost

//]
namespace boost {
namespace json {

BOOST_STATIC_ASSERT(
    has_to_json<std::complex<double>>::value);

//BOOST_STATIC_ASSERT(
//    has_from_json<std::complex<double>>::value);

//[snippet_exchange_6

::boost::json::value
traits< ::std::complex< double > >::
to_json_hook( ::std::complex< double > const& t, ::boost::json::storage_ptr sp )
{
    // Store a complex number as a 2-element array
    ::boost::json::value jv( ::boost::json::array_kind, std::move(sp) );
    auto& arr = jv.get_array();

    // Real part first
    arr.emplace_back( t.real() );

    // Imaginary part last
    arr.emplace_back( t.imag() );

    return jv;
}

//]

//[snippet_exchange_7]

#if 0
void
conversion< ::std::complex< double > >::
from_json( ::std::complex< double >& t, value const& jv )
{
    // as_array() throws if jv.kind() != kind::array
    auto const& arr = jv.as_array();

    // at() throws if index is out of range
    // as_double() throws if kind() != kind::double_
    t.real( arr.at(0).as_double() );

    // imaginary part last
    t.imag( arr.at(1).as_double() );
}
#endif

//]

namespace {

void
usingExchange2()
{
    //[snippet_exchange_8

    std::complex<double> c = { 3.14159, 2.71727 };

    // Convert std::complex<double> to value
    value jv = to_json(c);

    // Store value in std::complex<double>
    //std::complex<double> c2;
    //c2 = value_cast<std::complex<double>>(jv);

    //]
}

} // (anon)

//----------------------------------------------------------

class snippets_test
{
public:
    void
    run()
    {
        (void)&usingStrings;
        (void)&usingArrays;
        (void)&usingObjects;
        (void)&usingStorage;
        (void)&parse_fast;
        (void)&do_json;
        (void)&do_rpc;
        (void)&usingParsing;
        (void)&usingSerializing;
        (void)&usingExchange1;
        (void)&usingExchange2;
        BOOST_TEST_PASS();
    }
};

TEST_SUITE(snippets_test, "boost.json.snippets");

} // json
} // boost

#endif
