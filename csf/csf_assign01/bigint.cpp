#include <cassert>
#include "bigint.h"
#include <sstream> // for stringstream in to_hex()
#include <iomanip>
#include <iostream>
#include <cstdlib> 

BigInt::BigInt()
  // TODO: initialize member variables
{
  value.push_back(0);
  isNegative = false;
}

BigInt::BigInt(uint64_t val, bool negative)
  // TODO: initialize member variables
{
  value.push_back(val);
  isNegative = negative;
}

BigInt::BigInt(std::initializer_list<uint64_t> vals, bool negative)
  // TODO: initialize member variables
{
  value = vals;
  isNegative = negative;
}

BigInt::BigInt(const BigInt &other)
  // TODO: initialize member variables
{
  value = other.value;
  isNegative = other.isNegative;

}

BigInt::BigInt(std::vector<uint64_t> vals, bool negative) {
  value = vals;
  isNegative = negative;
}

BigInt::~BigInt()
{
}

BigInt &BigInt::operator=(const BigInt &rhs)
{
  // TODO: implement
  if (this != &rhs) {
    value = rhs.value;
    isNegative = rhs.isNegative;
  }

    return *this;

}

bool BigInt::is_negative() const
{
  // TODO: implement
  return isNegative == true;
}

const std::vector<uint64_t> &BigInt::get_bit_vector() const {
  // TODO: implement
  return this->value;
}

uint64_t BigInt::get_bits(unsigned index) const
{
  // TODO: implement
  if (index >= value.size()) {
    return 0;
  } else {
    return value[index];
  }
}


/*std::vector<uint64_t> BigInt::addMagnitudes(const BigInt& rhs) const {

    std::vector<uint64_t> lhsVal = value; // in case need to push back 0 as seen below.
    std::vector<uint64_t> rhsVal = rhs.value; // don't want to edit the other BigInt either.
    uint64_t overflow = 0;
    std::vector<uint64_t> sumVec;
    auto largerSize = lhsVal > rhsVal ? lhsVal.size() : rhsVal.size();

    // below code's purpose is to add 0s if necessary to the one with fewer digits to simplify the addition.
    if (largerSize == lhsVal.size()) { 
      for (int i = rhsVal.size(); i < largerSize; i++) {
        rhsVal.push_back(0);
      }
    } else {
      for (int i = lhsVal.size(); i < largerSize; i++) {
        lhsVal.push_back(0); // don't want to do this to actual bit string, so do it to a copy.
      }
    }

    for (int i = 0; i < largerSize; i++) { 
    // add zeros to the extra missing places of the smaller one, then just run through the loop as normal.
      uint64_t sum = lhsVal[i] + rhsVal[i] + overflow; // overflow is +1 if there was any overflow from before, else 0

      if (sum < lhsVal[i] || sum < rhsVal[i]) {// overflowed. Check both. 
        sumVec.push_back(sum); // the overflow is sum mod 2^64, since uint64_t overflows to positive numbers from 0
        //overflow = 1; // to be added to the next non-overflowing spot.
      } else {
        sumVec.push_back(sum);
        //overflow = 0;
      }
      overflow = (sum < lhsVal[i] || sum < rhsVal[i] || overflow > 0) ? 1 : 0;


    }
    if (overflow == 1) { // if still have overflow after finishing 
      sumVec.push_back(overflow); // in the largest digits place
    }


    return sumVec;
} */

std::vector<uint64_t> BigInt::addMagnitudes(const BigInt& rhs) const {
    std::vector<uint64_t> lhsVal = value;
    std::vector<uint64_t> rhsVal = rhs.value;
    std::vector<uint64_t> sumVec;
    
    // Determine the larger size between lhsVal and rhsVal
    size_t largerSize = std::max(lhsVal.size(), rhsVal.size());

    // Pad the shorter vector with zeros
    if (lhsVal.size() < largerSize) {
        lhsVal.resize(largerSize, 0);
    }
    if (rhsVal.size() < largerSize) {
        rhsVal.resize(largerSize, 0);
    }

    uint64_t overflow = 0;

    for (size_t i = 0; i < largerSize; i++) {
        uint64_t sum = lhsVal[i] + rhsVal[i] + overflow;
        overflow = (sum < lhsVal[i] || sum < rhsVal[i]) ? 1 : 0;
        sumVec.push_back(sum);
    }

    // If there is any overflow left after the last addition
    if (overflow > 0) {
        sumVec.push_back(overflow);
    }

    return sumVec;
}



