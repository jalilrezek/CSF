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


std::vector<uint64_t> BigInt::addMagnitudes(const BigInt& rhs) const {

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

    for (int i = 0; i < largerSize; i++) { // left as "smallerSize" cuz need to convert to largerSize.
    // add zeros to the extra missing places of the smaller one, then just run through the loop as normal.
      uint64_t sum = 0;

      sum = lhsVal[i] + rhsVal[i] + overflow; // overflow is +1 if there was any overflow from before, else 0

      if (sum < lhsVal[i]) { // overflowed. Sufficient to check if greater than just one of lhs[i] or rhs[i] 
        sumVec.push_back(sum); // the overflow is sum mod 2^64, since uint64_t overflows to positive numbers from 0
        overflow = 1; // to be added to the next non-overflowing spot.
      } else {
        sumVec.push_back(sum);
        overflow = 0;
      }

    }
    if (overflow == 1) { // if still have overflow after finishing 
      sumVec.push_back(overflow); // in the largest digits place
    }


    return sumVec;
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
    // initializer_list

  } else if (!otherNegative && !isNegative) {
    //std::cout<<"lhs positive and rhs positive" << std::endl;
    return BigInt(addMagnitudes(rhs), false);

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
}

BigInt BigInt::operator<<(unsigned n) const
{
  // TODO: implement
}

BigInt BigInt::operator*(const BigInt &rhs) const
{
  // TODO: implement
}

BigInt BigInt::operator/(const BigInt &rhs) const
{
  // TODO: implement
}

int BigInt::compare(const BigInt &rhs) const
{
  // TODO: implement
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

