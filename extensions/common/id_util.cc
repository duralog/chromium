// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "extensions/common/id_util.h"

#include "base/files/file_path.h"
#include "base/string_number_conversions.h"
#include "base/string_util.h"
#include "crypto/sha2.h"

namespace {

// Converts a normal hexadecimal string into the alphabet used by extensions.
// We use the characters 'a'-'p' instead of '0'-'f' to avoid ever having a
// completely numeric host, since some software interprets that as an IP
// address.
static void ConvertHexadecimalToIDAlphabet(std::string* id) {
  for (size_t i = 0; i < id->size(); ++i) {
    int val;
    if (base::HexStringToInt(base::StringPiece(id->begin() + i,
                                               id->begin() + i + 1),
                             &val)) {
      (*id)[i] = val + 'a';
    } else {
      (*id)[i] = 'a';
    }
  }
}

}  // namespace

namespace extensions {
namespace id_util {

// First 16 bytes of SHA256 hashed public key.
const size_t kIdSize = 16;

std::string GenerateId(const std::string& input) {
  uint8 hash[kIdSize];
  crypto::SHA256HashString(input, hash, sizeof(hash));
  std::string output = StringToLowerASCII(base::HexEncode(hash, sizeof(hash)));
  ConvertHexadecimalToIDAlphabet(&output);

  return output;
}

std::string GenerateIdForPath(const base::FilePath& path) {
  base::FilePath new_path = MaybeNormalizePath(path);
  std::string path_bytes =
      std::string(reinterpret_cast<const char*>(new_path.value().data()),
                  new_path.value().size() * sizeof(base::FilePath::CharType));
  return GenerateId(path_bytes);
}

base::FilePath MaybeNormalizePath(const base::FilePath& path) {
#if defined(OS_WIN)
  // Normalize any drive letter to upper-case. We do this for consistency with
  // net_utils::FilePathToFileURL(), which does the same thing, to make string
  // comparisons simpler.
  base::FilePath::StringType path_str = path.value();
  if (path_str.size() >= 2 && path_str[0] >= L'a' && path_str[0] <= L'z' &&
      path_str[1] == ':')
    path_str[0] += ('A' - 'a');

  return base::FilePath(path_str);
#else
  return path;
#endif
}

}  // namespace id_util
}  // namespace extensions
