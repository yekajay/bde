// bslh_hash.t.cpp                                                    -*-C++-*-
#include <bslh_hash.h>
#include <bslh_defaulthashalgorithm.h>
#include <bslh_defaultseededhashalgorithm.h>
#include <bslh_siphashalgorithm.h>
#include <bslh_spookyhashalgorithm.h>

#include <bslmf_isbitwisemoveable.h>
#include <bslmf_issame.h>
#include <bslmf_istriviallycopyable.h>
#include <bslmf_istriviallydefaultconstructible.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_platform.h>

#include <math.h>
#include <wchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace BloombergLP;
using namespace bslh;


//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// TYPEDEF
// [ 5] typedef size_t result_type;
//
// CREATORS
// [ 2] Hash()
// [ 2] Hash(const Hash)
// [ 2] ~Hash()
//
// MANIPULATORS
// [ 2] Hash& operator=(const Hash&)
//
// ACCESSORS
// [ 4] operator()(const T&) const
//
// FREE FUNCTIONS
// [ 3] void hashAppend(HASHALG& hashAlg, bool input);
// [ 3] void hashAppend(HASHALG& hashAlg, char input);
// [ 3] void hashAppend(HASHALG& hashAlg, signed char input);
// [ 3] void hashAppend(HASHALG& hashAlg, unsigned char input);
// [ 3] void hashAppend(HASHALG& hashAlg, wchar_t input);
// [ 3] void hashAppend(HASHALG& hashAlg, char16_t input);
// [ 3] void hashAppend(HASHALG& hashAlg, char32_t input);
// [ 3] void hashAppend(HASHALG& hashAlg, short input);
// [ 3] void hashAppend(HASHALG& hashAlg, unsigned short input);
// [ 3] void hashAppend(HASHALG& hashAlg, int input);
// [ 3] void hashAppend(HASHALG& hashAlg, unsigned int input);
// [ 3] void hashAppend(HASHALG& hashAlg, long input);
// [ 3] void hashAppend(HASHALG& hashAlg, unsigned long input);
// [ 3] void hashAppend(HASHALG& hashAlg, long long input);
// [ 3] void hashAppend(HASHALG& hashAlg, unsigned long long input);
// [ 3] void hashAppend(HASHALG& hashAlg, float input);
// [ 3] void hashAppend(HASHALG& hashAlg, double input);
// [ 3] void hashAppend(HASHALG& hashAlg, long double input);
// [ 3] void hashAppend(HASHALG& hashAlg, const char (&input)[N]);
// [ 3] void hashAppend(HASHALG& hashAlg, const TYPE (&input)[N]);
// [ 3] void hashAppend(HASHALG& hashAlg, const void *input);
// [ 3] void hashAppend(HASHALG& hashAlg, RT (*input)(ARGS...));
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 8] USAGE EXAMPLE
// [ 6] IsBitwiseMovable trait
// [ 6] is_trivially_copyable trait
// [ 6] is_trivially_default_constructible trait
// [ 7] QoI: Support for empty base optimization
//-----------------------------------------------------------------------------

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

namespace {

int testStatus = 0;

void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

// ============================================================================
//                      STANDARD BDE TEST DRIVER MACROS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                             USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example 1: Keying a hash table with a user defined type
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we have a value semantic type, 'Box', that contains attributes that
// are salient to hashing as well as attributes that are not salient to
// hashing. Some of these attributes are themselves user defined types. We want
// to store objects of type 'Box' in a hash table, so we need to be able to
// produce hash values that represent instances of 'Box'. We don't want to
// write our own hashing or hash combine algorithm, because we know it is very
// difficult and labour intensive to write a proper hashing algorithm. In order
// to hash this 'Box', we will use the modular hashing system supplied in
// 'bslh'.
//
// First, we define 'Point', a class that allows us to identify a loction on a
// two dimensional cartesian plane.
//..

class Point {
    // This class is a value semantic type that represents a two dimensional
    // location on a cartesian plane.

  private:
    int    d_x;
    int    d_y;
    double d_distToOrigin; // This value will be accessed frequently, so we
                           // cache it rather than recalculate it every time.

  public:
    Point (int x, int y);
        // Create a 'Point' having the specified 'x' and 'y' coordinates.

    double distanceToOrigin() const;
        // Return the distance from the origin (0, 0) to this point.

    int getX() const;
        // Return the x coordinate of this point.

    int getY() const;
        // Return the y coordinate of this point.
};

inline
Point::Point(int x, int y)
: d_x(x)
, d_y(y)
{
    d_distToOrigin = sqrt(static_cast<double>(d_x * d_x) +
                          static_cast<double>(d_y * d_y));
}

inline
double Point::distanceToOrigin() const
{
    return d_distToOrigin;
}

inline
int Point::getX() const
{
    return d_x;
}

inline
int Point::getY() const
{
    return d_y;
}


//..
// Then, we define 'operator=='. Notice how it checks only attributes that we
// would want to incorporate into the hashed value. Note that attributes that
// are salient to hashing tend to be the same as or a subset of the attributes
// that are checked in 'operator=='.
//..
bool operator==(const Point &lhs, const Point &rhs)
    // Return true if the specified 'lhs' and 'rhs' have the same value. Two
    // 'Point' objects have the same value if they have the same x and y
    // coordinates.
{
    return (lhs.getX() == rhs.getX()) && (lhs.getY() == rhs.getY());
}

//..
// Next, we define 'hashAppend'. This function will allow any hashing algorithm
// that meets the 'bslh' hashing algorithm requirements to be applied to
// 'Point'. This is the full extent of the work that needs to be done by type
// creators. They do not need to implement any algorithms, they just need to
// call out the attributes that are salient to hashing by calling 'hashAppend'
// on them.
//..
template <class HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM &hashAlg, const Point &point)
    // Apply the specified 'hashAlg' to the specified 'point'
{
    hashAppend(hashAlg, point.getX());
    hashAppend(hashAlg, point.getY());
}

//..
// Then, we declare another value semantic type, 'Box' that will have a 'Point'
// as one of its attributes that are salient to hashing.
//..
class Box {
    // This class is a value semantic type that represents a box drawn on to a
    // cartesian plane.

  private:
    Point d_position;
    int d_length;
    int d_width;

  public:
    Box(Point position, int length, int width);
        // Create a box having the specified 'length' and 'width', with its
        // upper left corner at the specified 'position'

    int getLength() const;
        // Return the length of this box.