BigInt BigInt::add_magnitude(const BigInt &lhs, const BigInt &rhs) const {
    BigInt result;
    result.isNegative = false; // This function only handles magnitudes, so result is non-negative

    size_t max_size = std::max(lhs.value.size(), rhs.value.size());
    result.value.resize(max_size, 0);

    uint64_t carry = 0;
    for (size_t i = 0; i < max_size; ++i) {
        uint64_t lhs_value = (i < lhs.value.size()) ? lhs.value[i] : 0;
        uint64_t rhs_value = (i < rhs.value.size()) ? rhs.value[i] : 0;

        // Sum the values and add carry from the previous iteration
        uint64_t total = lhs_value + rhs_value + carry;

        // Check for overflow and set carry
        if (total < lhs_value || total < rhs_value) {
            carry = 1;
        } else {
            carry = 0;
        }

        result.value[i] = total;
    }

    // If there is a remaining carry, append it to the result
    if (carry > 0) {
        result.value.push_back(carry);
    }

    return result;
}

int BigInt::thisGreaterMagThanOther(const BigInt& rhs) const {

  auto leftSize = this->value.size();
  auto rightSize = rhs.value.size();

  if (leftSize > rightSize) {
    return 1; // left is bigger

  } else if (rightSize > leftSize) {
    return -1; // right is bigger

  } else { // sizes are equal. Check which is bigger starting from biggest place value to smallest.
    for (int placeValue = value.size(); placeValue >= 0; placeValue--) {
      if (this->get_bits(placeValue) > rhs.get_bits(placeValue)) {
        return 1; // left ("this") is bigger
      } else if (rhs.get_bits(placeValue) > this->get_bits(placeValue)) {
        return -1; // right is bigger
      }
    }
    return 0; // placeValue went through all place values, down to the first one (placeValue = 0), and all are equal.
    // therefore the 2 bit strings are equal.
  }

}

std::vector<uint64_t> BigInt::subtractMagnitudes(const BigInt& rhs) const {
    std::vector<uint64_t> diffVec;
    auto largerMag = value;
    auto smallerMag = rhs.value;

    // Ensure both vectors are the same size
    while (smallerMag.size() < largerMag.size()) {
        smallerMag.push_back(0);
    }

    bool borrow = false;
    for (size_t i = 0; i < largerMag.size(); ++i) {
        uint64_t diff = largerMag[i] - smallerMag[i] - (borrow ? 1 : 0);
        if (largerMag[i] < smallerMag[i] + (borrow ? 1 : 0)) {
            diff = UINT64_MAX - (smallerMag[i] + (borrow ? 1 : 0) - largerMag[i]) + 1;
            borrow = true;
        } else {
            borrow = false;
        }
        diffVec.push_back(diff);
    }
    // Remove leading zeros
    while (diffVec.size() > 1 && diffVec.back() == 0) {
        diffVec.pop_back();
    }

    return diffVec;
}



