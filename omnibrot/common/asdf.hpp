/* asdf.hpp
 * common/asdf - header file
 * by Robert Babin (Oswell Whent <xbrot.rykath@xoxy.net>)
 *
 * ASDF Format - utility functions
 */

#ifndef OMNIBROT_ASDF_HPP
#define OMNIBROT_ASDF_HPP

#include <cstdio>
#include <cstdint>

namespace asdf {

  // --- binary --- //
  void write_be_16(FILE*, uint16_t);
  void write_be_32(FILE*, uint32_t);
  void write_be_64(FILE*, uint64_t);

  // --- basic raw ASDF --- //
  void header(FILE*);
  void block_header(FILE*, uint64_t);


}
#endif //OMNIBROT_ASDF_HPP
