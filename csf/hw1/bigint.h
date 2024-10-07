#ifndef BIGINT_H
#define BIGINT_H

#include <initializer_list>
#include <vector>
#include <string>
#include <cstdint>

//! @file
//! Arbitrary-precision integer data type.

//! Class representing an arbitrary-precision integer represented as a bit string
//! (implemented using a vector of `uint64_t` elements) and a boolean flag
//! to record whether or not the value is negative.
class BigInt {
private:
  // TODO: add fields
  std::vector<uint64_t> value;
  bool isNegative;

public:
  //! Default constructor.
  //! The initialized BigInt value should be equal to 0.
  BigInt();

  //! Constructor from a `uint64_t` value and (optionally) a boolean
  //! indicating whether the value is negative.
  //!
  //! @param val a `uint64_t` value indicating the magnitude of the
  //!            BigInt value
  //! @param negative if true, the value is negative
  BigInt(uint64_t val, bool negative = false);

  //! Constructor from an `std::initializer_list` of `uint64_t` values
  //! to initialize the BigInt object's bit string, and (optionally)
  //! a boolean value indicating whether the value is negative.
  //!
  //! @param vals `std::initializer_list` of `uint64_t` values,
  //!             in order from less-significant to more-significant
  //! @param negative if true, the value is negative
  BigInt(std::initializer_list<uint64_t> vals, bool negative = false);


  //! constructor taking in a vector not an initializer_list
  //!
  //! @param vals a vector of uint64_t values
  //! negative: An optional parameter to indicate the sign of the BigInt
  BigInt(std::vector<uint64_t> vals, bool negative = false);


  //! Copy constructor.
  //!
  //! @param other another BigInt object that this object should be made
  //!              identical to
  BigInt(const BigInt &other);

  //! Destructor.
  ~BigInt();

  //! Assignment operator.
  //!
  //! @param rhs another BigInt object that this object should be made
  //!            identical to
  BigInt &operator=(const BigInt &rhs);

  //! Check whether value is negative.
  //!
  //! @return true if the value is negative, false otherwise
  bool is_negative() const;

  //! Return a const reference to the underlying vector of
  //! `uint64_t` values representing the bits of the magnitude of the
  //! overall BigInt value. Note that the values should be in
  //! "little endian" order: element 0 is the lowest 64 bits,
  //! eleemnt 1 is the next-lowest 64 bits, etc. In theory,
  //! all you should need to do is return a reference to the
  //! internal vector the BigInt object keeps its magnitude bits in.
  //!
  //! @return const reference to the vector containing the bit string values
  //!         (element at index has the least-significant 64 bits, etc.)
  const std::vector<uint64_t> &get_bit_vector() const;

  //! Get one `uint64_t` chunk of the overall bit string.
  //! Note that this function should work correctly regardless of the
  //! index passed in. If the index passed in is greater than the index
  //! of any explicit element in the vector containing the bit string,
  //! it should return 0.
  //!
  //! @param index the index of the `uint64_t` value to retrieve (0 indicates
  //!              the least-significant 64 bits, etc.)
  //! @return the `uint64_t` value containing the requested bits (0 if
  //!         the index is outside the bounds of the internal vector
  //!         containing the bit string)
  uint64_t get_bits(unsigned index) const;

  //! Addition operator.
  //!
  //! @param rhs the right-hand side BigInt value (the left hand value
  //!            is the implicit receiver object, i.e., `*this`)
  //! @return the BigInt value representing the sum of the operands
  BigInt operator+(const BigInt &rhs) const;

  //! Subtraction operator.
  //!
  //! @param rhs the right-hand side BigInt value (the left hand value
  //!            is the implicit receiver object, i.e., `*this`)
  //! @return the BigInt value representing the difference of the operands
  BigInt operator-(const BigInt &rhs) const;

  //! Unary negation operator.
  //!
  //! @return the BigInt value representing the negation of this
  //!         BigInt value
  BigInt operator-() const;

  //! Test whether a specific bit in the bit string is set to 1.
  //!
  //! @param n the bit to test (0 for the least significant bit, etc.)
  //! @return true if bit `n` is set to 1, false if it is set to 0
  bool is_bit_set(unsigned n) const;

  //! Left shift by n bits. Note that it is only allowed
  //! to use this operation on non-negative values.
  //! An `std::invalid_argument` exception is thrown if
  //! this BigInt is negative.
  //!
  //! @param n number of bits to shift left by
  //! @return BigInt value representing the result of shifting this]
  //!         value left by `n` bits
  //! @throw std::invalid_argument if this object represents a negative value
  BigInt operator<<(unsigned n) const;

