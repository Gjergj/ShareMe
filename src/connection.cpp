//
// connection.cpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2014 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "connection.hpp"
#include <vector>
#include <boost/bind.hpp>
#include "request_handler.hpp"
#include <iostream>
namespace http {
	namespace server2 {

		connection::connection(asio::io_service& io_service,
			request_handler& handler)
			: socket_(io_service),
			request_handler_(handler)
		{
		}

		asio::ip::tcp::socket& connection::socket()
		{
			return socket_;
		}

		void connection::start()
		{
			std::cout << socket_.remote_endpoint().address().to_string() << std::endl;
			//std::cout << socket_.remote_endpoint().port() << std::endl;

			socket_.async_read_some(asio::buffer(buffer_),
				boost::bind(&connection::handle_read, shared_from_this(),
				asio::placeholders::error,
				asio::placeholders::bytes_transferred));
		}

		void connection::handle_read(const asio::error_code& e,
			std::size_t bytes_transferred)
		{
			int totalWritenBytes = 0;
			if (!e)
			{
				request_.downloadResumeFileDownloadOffset = 0;
				request_.downloadResume = false;
				boost::tribool result;
				boost::tie(result, boost::tuples::ignore) = request_parser_.parse(
					request_, buffer_.data(), buffer_.data() + bytes_transferred);

				if (result)
				{
					int written = 0;
					request_handler_.handle_request(request_, reply_);
					if (reply_.isFileDownLoad)
					{
						//asio::write(socket_, reply_.to_buffers());
						written = socket_.write_some(reply_.to_buffers());
						
						std::ifstream is(request_.fileFullPath.c_str(), std::ios::in | std::ios::binary );
						if (!is)
						{
							reply_ = reply::stock_reply(reply::not_found);
						}
						std::cout << "Duke u shkaruar:" << request_.fileFullPath << std::endl;
						char buf[8192];
						memset(buf, 0, sizeof(buf));
						int read = 0;

						asio::error_code error;
						try{
							if (request_.downloadResume && (request_.downloadResumeFileDownloadOffset > 0))
								is.seekg(request_.downloadResumeFileDownloadOffset);
							while ((read = is.read(buf, sizeof(buf)).gcount()) > 0)
							{
								written = asio::write(socket_, asio::buffer(buf, read), error);
								if (error)
								{
									std::cout << "Transferimi u nderpre." << std::endl;
									std::cout << "Transferuar: " << (((float)totalWritenBytes / 1024) / 1024) << "MB" << std::endl;
									break;
								}
								totalWritenBytes += written;
							}
							std::cout << "Transferuar: " << (((float)totalWritenBytes / 1024) / 1024) << "MB" << std::endl;
							asio::error_code ignored_ec;
							socket_.shutdown(asio::ip::tcp::socket::shutdown_both);
						}
						catch (std::exception ex)
						{
							std::cout << "Transferimi u nderpre." << std::endl;
							std::cout << "Transferuar: " << (((float)totalWritenBytes / 1024) / 1024) << "MB" << std::endl;
							asio::error_code ignored_ec;
							socket_.shutdown(asio::ip::tcp::socket::shutdown_both);
						}
						
					}
					else{//not download, serve web html
					asio::async_write(socket_, reply_.to_buffers(),
						boost::bind(&connection::handle_write, shared_from_this(),
						asio::placeholders::error));
					}
				}
				else if (!result)
				{
					reply_ = reply::stock_reply(reply::bad_request);
					asio::async_write(socket_, reply_.to_buffers(),
						boost::bind(&connection::handle_write, shared_from_this(),
						asio::placeholders::error));
				}
				else
				{
					socket_.async_read_some(asio::buffer(buffer_),
						boost::bind(&connection::handle_read, shared_from_this(),
						asio::placeholders::error,
						asio::placeholders::bytes_transferred));
				}
			}

			// If an error occurs then no new asynchronous operations are started. This
			// means that all shared_ptr references to the connection object will
			// disappear and the object will be destroyed automatically after this
			// handler returns. The connection class's destructor closes the socket.
		}

		void connection::handle_write(const asio::error_code& e)
		{
			if (!e)
			{
				// Initiate graceful connection closure.
				asio::error_code ignored_ec;
				socket_.shutdown(asio::ip::tcp::socket::shutdown_both, ignored_ec);
			}

			// No new asynchronous operations are started. This means that all shared_ptr
			// references to the connection object will disappear and the object will be
			// destroyed automatically after this handler returns. The connection class's
			// destructor closes the socket.
		}

	} // namespace server2
} // namespace http