BigInt BigInt::operator+(const BigInt &rhs) const
{
  // TODO: implement

  bool otherNegative = rhs.isNegative;


  if (otherNegative && isNegative) {
    //std::cout<<"lhs negative and rhs negative";
    return BigInt(addMagnitudes(rhs), true); // constructor needed that takes in a vector not 
      //return add_magnitude(*this, rhs);
    // initializer_list

  } else if (!otherNegative && !isNegative) {
    //std::cout<<"lhs positive and rhs positive" << std::endl;
    return BigInt(addMagnitudes(rhs), false);
    //return add_magnitude(*this, rhs);

  } else if (isNegative && !otherNegative) {
    //std::cout<<"lhs negative and rhs positive";

    int thisBiggerThanOther = thisGreaterMagThanOther(rhs);

    if (thisBiggerThanOther == 1) { // "this" has greater magnitude, and "this" neg while "rhs" pos. val - rhs.val
      //std::cout<<"left has greater magnitude" << std::endl;
      return BigInt(subtractMagnitudes(rhs), true); // result will be negative

    } else if (thisBiggerThanOther == -1) { // "rhs" has greater magnitude, and "rhs" pos while "this" neg. rhs.val - val
      //std::cout<<"right has greater magnitude";
      // gonna call rhs.subtractMagnitudes. From perspective of rhs, rhs is the "left" and
      // "this" is the right. 
      return BigInt(rhs.subtractMagnitudes(*this), false); // positive result.
    } else { // thisBiggerThanOther == 0 that is the magnitudes are equal and they are oppositely signed.
      //std::cout<<"Left and right have equal magnitudes";
      return BigInt(0); 
    }

  } else { // !isNegative && otherNegative
    //std::cout<<"lhs positive and rhs negative" << std::endl;
    int thisBiggerThanOther = thisGreaterMagThanOther(rhs);

    if (thisBiggerThanOther == 1) { // "this" has greater magnitude, and "this" pos while "rhs" neg. val - rhs.val
      //std::cout<<"'this' has greater magnitude" << std::endl;
      return BigInt(subtractMagnitudes(rhs), false); // result will be positive
    } else if (thisBiggerThanOther == -1) { // "rhs" has greater magnitude, and "rhs" neg while "this" pos. rhs.val - val
      //std::cout<<"right has greater magnitude";
      return BigInt(rhs.subtractMagnitudes(*this), true); // negative result.
    } else { // thisBiggerThanOther == 0 that is the magnitudes are equal and they are oppositely signed.
      //std::cout<<"thisBigIntIsBiggerThanOther == 0 that is the magnitudes are equal";
      return BigInt(0); 
    }
  
  }

  
}

BigInt BigInt::operator-(const BigInt &rhs) const
{
  // TODO: implement
  // Hint: a - b could be computed as a + -b
  
  BigInt negRhs = -rhs;
  return (*this + negRhs); // return type of "+" operator is a BigInt
}

BigInt BigInt::operator-() const
{
  // TODO: implement
  BigInt result(*this);
  bool checkIfZero = (value.size() == 1 && value[0] == 0);

  if (!checkIfZero) {
    result.isNegative = !isNegative;
  } else { 
    result.isNegative = false; // zero mustn't be counted as negative
  }

  return result;


  /*BigInt result(*this);
  result.isNegative = !isNegative;
  return result;*/
}

bool BigInt::is_bit_set(unsigned n) const
{
  // TODO: implement
  // find which 64-bit chunk the bit belongs to
  unsigned chunk_pos = n / 64;
  if (chunk_pos >= value.size()) {
    // this means that the bit is not set
    return false;
  }

  // find bit position inside the chunk
  unsigned bit_pos = n % 64;

  // check if the bit is set at the correct index
  return (value[chunk_pos] & (1ULL << bit_pos)) != 0;
}

/*BigInt BigInt::operator<<(unsigned n) const
{
  // TODO: implement
  BigInt result = *this;
  
  // find out how many full 64-bit chunks we would have to shift
  unsigned number_of_chunks = n / 64;
  unsigned bit_shift = n % 64;

  // add zeros at the least-significant positions
  if (number_of_chunks > 0) {
      // add zeros to the beginning of the vector
      result.value.insert(result.value.begin(), number_of_chunks, 0);
  }

  if (bit_shift > 0) {
      uint64_t carry = 0;
      for (size_t i = 0; i < result.value.size(); ++i) {
          uint64_t second_carry = result.value[i] >> (64 - bit_shift); // get the bits that overflow
          result.value[i] = (result.value[i] << bit_shift) | second_carry; // shift and add carry from before 
          carry = second_carry;
      }
      
      if (carry > 0) {
          result.value.push_back(carry);
      }
  }

  return result;
} */

