#pragma once

//#include "Order.hpp"
#define _CRT_SECURE_NO_WARNINGS
#include "Exchange.hpp"
//#include <chrono>
#include <thread>
#include <chrono>
#include <cstdio>
#include "httplib.h"
#include "stdlib.h"
#include <boost/asio.hpp> 
#include <iostream> 
#include "ExchangeHandler.hpp"

using namespace std;
using namespace boost::asio;
using namespace boost::asio::ip;
using namespace httplib;

void main()
{
	Server svr;

	if (!svr.is_valid()) {
		printf("server has an error...\n");
		return;
	}

	svr.Get("/", [=](const Request& /*req*/, Response& res) {
		res.set_redirect("/hi");
		});

	svr.Get("/hi", [](const Request& /*req*/, Response& res) {
		res.set_content("Hello from orderExSim!\n", "text/plain");
		});

	svr.Post("/exchange", [](const Request& req, Response& res) {
		std::cout << ">>>>>" << boost::posix_time::to_simple_string(boost::posix_time::microsec_clock::universal_time()) << ": " << req.body << std::endl;
		std::string reply = ExchangeHandler(req.body);
		res.set_content(reply, "text / plain"); // + "\n"

		std::cout << endl;
		});

	svr.Get("/stop",
		[&](const Request& /*req*/, Response& /*res*/) { svr.stop(); });

	svr.set_error_handler([](const Request& /*req*/, Response& res) {
		const char* fmt = "<p>Error Status: <span style='color:red;'>%d</span></p>";
		char buf[BUFSIZ];
		snprintf(buf, sizeof(buf), fmt, res.status);
		res.set_content(buf, "text/html");
		});

	svr.listen(orderExSim_ip, orderExSim_port);

	return;
}