    Point getPosition() const;
        // Return a 'Point' representing the upper left corner of this box on a
        // cartesian plane

    int getWidth() const;
        // Return the width of this box.
};

inline
Box::Box(Point position, int length, int width)
: d_position(position)
, d_length(length)
, d_width(width) { }

int Box::getLength() const
{
    return d_length;
}

Point Box::getPosition() const
{
    return d_position;
}

int Box::getWidth() const
{
    return d_width;
}

//..
// Then, we define 'operator=='. This time all of the data members are salient
// to equality.
//..
bool operator==(const Box &lhs, const Box &rhs)
    // Return true if the specified 'lhs' and 'rhs' have the same value. Two
    // 'Box' objects have the same value if they have the same length, width,
    // and position.
{
    return (lhs.getPosition() == rhs.getPosition()) &&
           (lhs.getLength()   == rhs.getLength()) &&
           (lhs.getWidth()    == rhs.getWidth());
}

//..
// Next, we define 'hashAppend' for 'Box'. Notice how as well as calling
// 'hashAppend' on fundamental types, we can also call it with our user defined
// type 'Point'. Calling 'hashAppend' with 'Point' will propogate a reference
// to the hashing algorithm functor 'hashAlg' down to the fundamental types
// that make up 'Point', and those types will then be passed into the
// referenced algorithm functor.
//..
template <class HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM &hashAlg, const Box &box)
    // Apply the specified 'hashAlg' to the specified 'box'
{
    hashAppend(hashAlg, box.getPosition());
    hashAppend(hashAlg, box.getLength());
    hashAppend(hashAlg, box.getWidth());
}

//..
// Then, we create our hash table. We simplify the problem by requiring the
// caller to supply an array. This means we already know how many buckets we
// need.  We do not need to copy the values into our own area, so we don't have
// to create storage for them, or require that a copy constructor or destructor
// be available.  We only require that they have a transitive, symmetric
// equivalence operation 'bool operator==' and that a they are hashable using
// 'bslh::Hash'.
//
// Our hash table takes two type parameters: 'TYPE' (the type being referenced)
// and 'HASHER' (a functor that produces the hash). 'HASHER' will default to
// 'bslh::Hash<>'.

template <class TYPE, class HASHER = bslh::Hash<> >
class HashTable {
    // This class template implements a hash table providing fast lookup of an
    // external, non-owned, array of values of (template parameter) 'TYPE'.
    //
    // The (template parameter) 'TYPE' shall have a transitive, symmetric
    // 'operator==' function and it will be hashable using 'bslh::Hash'.  Note
    // that there is no requirement that it have any kind of creator defined.
    //
    // The 'HASHER' template parameter type must be a functor with a method
    // having the following signature:
    //..
    //  size_t operator()(TYPE)  const;
    //                   -OR-
    //  size_t operator()(const TYPE&) const;
    //..
    // and 'HASHER' shall have a publicly accessible default constructor and
    // destructor. Here we use 'bslh::Hash' as our default template argument.
    // This allows us to hash any type for which 'hashAppend' has been
    // implemented.
    //
    // Note that this hash table has numerous simplifications because we know
    // the size of the array and never have to resize the table.

    // DATA
    const TYPE       *d_values;             // Array of values table is to hold
    size_t            d_numValues;          // Length of 'd_values'.
    const TYPE      **d_bucketArray;        // Contains ptrs into 'd_values'
    unsigned          d_bucketArrayMask;    // Will always be '2^N - 1'.
    HASHER            d_hasher;

  private:
    // PRIVATE ACCESSORS
    bool lookup(size_t      *idx,
                const TYPE&  value,
                size_t       hashValue) const
        // Look up the specified 'value', having the specified 'hashValue', and
        // load its index in 'd_bucketArray' into the specified 'idx'.  If not
        // found, return the vacant entry in 'd_bucketArray' where it should be
        // inserted.  Return 'true' if 'value' is found and 'false' otherwise.
    {
        const TYPE *ptr;
        for (*idx = hashValue & d_bucketArrayMask; (ptr = d_bucketArray[*idx]);
                                       *idx = (*idx + 1) & d_bucketArrayMask) {
            if (value == *ptr) {
                return true;                                          // RETURN
            }
        }
        // value was not found in table

        return false;
    }

  public:
    // CREATORS
    HashTable(const TYPE *valuesArray,
              size_t      numValues)
        // Create a hash table referring to the specified 'valuesArray' having
        // length of the specified 'numValues'. No value in 'valuesArray' shall
        // have the same value as any of the other values in 'valuesArray'
    : d_values(valuesArray)
    , d_numValues(numValues)
    , d_hasher()
    {
        size_t bucketArrayLength = 4;
        while (bucketArrayLength < numValues * 4) {
            bucketArrayLength *= 2;

        }
        d_bucketArrayMask = bucketArrayLength - 1;
        d_bucketArray = new const TYPE *[bucketArrayLength];
        memset(d_bucketArray,  0, bucketArrayLength * sizeof(TYPE *));

        for (unsigned i = 0; i < numValues; ++i) {
            const TYPE& value = d_values[i];
            size_t idx;
            BSLS_ASSERT_OPT(!lookup(&idx, value, d_hasher(value)));
            d_bucketArray[idx] = &d_values[i];
        }
    }

    ~HashTable()
        // Free up memory used by this cross-reference.
    {
        delete [] d_bucketArray;
    }

    // ACCESSORS
    bool contains(const TYPE& value) const
        // Return true if the specified 'value' is found in the table and false
        // otherwise.
    {
        size_t idx;
        return lookup(&idx, value, d_hasher(value));
    }
};

//=============================================================================
//          GLOBAL TYPEDEFS, HELPER FUNCTIONS, AND CLASSES FOR TESTING
//-----------------------------------------------------------------------------

typedef bslh::Hash<> Obj;

static bool binaryCompare(const char *first, const char *second, size_t size)
    // Return the result of a comparison of the binary representation of the
    // specified 'size' number of bytes of the data pointed to by the specified
    // 'first' and 'second'.
{
    for (size_t i = 0; i < size; ++i) {
        if(!(first[i] == second[i])) {
            return false;                                             // RETURN
        }
    }
    return true;
}

class MockHashingAlgorithm {
    // This class implements a mock hashing algorithm that provides a way to
    // examine data that is being passed into hashing algorithms by
    // 'hashAppend'.

    char   *d_data;   // Data we were asked to hash
    size_t  d_length; // Length of the data we were asked to hash

