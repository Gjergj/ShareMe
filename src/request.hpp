//
// request.hpp
// ~~~~~~~~~~~
//
// Copyright (c) 2003-2014 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_SERVER2_REQUEST_HPP
#define HTTP_SERVER2_REQUEST_HPP

#include <string>
#include <vector>
#include "header.hpp"

namespace http {
namespace server2 {

/// A request received from a client.
struct request
{
  std::string method;
  std::string uri;
  std::string fileFullPath;
  bool isFileDownload;
  int http_version_major;
  int http_version_minor;
  std::string decodedUri;
  std::vector<header> headers;
  long int downloadResumeFileDownloadOffset;
  bool downloadResume;
};

} // namespace server2
} // namespace http

#endif // HTTP_SERVER2_REQUEST_HPP
