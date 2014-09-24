//
// request_handler.hpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2014 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_SERVER2_REQUEST_HANDLER_HPP
#define HTTP_SERVER2_REQUEST_HANDLER_HPP

#include <string>
#include <boost/noncopyable.hpp>
#include <boost/filesystem.hpp>
#include <vector>

#include <cctype>
#include <iomanip>
#include <sstream>
#include <string>
#include "global.h"
namespace http {
namespace server2 {

struct reply;
struct request;

/// The common handler for all incoming requests.
class request_handler
  : private boost::noncopyable
{
public:
  /// Construct with a directory containing files to be served.
  explicit request_handler(const std::string& doc_root);

  /// Handle a request and produce a reply.
  void handle_request(request& req, reply& rep);

private:
  /// The directory containing the files to be served.
  std::string doc_root_;

  /// Perform URL-decoding on a string. Returns false if the encoding was
  /// invalid.
  static bool url_decode(const std::string& in, std::string& out);

  //Create index.html file from directory content
  bool createIndex(const request& req, reply& rep);
  //Create download response for file
  bool createDownload(request& req, reply& rep);
  bool createUploadHtml(request& req, reply& rep);
  std::string url_encode(const std::string &value);
  std::vector<boost::filesystem::path> paths;
  std::string currentPath;
  
};

} // namespace server2
} // namespace http

#endif // HTTP_SERVER2_REQUEST_HANDLER_HPP
