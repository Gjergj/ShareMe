//
// request_handler.cpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2014 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "request_handler.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <boost/lexical_cast.hpp>
#include "mime_types.hpp"
#include "reply.hpp"
#include "request.hpp"
#include <iostream>
#include "configurations.h"
namespace http {
namespace server2 {
	
request_handler::request_handler(const std::string& doc_root)
  : doc_root_(doc_root)
{
}

void request_handler::handle_request(request& req, reply& rep)
{
	rep.isFileDownLoad = false;
	req.isFileDownload = false;
	

	
  // Decode url to path.
  std::string request_path;
  if (!url_decode(req.uri, request_path))
  {
    rep = reply::stock_reply(reply::bad_request);
    return;
  }
  req.decodedUri = request_path;
  // Request path must be absolute and not contain "..".
  if (request_path.empty() || request_path[0] != '/'
      || request_path.find("..") != std::string::npos)
  {
    rep = reply::stock_reply(reply::bad_request);
    return;
  }

  // If path ends in slash (i.e. is a directory) then add "index.html".
  if (request_path[request_path.size() - 1] == '/')
  {
    //request_path += "index.html";
	createIndex(req, rep); 
	return;
  }

  if (req.uri == "/upload.html")
  {
	  createUploadHtml(req, rep);
	  return;
  }
  if (req.uri == "/index.html")
  {
	  createIndex(req, rep);
	  return;
  }

  std::string uri;
  if (global::dragAndDropFileName.size() > 0) //qe te heqi / e pare
	  req.decodedUri.erase(0, 1);
  //check if requested file is in our list
  for (std::vector<boost::filesystem::path>::iterator iter = paths.begin(); iter != paths.end(); ++iter)
  {
	  uri = iter->generic_string();
	  uri.erase(0, currentPath.size());

	  if (uri == req.decodedUri)
	  {
		  req.isFileDownload = true;
		  req.fileFullPath = iter->string();
		  createDownload(req, rep); return;
	  }
  }

  createIndex(req, rep); return;

  //// Determine the file extension.
  //std::size_t last_slash_pos = request_path.find_last_of("/");
  //std::size_t last_dot_pos = request_path.find_last_of(".");
  //std::string extension;
  //if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos)
  //{
  //  extension = request_path.substr(last_dot_pos + 1);
  //}

  //// Open the file to send back.
  //std::string full_path = doc_root_ + request_path;
  //std::ifstream is(full_path.c_str(), std::ios::in | std::ios::binary);
  //if (!is)
  //{
  //  rep = reply::stock_reply(reply::not_found);
  //  return;
  //}

  //// Fill out the reply to be sent to the client.
  //rep.status = reply::ok;
  //char buf[512];
  //while (is.read(buf, sizeof(buf)).gcount() > 0)
  //  rep.content.append(buf, is.gcount());
  //rep.headers.resize(2);
  //rep.headers[0].name = "Content-Length";
  //rep.headers[0].value = boost::lexical_cast<std::string>(rep.content.size());
  //rep.headers[1].name = "Content-Type";
  //rep.headers[1].value = mime_types::extension_to_type(extension);
}
bool request_handler::createIndex(const request& req, reply& rep)
{
	int filesInQueue = 0;
	paths.clear();
	if (global::dragAndDropFileName.size() < 1)
	{
		currentPath = boost::filesystem::current_path().string();
		for (boost::filesystem::recursive_directory_iterator it = boost::filesystem::recursive_directory_iterator(currentPath); it != boost::filesystem::recursive_directory_iterator(); ++it)
		{
			if (!is_directory(it->path()))//do not print directories
			{
				paths.push_back(it->path());
				filesInQueue++;
			}
			if (filesInQueue > fileLimit)
				break;
		}
	}
	else { 
		for (std::vector<std::string>::iterator temp = global::dragAndDropFileName.begin(); temp != global::dragAndDropFileName.end();
            			++temp)
			paths.push_back(boost::filesystem::path(*temp));
	}
	rep.content.clear();
	rep.content = "";

	rep.content += "<HTML> <HEAD> <TITLE>ShareMe</TITLE> </HEAD> <BODY>";


	//std::string domain = "";
	//for (std::vector<http::server2::header>::const_iterator iter = req.headers.begin(); iter != req.headers.end(); ++iter)
	//	if (iter->name == "Host")
	//		domain = iter->value;
	std::string uri;
	for (std::vector<boost::filesystem::path>::iterator iter = paths.begin(); iter != paths.end(); ++iter)
	{
		uri = iter->generic_string();
		if (global::dragAndDropFileName.size() < 1)//perndyshe heq germen e pare (C)
			uri.erase(0, currentPath.size() + 1);

		rep.content += "<a href=\"" /*+ domain + "/"*/ + url_encode(uri) + "\">" + iter->filename().string() + "</a>  " + boost::lexical_cast<std::string> (((float)boost::filesystem::file_size(iter->string()) / 1024) / 1024) + "MB <br> ";
	}
	rep.content += "<br><br><br>ShareMe v" + std::string(version) + "<br>";
	rep.content += creator;
	rep.content += "<br>";
	rep.content += email;
	rep.content += "<br>";
	rep.content += "</BODY> </HTML>";
	rep.status = reply::ok;
	/*char buf[512];
	while (is.read(buf, sizeof(buf)).gcount() > 0)
		rep.content.append(buf, is.gcount());*/
	rep.headers.resize(2);
	rep.headers[0].name = "Content-Length";
	rep.headers[0].value = boost::lexical_cast<std::string>(rep.content.size());
	rep.headers[1].name = "Content-Type";
	rep.headers[1].value = mime_types::extension_to_type("html");

	return true;//todo kur nuk duhet te ktheje true
}

bool request_handler::createUploadHtml(request& req, reply& rep)
{
	rep.content.clear();
	rep.content = "";

	rep.content += "<HTML> <HEAD> <TITLE>ShareMe</TITLE> </HEAD> <BODY>";

	
		
		
	
	rep.content += "<form method = \"post\" action = \"upload\" name = \"submit\" enctype = \"multipart/form-data\">";
	rep.content += "<input type = \"file\" name = \"fileField[]\" multiple=\"multiple\"><br/><br/>";
	rep.content += "<input type = \"submit\" name = \"submit\" value = \"Submit\"> 	</form>";

	rep.content += "<br><br><br>ShareMe v" + std::string(version) + "<br>";
	rep.content += creator;
	rep.content += "<br>";
	rep.content += email;
	rep.content += "<br>";
	rep.content += "</BODY> </HTML>";
	rep.status = reply::ok;
	/*char buf[512];
	while (is.read(buf, sizeof(buf)).gcount() > 0)
	rep.content.append(buf, is.gcount());*/
	rep.headers.resize(2);
	rep.headers[0].name = "Content-Length";
	rep.headers[0].value = boost::lexical_cast<std::string>(rep.content.size());
	rep.headers[1].name = "Content-Type";
	rep.headers[1].value = mime_types::extension_to_type("html");

	return true;//todo kur nuk duhet te ktheje true
}

bool request_handler::createDownload(request& req, reply& rep)
{
	rep.content.clear();
	rep.content = "";

	/*std::ifstream is(req.fileFullPath.c_str(), std::ios::in | std::ios::binary);
	if (!is)
	{
		rep = reply::stock_reply(reply::not_found);
		return false;
	}
	char buf[512];
	while (is.read(buf, sizeof(buf)).gcount() > 0)
	rep.content.append(buf, is.gcount());*/
	//std::vector<boost::filesystem::path>::iterator iter = paths.begin(); iter != paths.end(); ++iter
	//std::vector<header> headers;
	for (std::vector<header>::iterator tempStr = req.headers.begin() ; tempStr != req.headers.end(); ++tempStr)
	{
		if (tempStr->name == "Range")
		{
			req.downloadResume = true;
			int start = tempStr->value.find_first_of("=") + 1;
			int end = tempStr->value.find_first_of("-");
			std::string sub = std::string(tempStr->value, start, end - start);
			req.downloadResumeFileDownloadOffset = atoi(sub.c_str());
			break;
		}
	}

	
	rep.isFileDownLoad = req.isFileDownload;
	
	rep.headers.resize(6);
	if (req.downloadResume)
	{
		rep.status = reply::partial_content;
		rep.headers[0].name = "Content-Length";
		rep.headers[0].value = std::to_string(boost::filesystem::file_size(req.fileFullPath) - req.downloadResumeFileDownloadOffset);
	}
	else
	{
		rep.status = reply::ok;
		rep.headers[0].name = "Content-Length";
		rep.headers[0].value = std::to_string(boost::filesystem::file_size(req.fileFullPath));
	}
	rep.headers[1].name = "Content-Type";
	rep.headers[1].value = "application/octet-stream"; //mime_types::extension_to_type(boost::filesystem::extension(req.fileFullPath).erase(0,1));

	rep.headers[2].name = "Content-Disposition";
	rep.headers[2].value = "attachment; filename=" + url_encode(boost::filesystem::path::path(req.fileFullPath).filename().string());
	
	rep.headers[3].name = "Content-Transfer-Encoding";
	rep.headers[3].value = "binary";
	rep.headers[4].name = "Connection";
	rep.headers[4].value = "keep - alive";

	rep.headers[5].name = "Accept-Range";
	rep.headers[5].value = "bytes";
	if (req.downloadResume)
	{
		header resumeHeader;
		resumeHeader.name = "Content - Range";
		resumeHeader.value = "bytes " + boost::lexical_cast<std::string>(req.downloadResumeFileDownloadOffset) + " - " + std::to_string(boost::filesystem::file_size(req.fileFullPath) - req.downloadResumeFileDownloadOffset)
			+ " / " + std::to_string(boost::filesystem::file_size(req.fileFullPath));
		rep.headers.push_back(resumeHeader);
	}
	return true;
}

bool request_handler::url_decode(const std::string& in, std::string& out)
{
  out.clear();
  out.reserve(in.size());
  for (std::size_t i = 0; i < in.size(); ++i)
  {
    if (in[i] == '%')
    {
      if (i + 3 <= in.size())
      {
        int value = 0;
        std::istringstream is(in.substr(i + 1, 2));
        if (is >> std::hex >> value)
        {
          out += static_cast<char>(value);
          i += 2;
        }
        else
        {
          return false;
        }
      }
      else
      {
        return false;
      }
    }
    else if (in[i] == '+')
    {
      out += ' ';
    }
    else
    {
      out += in[i];
    }
  }
  return true;
}

std::string request_handler::url_encode(const std::string &value) {
	std::ostringstream escaped;
	escaped.fill('0');
	escaped << std::hex;

	for (std::string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
		std::string::value_type c = (*i);

		// Keep alphanumeric and other accepted characters intact
		if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
			escaped << c;
			continue;
		}

		// Any other characters are percent-encoded
		escaped << '%' << std::setw(2) << int((unsigned char)c);
	}

	return escaped.str();
}

} // namespace server2
} // namespace http