BigInt BigInt::operator<<(unsigned n) const
{
  if (isNegative) {
    throw std::invalid_argument("Cannot apply left shift to negative value");
  }
  // Create a copy of the current BigInt for the result
  BigInt result = *this;

  // Calculate the number of full 64-bit chunks to shift
  unsigned number_of_chunks = n / 64;
  unsigned bit_shift = n % 64;

  // Add zeros at the least-significant positions for chunk shifting
  if (number_of_chunks > 0) {
      // Insert zeros at the beginning to shift left by the number of chunks
      result.value.insert(result.value.begin(), number_of_chunks, 0);
  }

  // Handle the bit shifting within chunks
  if (bit_shift > 0) {
      uint64_t carry = 0;  // Initialize the carry variable for shifted bits
      for (size_t i = 0; i < result.value.size(); ++i) {
          uint64_t new_carry = result.value[i] >> (64 - bit_shift); // Get the bits that will overflow to the next chunk
          result.value[i] = (result.value[i] << bit_shift) | carry; // Shift current chunk left and add the previous carry
          carry = new_carry; // Update carry for the next iteration
      }

      // If there's still a carry left after the loop, push it as a new chunk
      if (carry > 0) {
          result.value.push_back(carry);
      }
  }

  return result;
}


BigInt BigInt::operator*(const BigInt &rhs) const
{
  // TODO: implement
  
  bool resultIsNegative = (this->isNegative != rhs.isNegative);

  /*if (this->is_zero() || rhs.is_zero()) { // need to define "is_zero"
      return BigInt(0, false);
  }*/

  BigInt result(0, false);

  // continue with the function here (pretty sure we have to iterate and add using operator+)

} 

BigInt BigInt::operator/(const BigInt &rhs) const
{
  // TODO: implement
  // Check for division by zero
  /*if (rhs.is_zero()) {
      throw std::overflow_error("Division by zero");
  }

  // If the left-hand side is zero, return zero    
  if (this->is_zero()) {
      return BigInt(0, false);
  }*/

  // Handle the sign of the result
  bool resultIsNegative = (this->isNegative != rhs.isNegative);

  // Use magnitudes for the binary search (ignore signs during computation)
  BigInt lhs_abs = this->isNegative ? -(*this) : *this;
  BigInt rhs_abs = rhs.isNegative ? -rhs : rhs;

  // continue working at this point - most likely gotta do binary search 
} 

int BigInt::compare(const BigInt &rhs) const
{
  // TODO: implement
  // check for different signs 
    if (this->isNegative && !rhs.isNegative) {
        return -1;  // negative < positive
    }
    if (!this->isNegative && rhs.isNegative) {
        return 1;   // positive > negative
    }

    // if both are non-negative or negative, compare magnitudes
    int magnitude_compare = compare_magnitude(*this, rhs);

    // if both are positive or negative
    if (this->isNegative) {
        // for negative numbers, reverse the result
        return -magnitude_compare;
    } else {
        return magnitude_compare;
    }
}

// helper function 
int BigInt::compare_magnitude(const BigInt &lhs, const BigInt &rhs) const {
    // compare the # of elements in the bit vectors (larger vectors = larger values)
    if (lhs.value.size() > rhs.value.size()) {
        return 1;  // lhs is larger
    }
    if (lhs.value.size() < rhs.value.size()) {
        return -1; // rhs is larger 
    }

    // for same size vectors, compare element by element from most to least significant
    for (int i = lhs.value.size() - 1; i >= 0; --i) {
        if (lhs.value[i] > rhs.value[i]) {
            return 1;
        }
        if (lhs.value[i] < rhs.value[i]) {
            return -1;
        }
    }

    // this indicates the magnitudes are equal
    return 0;
}

std::string BigInt::to_hex() const
{
  // TODO: implement
  if (value.empty()) {
        return "0";
  }

  std::stringstream s;


  if (isNegative) { // make sure negative numbers have the - sign 
        s << "-";
  }
  s << std::hex << std::setfill('0'); // output numbers in hexadecimal and fill empty spots with leading zeros
  bool haveReachedNonzeroDigit = false;
   

  for (auto it = value.rbegin(); it != value.rend(); ++it) {

      if (*it != 0 && !haveReachedNonzeroDigit) {
        haveReachedNonzeroDigit = true;
        s << *it; // for the first digit, you do not want to pad with leading zeros.
        continue; 
      }

      if (haveReachedNonzeroDigit) {
        s << std::setw(16) << *it; // for digits besides the first digit, you do want to pad with leading zeros.
      }

  }
  // if all zeros, return "0"
  // otherwise, return the hexadecimal representation 
 
  return haveReachedNonzeroDigit ? s.str() : "0";

}

std::string BigInt::to_dec() const
{
  // TODO: implement
}

