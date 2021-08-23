/* asdf.cpp
* common/asdf - source file
* by Robert Babin (Oswell Whent <xbrot.rykath@xoxy.net>)
*
* ASDF Format - utility functions
*/

#include "asdf.hpp"

// --- binary --- //

void asdf::write_be_16(FILE* fptr, uint16_t input){
  // write big endian 16 bits to fptr
  uint8_t buffer = input >> 8u;
  fwrite(&buffer, sizeof(uint8_t), 1, fptr);
  buffer = input & 0xFFu;
  fwrite(&buffer, sizeof(uint8_t), 1, fptr);
}

void asdf::write_be_32(FILE* fptr, uint32_t input) {
  // write big endian 16 bits to fptr
  uint8_t buffer = input >> 24u;
  fwrite(&buffer, sizeof(uint8_t), 1, fptr);
  buffer = (input >> 16u) & 0xFFu;
  fwrite(&buffer, sizeof(uint8_t), 1, fptr);
  buffer = (input >> 8u) & 0xFFu;
  fwrite(&buffer, sizeof(uint8_t), 1, fptr);
  buffer = input & 0xFFu;
  fwrite(&buffer, sizeof(uint8_t), 1, fptr);
}

void asdf::write_be_64(FILE* fptr, uint64_t input){
  // write big endian 16 bits to fptr
  uint8_t buffer = input >> 56u;
  fwrite(&buffer, sizeof(uint8_t), 1, fptr);
  buffer = (input >> 48u) & 0xFFu;
  fwrite(&buffer, sizeof(uint8_t), 1, fptr);
  buffer = (input >> 40u) & 0xFFu;
  fwrite(&buffer, sizeof(uint8_t), 1, fptr);
  buffer = (input >> 32u) & 0xFFu;
  fwrite(&buffer, sizeof(uint8_t), 1, fptr);
  buffer = (input >> 24u) & 0xFFu;
  fwrite(&buffer, sizeof(uint8_t), 1, fptr);
  buffer = (input >> 16u) & 0xFFu;
  fwrite(&buffer, sizeof(uint8_t), 1, fptr);
  buffer = (input >> 8u) & 0xFFu;
  fwrite(&buffer, sizeof(uint8_t), 1, fptr);
  buffer = input & 0xFFu;
  fwrite(&buffer, sizeof(uint8_t), 1, fptr);
}

// --- basic raw ASDF --- //
void asdf::header(FILE* fptr){
  fprintf(fptr, "#ASDF 1.0.0\n#ASDF_STANDARD 1.3.0\n%%YAML 1.1\n%%TAG ! tag:stsci.edu:asdf/\n");
  fprintf(fptr, "--- !core/asdf-1.1.0\n");
}

void asdf::block_header(FILE* fptr, uint64_t size) {
  asdf::write_be_32(fptr, 0xd3424c4b); // block_magic_token
  asdf::write_be_16(fptr, 48); // header_size
  asdf::write_be_32(fptr, 0); // flags
  asdf::write_be_32(fptr, 0); // compression
  asdf::write_be_64(fptr, size); // allocated_size
  asdf::write_be_64(fptr, size); // used_size
  asdf::write_be_64(fptr, size); // data_size
  asdf::write_be_64(fptr, 0); // checksum[0]
  asdf::write_be_64(fptr, 0); // checksum[1]
}