  public:
    MockHashingAlgorithm()
    : d_length(0)
        // Create a new 'MockHashingAlgorithm'
    {
        d_data = new char[0];
    }

    ~MockHashingAlgorithm()
        // Destroy this object
    {
        delete [] d_data;
    }

    void operator()(const void *voidPtr, size_t length)
        // Store the specified 'voidPtr' and 'length' for inspection later.
    {
        const char *ptr = reinterpret_cast<const char *>(voidPtr);
        delete [] d_data;
        d_data = new char [length];
        memcpy(d_data, ptr, length);
        d_length = length;
    }

    const char *getData()
        // Return the pointer stored by 'operator()'. The behaviour is
        // undefined if 'operator()' has not been called.
    {
        return d_data;
    }

    size_t getLength()
        // Return the length stored by 'operator()'. The behaviour is
        // undefined if 'operator()' has not been called.
    {
        return d_length;
    }
};


class MockAccumulatingHashingAlgorithm {
    // This class implements a mock hashing algorithm that provides a way to
    // accumulate and then examine data that is being passed into hashing
    // algorithms by 'hashAppend'.

    char   *d_data;   // Data we were asked to hash
    size_t  d_length; // Length of the data we were asked to hash

  public:
    MockAccumulatingHashingAlgorithm()
    : d_length(0)
        // Create a new 'MockAccumulatingHashingAlgorithm'
    {
        d_data = new char[0];
    }

    ~MockAccumulatingHashingAlgorithm()
        // Destroy this object
    {
        delete [] d_data;
    }

    void operator()(const void *voidPtr, size_t length)
        // Store the specified 'voidPtr' and 'length' for inspection later.
    {
        const char *ptr = reinterpret_cast<const char *>(voidPtr);
        char *newPtr = new char [d_length + length];
        char *oldPtr = d_data;
        memcpy(newPtr, oldPtr, d_length);
        memcpy(newPtr + d_length, ptr, length);

        d_data = newPtr;
        delete [] oldPtr;

        d_length += length;
    }

    const char *getData()
        // Return the pointer stored by 'operator()'. The behaviour is
        // undefined if 'operator()' has not been called.
    {
        return d_data;
    }

    size_t getLength()
        // Return the length stored by 'operator()'. The behaviour is
        // undefined if 'operator()' has not been called.
    {
        return d_length;
    }
};

template<class TYPE>
class TestDriver {
    // This class implements a test driver that can run tests on any type.

    char data[sizeof(TYPE)];

  public:
    TestDriver()
        // Construct a 'TestDriver'
    {
        srand(37);
        for (unsigned int i = 0; i < sizeof(TYPE); ++i) {
            data[i] = static_cast<char>(rand());
        }
    }

    void testHashAppendNegativeZero()
        // Test 'hashAppend' on the (template parameter) 'TYPE' ensuring that
        // both 0 and -0 will hash to the same value. This is intended to test
        // floating point numbers where 0.0 and -0.0 have different binary
        // representations.
    {
        TYPE zero = 0;
        TYPE negativeZero = -zero;
        ASSERT(!binaryCompare(reinterpret_cast<const char *>(&zero),
                              reinterpret_cast<const char *>(&negativeZero),
                              sizeof(TYPE)));

        MockHashingAlgorithm zeroAlg;
        hashAppend(zeroAlg, zero);
        MockHashingAlgorithm negativeZeroAlg;
        hashAppend(negativeZeroAlg, negativeZero);
        ASSERT(binaryCompare(zeroAlg.getData(),
                             negativeZeroAlg.getData(),
                             sizeof(TYPE)));
    }

    void testHashAppendPassThrough(int line)
        // Test 'hashAppend' on 'TYPE', using the specified 'line' in a
        // 'LOOP_ASSERT'.
    {
        MockHashingAlgorithm alg;
        TYPE input;
        memcpy(&input, data, sizeof(TYPE));
        hashAppend(alg, input);

        const char *output = alg.getData();
        for (size_t i = 0; i < sizeof(TYPE); ++i) {
            LOOP_ASSERT(line, output[i] == data[i]);
        }
        LOOP_ASSERT(line, alg.getLength() == sizeof(TYPE));
    }
};

int testFunction1()
    // This function is used only for testing funciton pointers
{
    return 1;
}

int testFunction2()
    // This function is used only for testing funciton pointers
{
    return 1;
}

int testFunction3()
    // This function is used only for testing funciton pointers
{
    return 3;
}

class SomeType {

};


// ============================================================================
//                            MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
//  bool     veryVeryVerbose = argc > 4;
//  bool veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 8: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   The hashing algorithm can be applied to user defined types which
        //   in turn can be used in more powerful components such as hash
        //   tables.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Run the usage example (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("USAGE EXAMPLE\n"
                            "=============\n");

//..
// Next, we will create an array of boxes that we want to store in our hash
// table.
//..

        Box boxes[] = { Box(Point(1, 1), 3, 2),
                        Box(Point(3, 1), 4, 2),
                        Box(Point(1, 2), 3, 3),
                        Box(Point(1, 1), 2, 2),
                        Box(Point(1, 4), 4, 3),
                        Box(Point(2, 1), 4, 2),
                        Box(Point(1, 0), 3, 1)};
        enum { NUM_BOXES = sizeof boxes / sizeof *boxes };

//..
// Then, we create our hash table 'hashTable'.  We pass we use the default
// functor which will pick up the 'hashAppend' function we created:
//..

        HashTable<Box> hashTable(boxes, NUM_BOXES);

// Now, we verify that each element in our array registers with count:
        for ( int i = 0; i < 6; ++i) {
            ASSERT(hashTable.contains(boxes[i]));
        }

