#include <iterator>
#include <utility>
#include "Kmer.hpp"
#include "KmerIterator.hpp"


/* Note: That an iter is exhausted means that (iter._invalid == true) */

// use:  ++iter;
// pre:
// post: *iter is now exhausted
//       OR *iter is the next valid pair of kmer and location
KmerIterator& KmerIterator::operator++() {

    if (!invalid_) {

        const int pos_ = p_.second;

        if (s_[pos_ + Kmer::k] == 0) invalid_ = true;
        else find_next(pos_, pos_ + Kmer::k - 1, true);
    }

    return *this;
}


// use:  iter++;
// pre:
// post: iter has been incremented by one
KmerIterator KmerIterator::operator++(int) {

    KmerIterator tmp(*this);
    operator++();
    return tmp;
}

KmerIterator& KmerIterator::operator+=(const int length){

    const size_t next_pos = p_.second + length;

    while (!invalid_ && p_.second < next_pos) operator++();

    return *this;
}

// use:  val = (a == b);
// pre:
// post: (val == true) if a and b are both exhausted
//       OR a and b are in the same location of the same string.
//       (val == false) otherwise.
bool KmerIterator::operator==(const KmerIterator& o) {

    if (invalid_ || o.invalid_) return invalid_ && o.invalid_;
    return (s_ == o.s_) && (p_.second == o.p_.second);
}


// use:  p = *iter;
// pre:
// post: p is NULL or a pair of Kmer and int
std::pair<Kmer, int>& KmerIterator::operator*() {
    return p_;
}


// use:  example 1: km = iter->first;
//       example 2:  i = iter->second;
// pre:  *iter is not NULL
// post: km will be (*iter).first, i will be (*iter).second
std::pair<Kmer, int> *KmerIterator::operator->() {
    return &(operator*());
}


// use:  iter.raise(km, rep);
// post: iter has been incremented by one
//       if iter is not invalid, km is iter->first and rep is km.rep()
void KmerIterator::raise(Kmer& km, Kmer& rep) {

    operator++();

    if (!invalid_) {

        km = p_.first;
        rep = km.rep();
    }
}

// use:  find_next(i,j, last_valid);
// pre:
// post: *iter is either invalid or is a pair of:
//       1) the next valid kmer in the string that does not have any 'N'
//       2) the location of that kmer in the string
void KmerIterator::find_next(size_t i, size_t j, bool last_valid) {

    ++i;
    ++j;

    if (last_valid){

        const char c = s_[j] & 0xDF;

        if (c == 0) invalid_ = true;
        else if ((c != 'A') && (c != 'C') && (c != 'G') && (c != 'T')){

            i = ++j;
            last_valid = false;
        }
        else {

            p_.first.selfForwardBase(c);
            p_.second = i;
        }
    }

    if (!last_valid){

        while (s_[j] != 0) {

            const char c = s_[j] & 0xDF; // mask lowercase bit

            ++j;

            if ((c == 'A') || (c == 'C') || (c == 'G') || (c == 'T')) {

                if (i + Kmer::k == j) {

                    p_.first = Kmer(s_+i);
                    p_.second = i;

                    return;
                }
            }
            else i = j;
        }

        invalid_ = true;
    }
}