  //! Multiplication operator.
  //!
  //! @param rhs the right-hand side BigInt value (the left hand value
  //!            is the implicit receiver object, i.e., `*this`)
  //! @return the BigInt value representing the product of the operands
  BigInt operator*(const BigInt &rhs) const;

  //! Division operator.
  //! Note that since BigInt objects represent integers, this
  //! operator should return a quotient value with the largest
  //! magnitude such that the magnitude of the product of the right-hand
  //! value and the quotient is less than or equal to the
  //! dividend (the left-hand object.) In simpler terms, the
  //! magnitude of the quotient should be truncated (not rounded).
  //!
  //! Some examples to illustrate:
  //! - `5 / 2 = 2`
  //! - `-5 / 2 = -2`
  //! - `5 / -2 = -2`
  //! - `-5 / -2 = 2`
  //!
  //! @param rhs the right-hand side BigInt value (the left hand value
  //!            is the implicit receiver object, i.e., `*this`)
  //! @return the quotient resulting from dividing the left hand
  //!         BigInt by the right-hand BigInt
  //! @throw std::invalid_argument if the right hand object is
  //!        equal to 0
  BigInt operator/(const BigInt &rhs) const;

  //! Compare two BigInt values, returning
  //!   - negative if lhs < rhs
  //!   - 0 if lhs = rhs
  //!   - positive if lhs > rhs
  //!
  //! @param rhs the right-hand side BigInt value (the left hand value
  //!            is the implicit receiver object, i.e., `*this`)
  //! @return the result of the comparison (negative means less,
  //!         0 means equal, positive means greater)
  int compare(const BigInt &rhs) const;

  // comparison operators: you won't need to modify these,
  // since they're all implemented using compare
  bool operator==(const BigInt &rhs) const { return compare(rhs) == 0; }
  bool operator!=(const BigInt &rhs) const { return compare(rhs) != 0; }
  bool operator<(const BigInt &rhs) const  { return compare(rhs) < 0; }
  bool operator<=(const BigInt &rhs) const { return compare(rhs) <= 0; }
  bool operator>(const BigInt &rhs) const  { return compare(rhs) > 0; }
  bool operator>=(const BigInt &rhs) const { return compare(rhs) >= 0; }

  //! Return a string representing the value of this BigInt, in
  //! lower-case hexadecimal (base-16). Note that there should be a leading
  //! minus sign (`-`) if this value is negative.
  //!
  //! @return the value of this BigInt object in hexadecimal
  std::string to_hex() const;

  //! Return a string representing the value of this BigInt, in
  //! decimal (base-10). Note that there should be a leading
  //! minus sign (`-`) if this value is negative.
  //!
  //! @return the value of this BigInt object in decimal (base-10)
  std::string to_dec() const;

private:
  // TODO: add helper functions

  int compare_magnitude(const BigInt &lhs, const BigInt &rhs) const; //Tayseer built this, as I was debugging
  // I chose to stick to my functions since I understood them better.
  BigInt add_magnitude(const BigInt &lhs, const BigInt &rhs) const; //Tayseer built this. Same reasoning as above.

  //! Compares the magnitudes (absolute values) of this BigInt and the given BigInt `rhs`.
  //! This function is used for comparisons where the sign of the BigInt is irrelevant, 
  //! focusing purely on the size of the absolute values.
  //!
  //! @param rhs the BigInt to compare against
  //! @return 1 if this BigInt has a greater magnitude, -1 if rhs has a greater magnitude,
  //!         and 0 if the magnitudes are equal.
  int thisGreaterMagThanOther(const BigInt& rhs) const;

  //! Adds the magnitudes (absolute values) of this BigInt and the given BigInt `rhs`.
  //! The result is stored as a vector of 64-bit chunks, with no regard for the sign of either value.
  //! This is useful for operations where the result needs to consider magnitude without handling sign directly.
  //!
  //! @param rhs the BigInt to add
  //! @return a vector of uint64_t representing the sum of the magnitudes of the two BigInt values
  std::vector<uint64_t> addMagnitudes(const BigInt& rhs) const;

  //! Subtracts the magnitude (absolute value) of the given BigInt `rhs` from the magnitude of this BigInt.
  //! Assumes that this BigInt has a greater or equal magnitude compared to `rhs`.
  //! Like the `addMagnitudes` function, this function operates on the absolute values of the BigInts.
  //! The result is stored as a vector of 64-bit chunks, and the caller is responsible for determining the final sign of the result.
  //!
  //! @param rhs the BigInt to subtract from this BigInt
  //! @return a vector of uint64_t representing the difference between the magnitudes of the two BigInt values
  std::vector<uint64_t> subtractMagnitudes(const BigInt& rhs) const;
};

#endif // BIGINT_H