// Finally, we verify that futures not in our original array are correctly
// identified as not being in the set:

        ASSERT(!hashTable.contains(Box(Point(1, 1), 1, 1)));
        ASSERT(!hashTable.contains(Box(Point(0, 0), 0, 0)));
        ASSERT(!hashTable.contains(Box(Point(3, 3), 3, 3)));

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING QOI: IS AN EMPTY TYPE
        //   As a quality of implementation issue, the class has no state and
        //   should support the use of the empty base class optimization on
        //   compilers that support it.
        //
        // Concerns:
        //: 1 class 'Hash' does not increase the size of an object when used as
        //:   a base class.
        //:
        //: 2 'Hash' does increase the size of an object when used as a data
        //:   member.
        //
        // Plan:
        //: 1 Define two non-empty classes with no padding, one of which is
        //:   derived from 'hash'. Assert that both classes have the same size.
        //:   (C-1).
        //:
        //: 2 Create a third class, with identical structure to the previous
        //:   two, but with 'Hash' additional data member. Assert that the
        //:   class with 'Hash' as a member is larger than the other classes.
        //:   (C-2)
        //
        // Testing:
        //   QoI: Support for empty base optimization
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING QOI: IS AN EMPTY TYPE"
                            "\n=============================\n");

        typedef DefaultHashAlgorithm TYPE;

        if (verbose) printf("Define two non-empty classes with no padding, one"
                            " of which is derived from 'hash'. Assert that"
                            " both classes have the same size. (C-1).\n");
        {
            struct TwoInts {
                int a;
                int b;
            };

            struct DerivedInts : Hash<TYPE> {
                int a;
                int b;
            };

            ASSERT(8 == sizeof(TwoInts));
            ASSERT(8 == sizeof(DerivedInts));
        }

        if (verbose) printf("Create a third class, with identical structure to"
                            " the previous two, but with 'Hash' additional"
                            " data member. Assert that the class with 'Hash'"
                            " as a member is larger than the other classes."
                            "(C-2)\n");
        {

            struct IntsWithMember {
                Hash<TYPE> dummy;
                int              a;
                int              b;
            };

            ASSERT(8 < sizeof(IntsWithMember));
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING BDE TRAITS
        //   The functor is an empty POD, and should have the appropriate BDE
        //   type traits to reflect this.
        //
        // Concerns:
        //: 1 The class is bitwise moveable.
        //: 2 The class has the trivial copyable trait.
        //: 3 The class has the trivial default constructor trait.
        //
        // Plan:
        //: 1 ASSERT the presence of each trait using the 'bslalg::HasTrait'
        //:   metafunction. (C-1..3)
        //
        // Testing:
        //   IsBitwiseMovable trait
        //   is_trivially_copyable trait
        //   is_trivially_default_constructible trait
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING BDE TRAITS"
                            "\n==================\n");

        typedef int TYPE;

        if (verbose) printf("ASSERT the presence of each trait using the"
                            " 'bslalg::HasTrait' metafunction. (C-1..3)\n");
        {
            ASSERT(bslmf::IsBitwiseMoveable<Hash<> >::value);
            ASSERT(bsl::is_trivially_copyable<Hash<> >::value);
            ASSERT(bsl::is_trivially_default_constructible<Hash<> >::value);
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'result_type' TYPEDEF
        //   Verify that the struct hashes the proper 'typedef's.
        //
        // Concerns:
        //: 1 The typedef 'result_type' is publicly accessible and an alias for
        //:   'size_t'.
        //:
        //: 2 'result_type' is 'size_t' even when the algorithm returns a
        //:   'result_type' of a different size
        //:
        //: 3 'operator()' returns 'result_type'
        //
        //
        // Plan:
        //: 1 ASSERT the 'typedef' accessibly aliases the correct type using
        //:   'bslmf::IsSame' for a number of algorithms of different result
        //:   types. (C-1,2)
        //:
        //: 2 Declare the expected signature of 'operator()' and then assign to
        //:   it. If it compiles, the test passes. (C-3)
        //
        // Testing:
        //   typedef size_t result_type;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'result_type' TYPEDEF"
                            "\n=============================\n");

        if (verbose) printf("ASSERT the 'typedef' accessibly aliases the"
                            " correct type using 'bslmf::IsSame' for a number"
                            " of algorithms of different result types."
                            " (C-1,2)\n");
        {

            ASSERT((bslmf::IsSame<size_t, Hash<>::result_type>::VALUE));
            ASSERT((bslmf::IsSame<size_t,
                                  Hash<DefaultHashAlgorithm>
                                                      ::result_type >::VALUE));
            ASSERT((bslmf::IsSame<size_t,
                                  Hash<DefaultSeededHashAlgorithm>
                                                       ::result_type>::VALUE));
            ASSERT((bslmf::IsSame<size_t,
                                  Hash<SipHashAlgorithm>::result_type>
                                                                     ::VALUE));
            ASSERT((bslmf::IsSame<size_t,
                                  Hash<SpookyHashAlgorithm>::result_type>
                                                                     ::VALUE));
        }

        if (verbose) printf("Declare the expected signature of 'operator()'"
                            " and then assign to it. If it compiles, the test"
                            " passes. (C-3)\n");
        {
            Hash<>::result_type (Hash<>::*expectedSignature)(const int&) const;

            expectedSignature = &Hash<>::operator();

            Hash<SpookyHashAlgorithm>::result_type (Hash<SpookyHashAlgorithm>
                                      ::*expectedSignature2)(const int&) const;
            expectedSignature2 = &Hash<SpookyHashAlgorithm>::operator();
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'operator()'
        //   Verify that the struct offers the ability to invoke it with some
        //   bytes and a length, and that it return a hash.
        //
        // Concerns:
        //: 1 The function call operator will return the expected value
        //:   according to the canonical implementation of the algorithm being
        //:   used.
        //:
        //: 2 The function call operator can be invoked on constant objects.
        //
        // Plan:
        //: 1 Create 'const' ints and hash them. Compare the results against
        //:   known good values. (C-1,2)
        //
        // Testing:
        //   operator()(const T&) const
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'operator()'"
                            "\n====================\n");

        static const struct {
            int                  d_line;
            const int            d_value;
            bsls::Types::Uint64  d_hash;
        } DATA[] = {
        // LINE    DATA              HASH
         {  L_,        1,  9778072230994240314ULL,},
         {  L_,        3, 16874605512690156844ULL,},
         {  L_,        9,  6609278684846086166ULL,},
         {  L_,       27, 14610053422485613907ULL,},
         {  L_,       81,  4473763709117720193ULL,},
         {  L_,      243,  6469189993869193617ULL,},
         {  L_,      729, 18245170745653607298ULL,},
         {  L_,     2187,  4418771231001558887ULL,},
         {  L_,     6561,  8361494415593539480ULL,},
         {  L_,    19683,  8034516711244389554ULL,},
         {  L_,    59049, 15257840606198213647ULL,},
         {  L_,   177147,  9838846006369268307ULL,},
         {  L_,   531441,  2891007685366740764ULL,},
         {  L_,  1594323,  3005240762459740192ULL,},
         {  L_,  4782969,  3383268391725748969ULL,},
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) printf("Create 'const' strings and hash them. Compare the"
                            " results against known good values. (C-1,2)\n");
        {
            for (int i = 0; i != NUM_DATA; ++i) {
                const int    LINE  = DATA[i].d_line;
                const int    VALUE = DATA[i].d_value;
                const size_t HASH  = static_cast<size_t>(DATA[i].d_hash);

                if (veryVerbose) printf("Hashing: %i, Expecting: %lu\n",
                                        VALUE,
                                        HASH);

                Obj hash = Obj();
                LOOP_ASSERT(LINE, hash(VALUE) == HASH);

                const Obj constHash = Obj();
                LOOP_ASSERT(LINE, constHash(VALUE) == HASH);
            }
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'hashAppend'
        //   Verify that the 'hashAppend' free functions have been implemented
        //   for all of the fundamental types and don't truncate or pass extra
        //   data into the algorithms.
        //
        // Concerns:
        //: 1 'hashAppend' has been implemented for the fundamental types.
        //:
        //: 2 Floating point values -0.0 and +0.0 result in the same bytes
        //:   being passed into the hashing algorithm.
        //:
        //: 3 'bool' values result in one of two different binary states being
        //:   passed into the algorithms (doesn't need to be 00000000 and
        //:   00000001, but it does need to be consistent).
        //:
        //: 4 Function pointers and object pointers are hashed as pointers, NOT
        //:   the data they point to.
        //:
        //: 5 'hashAppend' passes the bytes it is given into the hashing
        //:   algorithm without truncation or appends for all types (with the
        //:   exceptions outlined above).
        //
        // Plan:
        //: 1 Call 'hashAppend' with each fundamental type to ensure the
        //:   function exists. (C-1)
        //:
        //: 2 Use a mock hashing algorithm to test that 'hashAppend' inputs the
        //:   same bytes when given -0.0 or +0.0 floating point numbers. (C-2)
        //:
        //: 3 Use a mock hashing algorithm to test that 'hashAppend' inputs one
        //:   of two possible byte representations of boolean values into the
        //:   hashing algorithm it is given. Attempt to permute the boolean
        //:   input using '++', assignment, and memcpy operations. (C-3)
        //:
        //: 4 Hash different pointers pointing to different data in different
        //:   locations, the same data in different locations, and the same
        //:   location, and verify all behave as expected. (C-4)
        //:
        //:5  Copy a known bitsequece into each fundamental type and pass it
        //:   into 'hashAppend' with a mocked hashing algorith. Verify that the
        //:   data inputted into the hashing algorithm matches the known input
        //:   bitsequence. (C-5)
        //
        // Testing:
        //   void hashAppend(HASHALG& hashAlg, bool input);
        //   void hashAppend(HASHALG& hashAlg, char input);
        //   void hashAppend(HASHALG& hashAlg, signed char input);
        //   void hashAppend(HASHALG& hashAlg, unsigned char input);
        //   void hashAppend(HASHALG& hashAlg, wchar_t input);
        //   void hashAppend(HASHALG& hashAlg, char16_t input);
        //   void hashAppend(HASHALG& hashAlg, char32_t input);
        //   void hashAppend(HASHALG& hashAlg, short input);
        //   void hashAppend(HASHALG& hashAlg, unsigned short input);
        //   void hashAppend(HASHALG& hashAlg, int input);
        //   void hashAppend(HASHALG& hashAlg, unsigned int input);
        //   void hashAppend(HASHALG& hashAlg, long input);
        //   void hashAppend(HASHALG& hashAlg, unsigned long input);
        //   void hashAppend(HASHALG& hashAlg, long long input);
        //   void hashAppend(HASHALG& hashAlg, unsigned long long input);
        //   void hashAppend(HASHALG& hashAlg, float input);
        //   void hashAppend(HASHALG& hashAlg, double input);
        //   void hashAppend(HASHALG& hashAlg, long double input);
        //   void hashAppend(HASHALG& hashAlg, const char (&input)[N]);
        //   void hashAppend(HASHALG& hashAlg, const TYPE (&input)[N]);
        //   void hashAppend(HASHALG& hashAlg, const void *input);
        //   void hashAppend(HASHALG& hashAlg, RT (*input)(ARGS...));
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'hashAppend'"
                            "\n====================\n");

        if (verbose) printf("Call 'hashAppend' with each fundamental type to"
                            " ensure the function exists. (C-1)\n");
        {
            MockHashingAlgorithm boolAlg;
            hashAppend(boolAlg, static_cast<bool>(1));

            MockHashingAlgorithm charAlg;
            hashAppend(charAlg, static_cast<char>(1));

            MockHashingAlgorithm signedCharAlg;
            hashAppend(signedCharAlg, static_cast<signed char>(1));

            MockHashingAlgorithm wchar_tAlg;
            hashAppend(wchar_tAlg, static_cast<wchar_t>(1));

#if defined BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES
            MockHashingAlgorithm char16_tAlg;
            hashAppend(char16_tAlg, static_cast<char16_t>(1));

            MockHashingAlgorithm char32_tAlg;
            hashAppend(char32_tAlg, static_cast<char32_t>(1));
#endif

            MockHashingAlgorithm shortAlg;
            hashAppend(shortAlg, static_cast<short>(1));

            MockHashingAlgorithm unsignedShortAlg;
            hashAppend(unsignedShortAlg, static_cast<unsigned short>(1));

            MockHashingAlgorithm intAlg;
            hashAppend(intAlg, static_cast<int>(1));

            MockHashingAlgorithm unsignedIntAlg;
            hashAppend(unsignedIntAlg, static_cast<unsigned int>(1));

            MockHashingAlgorithm longAlg;
            hashAppend(longAlg, static_cast<long>(1));

            MockHashingAlgorithm unsignedLongAlg;
            hashAppend(unsignedLongAlg, static_cast<unsigned long>(1));

            MockHashingAlgorithm longLongAlg;
            hashAppend(longLongAlg, static_cast<long long>(1));

            MockHashingAlgorithm unsignedLongLongAlg;
            hashAppend(unsignedLongLongAlg,static_cast<unsigned long long>(1));

            MockHashingAlgorithm floatAlg;
            hashAppend(floatAlg, static_cast<float>(1));

            MockHashingAlgorithm doubleAlg;
            hashAppend(doubleAlg, static_cast<double>(1));

            MockHashingAlgorithm longDoubleAlg;
            hashAppend(longDoubleAlg, static_cast<long double>(1));

            char carray[] = "asdf";
            MockHashingAlgorithm carrayAlg;
            hashAppend(carrayAlg, carray);

            const char constCarray[] = "asdf";
            MockHashingAlgorithm constCarrayAlg;
            hashAppend(constCarrayAlg, constCarray);

            int iarray[] = {1, 2, 3, 4};
            MockHashingAlgorithm iarrayAlg;
            hashAppend(iarrayAlg, iarray);

            const int constIarray[] = {1, 2, 3, 4};
            MockHashingAlgorithm constIarrayAlg;
            hashAppend(constIarrayAlg, constIarray);

            char literal[] = "asdf";
            char *ptr = literal;
            MockHashingAlgorithm ptrAlg;
            hashAppend(ptrAlg, ptr);

            char constLiteral[] = "asdf";
            const char *constPtr = constLiteral;
            MockHashingAlgorithm constPtrAlg;
            hashAppend(constPtrAlg, constPtr);

            void (*fnptr10)(int, int, int, int, int, int, int, int, int, int) =
                                                                             0;
            MockHashingAlgorithm fnptrAlg10;
            hashAppend(fnptrAlg10, fnptr10);

            void (*fnptr9)(int, int, int, int, int, int, int, int, int) = 0;
            MockHashingAlgorithm fnptrAlg9;
            hashAppend(fnptrAlg9, fnptr9);

            void (*fnptr8)(int, int, int, int, int, int, int, int) = 0;
            MockHashingAlgorithm fnptrAlg8;
            hashAppend(fnptrAlg8, fnptr8);

            void (*fnptr7)(int, int, int, int, int, int, int) = 0;
            MockHashingAlgorithm fnptrAlg7;
            hashAppend(fnptrAlg7, fnptr7);

            void (*fnptr6)(int, int, int, int, int, int) = 0;
            MockHashingAlgorithm fnptrAlg6;
            hashAppend(fnptrAlg6, fnptr6);

            void (*fnptr5)(int, int, int, int, int) = 0;
            MockHashingAlgorithm fnptrAlg5;
            hashAppend(fnptrAlg5, fnptr5);

            void (*fnptr4)(int, int, int, int) = 0;
            MockHashingAlgorithm fnptrAlg4;
            hashAppend(fnptrAlg4, fnptr4);

            void (*fnptr3)(int, int, int) = 0;
            MockHashingAlgorithm fnptrAlg3;
            hashAppend(fnptrAlg3, fnptr3);

            void (*fnptr2)(int, int) = 0;
            MockHashingAlgorithm fnptrAlg2;
            hashAppend(fnptrAlg2, fnptr2);

            void (*fnptr1)(int) = 0;
            MockHashingAlgorithm fnptrAlg1;
            hashAppend(fnptrAlg1, fnptr1);

            void (*fnptr0)() = 0;
            MockHashingAlgorithm fnptrAlg0;
            hashAppend(fnptrAlg0, fnptr0);
        }

        if (verbose) printf("Use a mock hashing algorithm to test that"
                            " 'hashAppend' inputs the same bytes when given"
                            " -0.0 or +0.0 floating point numbers. (C-2)\n");
        {
            TestDriver<float> floatDriver;
            floatDriver.testHashAppendNegativeZero();

            TestDriver<double> doubleDriver;
            doubleDriver.testHashAppendNegativeZero();

            TestDriver<long double> longDoubleDriver;
            longDoubleDriver.testHashAppendNegativeZero();
        }

        if (verbose) printf("Use a mock hashing algorithm to test that"
                            " 'hashAppend' inputs one of two possible byte"
                            " representations of boolean values into the"
                            " hashing algorithm it is given. Attempt to"
                            " permute the boolean input using '++',"
                            " assignment, and memcpy operations. (C-3)\n");
        {
            MockHashingAlgorithm defaultAlg;
            hashAppend(defaultAlg, true);

            bool incrementedBool = true;
            incrementedBool++;
            MockHashingAlgorithm incrementedAlg;
            hashAppend(incrementedAlg, incrementedBool);

            unsigned short uShort = 219;
            bool assignedBool = uShort;
            MockHashingAlgorithm assignedAlg;
            hashAppend(assignedAlg, assignedBool);

            // All various 'true's are the same
            ASSERT(binaryCompare(defaultAlg.getData(),
                                 incrementedAlg.getData(),
                                 sizeof(bool)));

            ASSERT(binaryCompare(defaultAlg.getData(),
                                 assignedAlg.getData(),
                                 sizeof(bool)));

            MockHashingAlgorithm falseAlg;
            hashAppend(falseAlg, false);

            // 'true' and 'false' are different
            ASSERT(!binaryCompare(defaultAlg.getData(),
                                  falseAlg.getData(),
                                  sizeof(bool)));
        }

        if (verbose) printf("Hash different pointers pointing to different"
                            " data in different locations, the same data in"
                            " different locations, and the same location, and"
                            " verify all behave as expected. (C-4)\n");
        {
            if (veryVerbose) printf("Testing object pointers\n");

            const char *ptr1Loc1Val1 = "asdf";
            const char *ptr2Loc1Val1 = ptr1Loc1Val1;

            // 'temp' is required to prevent 'ptr1Loc1Val1' and 'ptr3Loc2Val1'
            // from being optimized to point to the same location.
            char temp[] = "zxcv";
            const char *ptr3Loc2Val1 = temp;
            temp[0] = 'a';

            const char *ptr4Loc3Val2 = "qwer";

            MockHashingAlgorithm ptr1Loc1Val1Alg;
            hashAppend(ptr1Loc1Val1Alg, ptr1Loc1Val1);

            MockHashingAlgorithm ptr2Loc1Val1Alg;
            hashAppend(ptr2Loc1Val1Alg, ptr2Loc1Val1);

            MockHashingAlgorithm ptr3Loc2Val1Alg;
            hashAppend(ptr3Loc2Val1Alg, ptr3Loc2Val1);

            MockHashingAlgorithm ptr4Loc3Val2Alg;
            hashAppend(ptr4Loc3Val2Alg, ptr4Loc3Val2);

            // Correct length passed into the algorithm
            ASSERT(ptr1Loc1Val1Alg.getLength() == sizeof(const char *));
            ASSERT(ptr2Loc1Val1Alg.getLength() == sizeof(const char *));
            ASSERT(ptr3Loc2Val1Alg.getLength() == sizeof(const char *));
            ASSERT(ptr4Loc3Val2Alg.getLength() == sizeof(const char *));

            // Pointers to same location come out the same
            ASSERT(binaryCompare(ptr1Loc1Val1Alg.getData(),
                                 ptr2Loc1Val1Alg.getData(),
                                 sizeof(const char *)));

            // Pointers to same value, different location come out different
            ASSERT(!binaryCompare(ptr1Loc1Val1Alg.getData(),
                                  ptr3Loc2Val1Alg.getData(),
                                  sizeof(const char *)));

            // Pointers to different value and location come out different
            ASSERT(!binaryCompare(ptr1Loc1Val1Alg.getData(),
                                  ptr4Loc3Val2Alg.getData(),
                                  sizeof(const char *)));

            if (veryVerbose) printf("Testing function pointers\n");

            int (*fnptr1Loc1Val1)() = testFunction1;
            int (*fnptr2Loc1Val1)() = fnptr1Loc1Val1;
            int (*fnptr3Loc2Val1)() = testFunction2;
            int (*fnptr4Loc3Val2)() = testFunction3;

            MockHashingAlgorithm fnptr1Loc1Val1Alg;
            hashAppend(fnptr1Loc1Val1Alg, fnptr1Loc1Val1);

            MockHashingAlgorithm fnptr2Loc1Val1Alg;
            hashAppend(fnptr2Loc1Val1Alg, fnptr2Loc1Val1);

            MockHashingAlgorithm fnptr3Loc2Val1Alg;
            hashAppend(fnptr3Loc2Val1Alg, fnptr3Loc2Val1);

            MockHashingAlgorithm fnptr4Loc3Val2Alg;
            hashAppend(fnptr4Loc3Val2Alg, fnptr4Loc3Val2);

            // Correct length passed into the algorithm
            ASSERT(fnptr1Loc1Val1Alg.getLength() == sizeof(int (*)()));
            ASSERT(fnptr2Loc1Val1Alg.getLength() == sizeof(int (*)()));
            ASSERT(fnptr3Loc2Val1Alg.getLength() == sizeof(int (*)()));
            ASSERT(fnptr4Loc3Val2Alg.getLength() == sizeof(int (*)()));

            // Pointers to same location come out the same
            ASSERT(binaryCompare(fnptr1Loc1Val1Alg.getData(),
                                 fnptr2Loc1Val1Alg.getData(),
                                 sizeof(int (*)())));

            // Pointers to same value, different location come out different
            ASSERT(!binaryCompare(fnptr1Loc1Val1Alg.getData(),
                                  fnptr3Loc2Val1Alg.getData(),
                                  sizeof(int (*)())));

            // Pointers to different value and location come out different
            ASSERT(!binaryCompare(fnptr1Loc1Val1Alg.getData(),
                                  fnptr4Loc3Val2Alg.getData(),
                                  sizeof(int (*)())));

        }

        if (verbose) printf("Copy a known bitsequece into each fundamental"
                            " type and pass it into 'hashAppend' with a mocked"
                            " hashing algorith. Verify that the data inputted"
                            " into the hashing algorithm matches the known"
                            " input bitsequence. (C-5)\n");
        {
            // 'bool' has already been tested and we explicitly DO NOT want it
            // to preserve its bitwise representation.

            TestDriver<char> charDriver;
            charDriver.testHashAppendPassThrough(L_);

            TestDriver<signed char> signedCharDriver;
            signedCharDriver.testHashAppendPassThrough(L_);

            TestDriver<wchar_t> wchar_tDriver;
            wchar_tDriver.testHashAppendPassThrough(L_);

#if defined BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES
            TestDriver<char16_t> char16_tDriver;
            char16_tDriver.testHashAppendPassThrough(L_);

            TestDriver<char32_t> char32_tDriver;
            char32_tDriver.testHashAppendPassThrough(L_);
#endif

            TestDriver<short> shortDriver;
            shortDriver.testHashAppendPassThrough(L_);

            TestDriver<unsigned short> unsignedShortDriver;
            unsignedShortDriver.testHashAppendPassThrough(L_);

            TestDriver<int> intDriver;
            intDriver.testHashAppendPassThrough(L_);

            TestDriver<unsigned int> unsignedIntDriver;
            unsignedIntDriver.testHashAppendPassThrough(L_);

            TestDriver<long> longDriver;
            longDriver.testHashAppendPassThrough(L_);

            TestDriver<unsigned long> unsignedLongDriver;
            unsignedLongDriver.testHashAppendPassThrough(L_);

            TestDriver<long long> longLongDriver;
            longLongDriver.testHashAppendPassThrough(L_);

            TestDriver<unsigned long long> unsignedLongLongDriver;
            unsignedLongLongDriver.testHashAppendPassThrough(L_);

            TestDriver<float> floatDriver;
            floatDriver.testHashAppendPassThrough(L_);

            TestDriver<double> doubleDriver;
            doubleDriver.testHashAppendPassThrough(L_);

            TestDriver<long double> longDoubleDriver;
            longDoubleDriver.testHashAppendPassThrough(L_);

            // hashAppend char[]
            MockHashingAlgorithm carrayAlg;
            char carray[] = "asdf";
            size_t strLen = strlen(carray) + 1;
            hashAppend(carrayAlg, carray);
            const char *carrayOutput = carrayAlg.getData();
            for (size_t i = 0; i < strLen; ++i) {
                if (veryVerbose) printf("Asserting %hhu == %hhu\n",
                                        carrayOutput[i],
                                        carray[i]);
                ASSERT(carrayOutput[i] == carray[i]);
            }
            ASSERT(carrayAlg.getLength() == strLen);

            MockHashingAlgorithm constCarrayAlg;
            const char constCarray[] = "asdf";
            strLen = strlen(constCarray) + 1;
            hashAppend(constCarrayAlg, constCarray);
            const char *constCarrayOutput = constCarrayAlg.getData();
            for (size_t i = 0; i < strLen; ++i) {
                if (veryVerbose) printf("Asserting %hhu == %hhu\n",
                                       constCarrayOutput[i],
                                       constCarray[i]);
                ASSERT(constCarrayOutput[i] == constCarray[i]);
            }
            ASSERT(constCarrayAlg.getLength() == strLen);

            // hashAppend TYPE[]
            MockAccumulatingHashingAlgorithm iarrayAlg;
            int iarray[] = {1, 2, 3, 4};
            size_t iarrayLen = sizeof(iarray);
            const char *charIarray = reinterpret_cast<const char *>(iarray);
            hashAppend(iarrayAlg, iarray);
            const char *iarrayOutput = iarrayAlg.getData();
            for (size_t i = 0; i < iarrayLen; ++i) {
                if (veryVerbose) printf("Asserting %hhu == %hhu\n",
                                        iarrayOutput[i],
                                         charIarray[i]);
                ASSERT(iarrayOutput[i] == charIarray[i]);
            }
            ASSERT(iarrayAlg.getLength() == iarrayLen);

            MockAccumulatingHashingAlgorithm constIarrayAlg;
            const int constIarray[] = {1, 2, 3, 4};
            size_t constIarrayLen = sizeof(constIarray);
            const char *constCharIarray =
                                        reinterpret_cast<const char *>(iarray);
            hashAppend(constIarrayAlg, constIarray);
            const char *constIarrayOutput = constIarrayAlg.getData();
            for (size_t i = 0; i < constIarrayLen; ++i) {
                if (veryVerbose) printf("Asserting %hhu == %hhu\n",
                                       constIarrayOutput[i],
                                       constCharIarray[i]);
                ASSERT(constIarrayOutput[i] == constCharIarray[i]);
            }
            ASSERT(constIarrayAlg.getLength() == constIarrayLen);

            // hashAppend TYPE *
            TestDriver<char *> ptrDriver;
            ptrDriver.testHashAppendPassThrough(L_);

            TestDriver<const char *> constPtrDriver;
            constPtrDriver.testHashAppendPassThrough(L_);

            // hashAppend function pointers

            TestDriver<void (*)(int, int, int, int, int,
                                int, int, int, int, int)> fnptr10Driver;
            fnptr10Driver.testHashAppendPassThrough(L_);

            TestDriver<void (*)(int, int, int, int, int, int, int, int, int)>
                                                                  fnptr9Driver;
            fnptr9Driver.testHashAppendPassThrough(L_);

            TestDriver<void (*)(int, int, int, int, int, int, int, int)>
                                                                  fnptr8Driver;
            fnptr8Driver.testHashAppendPassThrough(L_);

            TestDriver<void (*)(int, int, int, int, int, int, int)>
                                                                  fnptr7Driver;
            fnptr7Driver.testHashAppendPassThrough(L_);

            TestDriver<void (*)(int, int, int, int, int, int)> fnptr6Driver;
            fnptr6Driver.testHashAppendPassThrough(L_);

            TestDriver<void (*)(int, int, int, int, int)> fnptr5Driver;
            fnptr5Driver.testHashAppendPassThrough(L_);

            TestDriver<void (*)(int, int, int, int)> fnptr4Driver;
            fnptr4Driver.testHashAppendPassThrough(L_);

            TestDriver<void (*)(int, int, int)> fnptr3Driver;
            fnptr3Driver.testHashAppendPassThrough(L_);

            TestDriver<void (*)(int, int)> fnptr2Driver;
            fnptr2Driver.testHashAppendPassThrough(L_);

            TestDriver<void (*)(int)> fnptr1Driver;
            fnptr1Driver.testHashAppendPassThrough(L_);

            TestDriver<void (*)()> fnptr0Driver;
            fnptr0Driver.testHashAppendPassThrough(L_);
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING IMPLICITLY DEFINED OPERATIONS
        //   Ensure that the four implicitly declared and defined special
        //   member functions are publicly callable and have no unexpected side
        //   effects.  As there is no observable state to inspect, there is
        //   little to verify other than that the expected expressions all
        //   compile.
        //
        // Concerns:
        //: 1 Objects can be created using the default constructor.
        //:
        //: 2 Objects can be created using the copy constructor.
        //:
        //: 3 The copy constructor is not declared as explicit.
        //:
        //: 4 Objects can be assigned to from constant objects.
        //:
        //: 5 Assignments operations can be chained.
        //:
        //: 6 Objects can be destroyed.
        //
        // Plan:
        //: 1 Create a default constructed 'Hash' and allow it to leave scope
        //:   to be destroyed. (C-1,6)
        //:
        //: 2 Use the copy-initialization syntax to create a new instance of
        //:   'Hash' from an existing instance. (C-2,3)
        //:
        //: 3 Assign the value of the one (const) instance of 'Hash' to a
        //:   second. (C-4)
        //:
        //: 4 Chain the assignment of the value of the one instance of 'Hash'
        //:   to a second instance of 'Hash', into a self-assignment of the
        //:   second object. (C-5)
        //
        // Testing:
        //   Hash()
        //   Hash(const Hash)
        //   ~Hash()
        //   Hash& operator=(const Hash&)
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING IMPLICITLY DEFINED OPERATIONS"
                   "\n=====================================\n");

        if (verbose) printf("Create a default constructed 'Hash' and allow it"
                            " to leave scope to be destroyed. (C-1,6)\n");
        {
            Obj alg1 = Obj();
        }

        if (verbose) printf("Use the copy-initialization syntax to create a"
                            " new instance of 'Hash' from an existing"
                            " instance. (C-2,3)\n");
        {
            const Obj alg1 = Obj();
            Obj alg2 = alg1;
        }

        if (verbose) printf("Assign the value of the one (const) instance of"
                            " 'Hash' to a second. (C-4)\n");
        {
            const Obj alg1 = Obj();
            Obj alg2 = alg1;
        }

        if (verbose) printf("Chain the assignment of the value of the one"
                            " instance of 'Hash' to a second instance of"
                            " 'Hash', into a self-assignment of the second"
                            " object. (C-5)\n");
        {
            const Obj alg1 = Obj();
            Obj alg2 = alg1;
            alg2 = alg2 = alg1;
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Create an instance of 'bslh::Hash<>'. (C-1)
        //:
        //: 2 Verify different hashes are produced for different ints. (C-1)
        //:
        //: 3 Verify the same hashes are produced for the same ints. (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        if (verbose) printf("Instantiate 'bslh::Hash'\n");
        {
            Obj hashAlg;
        }

        if (verbose) printf("Verify different hashes are produced for"
                            " different ints.\n");
        {
            Obj hashAlg;
            int int1 = 123456;
            int int2 = 654321;
            ASSERT(hashAlg(int1) != hashAlg(int2));
        }

        if (verbose) printf("Verify the same hashes are produced for the same"
                            " ints.\n");
        {
            Obj hashAlg;
            int int1 = 123456;
            int int2 = 123456;
            ASSERT(hashAlg(int1) == hashAlg(int2));
        }
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright (C) 2013 Bloomberg Finance L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
