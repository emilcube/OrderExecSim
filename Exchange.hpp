#pragma once

#include <boost/date_time/posix_time/posix_time.hpp>
#include <thread>
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include "Utils.hpp"
#include "config.hpp"
#include "Order.hpp"
#include "Orderbook.hpp"
#include "Orderbooks.hpp"
#include "ClientPos.hpp"
#include "Events.hpp"
#include "EvManager.hpp"
//#include "ExchangeHandler.hpp"

class Exchange
{
public:
	uint orderid_counter;
	vector<Order> orders;

	map<uint, int> order_client;
	map<int, ClientPos> client_pos;

	OrderBooks orderbooks;
	EvManager evman;
	//map <string, OrderBook> orderbooks;

	void Subscribe(const int client_id)
	{
		evman.isSub[client_id] = true;
	}
	void Unsubscribe(const int client_id)
	{
		evman.isSub[client_id] = false;
	}

	int GetClientId(const uint id)
	{
		return order_client[id];
	}

	std::string AddOrder(const Order& outer_order, const int client_id)
	{
		if (outer_order.size <= 0 || outer_order.limit_price <= 0)
			return R"({"success":"false","error":"price and size can be only a positive number"})";
		orders.push_back(outer_order);

		orders[orderid_counter].SetExchOrderId(orderid_counter);

		order_client.insert(make_pair(orderid_counter, client_id));
		evman.CreateOrder(client_id, orderid_counter);

		FillOrder(orderid_counter, client_id);

		++orderid_counter;
		return (R"({"success":"true","order_id":)" + (std::to_string(orderid_counter - 1)) + "}");

	}

	void FillOrder(const uint orderid, const int client_id)
	{
		Order& curr_order = orders[orderid];

		auto& orderbook = orderbooks.GetOrderBook(curr_order.instr);

		uint order_ob_id, ab_index = 1 - curr_order.Calc_ba_index();
		double past_ob_need_filled, past_curr_need_filled, limit_price_fill, delta;

		while (curr_order.NotFullyFilled())
		{
			if (orderbook.IsObNullSize(ab_index) || orderbook.HighLow(ab_index, curr_order.limit_price)) //&& curr_order.modified == 0
			{
				if (curr_order.tif != 0) // FOK or IOC
				{
					curr_order.SetCancel();
					break;
				}

				if (curr_order.NotFullyFilled())
				{
					orderbook.AddOrder(orderid, curr_order.limit_price, ab_index);
					curr_order.maker = 1;

					evman.Bba(client_id, curr_order.instr, orderbook.GetBba());
					evman.Ob(client_id, curr_order.instr, orderbook.ba_orders);
				}
				break;
			}
			order_ob_id = orderbook.ClosestInd(ab_index, curr_order.limit_price);
			Order& ob_order = orders[order_ob_id];
			limit_price_fill = (ab_index == 1 ? ob_order.limit_price : curr_order.limit_price);

			delta = ((curr_order.AmountNeedToFill() <= ob_order.AmountNeedToFill()) ? curr_order.AmountNeedToFill() : ob_order.AmountNeedToFill());

			//if (curr_order.tif == 1 && delta != curr_order.AmountNeedToFill()) // FOK
			//{	
			//	curr_order.SetCancel();
			//	break;
			//}
			if (curr_order.FOKstate(delta))
				break;

			ob_order.SetFilledSize(delta, limit_price_fill);
			curr_order.SetFilledSize(delta, limit_price_fill);

			evman.FillOrder(client_id, curr_order.GetExchOrderId(), delta, limit_price_fill, curr_order.maker);
			evman.FillOrder(order_client[ob_order.GetExchOrderId()], ob_order.GetExchOrderId(), delta, limit_price_fill, ob_order.maker);
			//std::cout << std::endl << curr_order.maker;
			//std::cout << std::endl << ob_order.maker << std::endl;

			ClientPos& cur_ord_pos = client_pos[order_client[curr_order.GetExchOrderId()]];
			ClientPos& ob_ord_pos = client_pos[order_client[ob_order.GetExchOrderId()]];

			client_pos[order_client[curr_order.GetExchOrderId()]].ClientPosChanges(curr_order.instr, curr_order.side, delta, limit_price_fill, ob_order.Comission(delta * limit_price_fill));
			ob_ord_pos.ClientPosChanges(ob_order.instr, ob_order.side, delta, limit_price_fill, ob_order.Comission(delta * limit_price_fill));

			evman.PosUpdate(client_id, curr_order.instr, cur_ord_pos.GetClientPos(curr_order.instr));
			evman.PosUpdate(order_client[ob_order.GetExchOrderId()], ob_order.instr, ob_ord_pos.GetClientPos(ob_order.instr));

			if (!ob_order.NotFullyFilled()) // if order in ob filled -> delete from ob
			{
				orderbook.DeleteOrder(ob_order.GetExchOrderId(), ab_index);

				evman.Bba(order_client[ob_order.GetExchOrderId()], ob_order.instr, orderbook.GetBba());
				evman.Ob(order_client[ob_order.GetExchOrderId()], ob_order.instr, orderbook.ba_orders);
			}

			//if (curr_order.tif == 2 && curr_order.AmountNeedToFill() != 0.0) // IOC
			//{
			//	curr_order.SetCancel();
			//	break;
			//}
			if (curr_order.IOCstate(delta))
				break;
			//cout << endl << ob_order.filled_size << " " << curr_order.filled_size << endl;
		}
	}

	std::string CancelOrder(const int client_id, cstring orderid)
	{
		stringstream errmes("");
		int ord_id = atoi(orderid.c_str());
		if (ord_id < 0 || ord_id >= orderid_counter)
		{
			errmes << "order not found";
			evman.CancelOrder(client_id, 0, errmes.str());
			return R"({"success":"false","error":"order not found"})";
		}

		Order& curr_order = orders[ord_id];
		uint ba_index = curr_order.Calc_ba_index();

		if (order_client[ord_id] != client_id)
		{
			errmes << "order not found";
		}
		else if (curr_order.IsActive())
		{
			curr_order.SetCancel();

			auto& orderbook = orderbooks.GetOrderBook(curr_order.instr);
			orderbook.DeleteOrder(ord_id, ba_index);
		}
		else
		{
			errmes << "order already cancelled or fully filled";
		}
		evman.CancelOrder(client_id, curr_order.GetExchOrderId(), errmes.str());
		if (errmes.str() == "")
			return R"({"success":"true"})";
		else
			return R"({"success":"false","error":")" + errmes.str() + R"("})";
	}

	string GetOrdId(const int client_id, uint client_order_id)
	{
		for (auto& ord : orders)
		{
			//Order& curr_order = ord;
			if (order_client[ord.GetExchOrderId()] == client_id && ord.client_order_id == client_order_id)
			{
				return ord.exch_order_id;
			}
		}
	}

	string QueryOrderInfo(const int client_id, cstring exch_ord_id) //uint client_order_id
	{
		//std::string ord = GetOrdId(client_id, client_order_id);
		int ord_id = atoi(exch_ord_id.c_str());
		if (ord_id < 0 || ord_id >= orderid_counter)
		{
			return R"({"success":"false","error":"order not found"})";
		}
		if (order_client[ord_id] != client_id)
		{
			return R"({"success":"false","error":"order not found"})";
		}
		return orders[stoi(exch_ord_id)].OrderInfoJson().str();
	}

	std::string ModifyOrder(const int client_id, cstring outer_order_id, const double& new_limit_price, const double& new_size)
	{
		stringstream errmes("");
		int ord_id = atoi(outer_order_id.c_str());
		if (ord_id < 0 || ord_id >= orderid_counter)
		{
			errmes << "order not found";
			evman.ModifyOrder(client_id, 0, new_limit_price, new_size, errmes.str());
			return R"({"success":"false","error":"order not found"})";
		}

		Order& curr_order = orders[ord_id];
		uint ba_index = curr_order.Calc_ba_index();

		auto instr = orders[ord_id].instr;

		if (order_client[ord_id] != client_id)
		{
			errmes << "order not found";
		}
		else if (orders[ord_id].IsActive() && orders[ord_id].filled_size <= new_size)
		{
			curr_order.limit_price = new_limit_price;
			curr_order.size = new_size;
			//curr_order.modified = 1;

			auto& orderbook = orderbooks.GetOrderBook(curr_order.instr);
			orderbook.DeleteOrder(ord_id, ba_index);
			//orderbook.AddOrder(ord_id, new_limit_price, 1 - ba_index);
			curr_order.maker = 0;
			FillOrder(ord_id, client_id);

		}
		else if (!orders[ord_id].IsActive())
		{
			errmes << "order cancelled or fully filled";
		}
		else if (orders[ord_id].filled_size > new_size)
		{
			errmes << "order filled size more than new size";
		}
		evman.ModifyOrder(client_id, curr_order.GetExchOrderId(), new_limit_price, new_size, errmes.str());
		if (errmes.str() == "")
			return R"({"success":"true"})";
		else
			return R"({"success":"false","error":")" + errmes.str() + R"("})";
	}

	void AccountBalances(const int client_id)
	{
		auto tmp = client_pos[client_id];
		tmp.PrintClientsPos();
	}

	std::string ActiveOrdersJson(const int client_id)
	{
		vector<uint> active_ord;
		for (auto& ord : orders) //order_client[ord_id] != client_id
		{
			if (order_client[ord.GetExchOrderId()] == client_id)
			{
				if (ord.IsActive())
				{
					active_ord.push_back(ord.GetExchOrderId());
				}
			}
		}

		stringstream tmp;
		tmp << "{\"active_orders\":[";
		for (int i = 0; i < active_ord.size(); i++)
		{
			tmp << active_ord[i];
			if (i != active_ord.size() - 1)
				tmp << ",";
		}
		tmp << "]}";
		return tmp.str();
	}

	std::string AccountBalancesJson(const int client_id)
	{
		auto tmp = client_pos[client_id];
		std::string balances = tmp.PrintClientsPosJson();

		stringstream temp;
		temp << "{\"balances\":[" << balances << "]}";
		return temp.str();
	}

	void PrintOrders()
	{
		std::cout << endl;
		std::cout << "client_order_id, instr, size, limit_price, side, type, tif, exch_submit_time, ";
		std::cout << "exch_receive_time, exch_order_id, filled_size, avg_fill_price, cancelled" << endl;
		for (auto& order_example : orders)
		{
			order_example.PrintOrder();
			//std::cout << std::endl;
		}
		std::cout << endl;
	}

	void PrintClientsPos()
	{
		ClientPos p;
		p.PrintClientsPos();

		std::cout << "clients positions" << std::endl;
		//map<int, ClientPos> client_pos;
		for (const auto& client : client_pos)
		{
			std::cout << "client " << client.first << " : ";

			ClientPos tmp2 = client.second;
			tmp2.PrintClientsPos();

			std::cout << '\n';
		}
	}

	Exchange() : orderid_counter(0) {}

	void PrintOrderbooks()
	{
		orderbooks.PrintObs();
	}

	//void SetBba(double& p, double& s, uint num)
	//{
	//	if (num == maxNum)
	//	{
	//		p = 0;
	//		s = 0;
	//	}
	//	else
	//	{
	//		p = orders[num].limit_price;
	//		s = orders[num].size;
	//	}
	//}

	//std::string GetBbaOld(const int client_id, cstring instr)
	//{
	//	auto& orderbook = orderbooks.GetOrderBook(instr);

	//	uint b, a;
	//	std::tie(b, a) = orderbook.GetSliceBba();

	//	std::stringstream tmp;

	//	double b_p, b_s, a_p, a_s;

	//	SetBba(b_p, b_s, b);
	//	SetBba(a_p, a_s, a);

	//	tmp << "{" << R"("bid_price":)" << b_p << ","
	//		<< R"("bid_size":)" << b_s << ","
	//		<< R"("ask_price":)" << a_p << ","
	//		<< R"("ask_size":)" << a_s << "}";

	//	return tmp.str();
	//}

	void SetBba(double& p, double& s, vector<uint> num)
	{
		if (num.size() == 1 && num[0] == maxNum)
		{
			p = 0;
			s = 0;
		}
		else
		{
			p = orders[num[0]].limit_price;
			s = 0;
			for (auto& obj : num)
			{
				//std::cout << num[o]
				s = s + orders[obj].size - orders[obj].filled_size;
			}
		}
	}

	std::string GetBba(const int client_id, cstring instr) //
	{
		auto& orderbook = orderbooks.GetOrderBook(instr);
		vector<uint> b, a;
		b = orderbook.GetSliceBids();
		a = orderbook.GetSliceAsks();
		std::stringstream tmp;
		double b_p, b_s, a_p, a_s;
		SetBba(b_p, b_s, b);
		SetBba(a_p, a_s, a);

		tmp << "{" << R"("bid_price":)" << std::setprecision(5) << std::fixed << b_p << "," //
			<< R"("bid_size":)" << b_s << ","
			<< R"("ask_price":)" << a_p << ","
			<< R"("ask_size":)" << a_s << "}";

		return tmp.str();
	}

	//string BAjson(vector<uint>& ba)
	//{
	//	stringstream tmp;

	//	if (ba.size() == 0)
	//		return "[0,0]";
	//	for (int i = 0; i < ba.size(); i++)
	//	{
	//		tmp << "[" << orders[ba[i]].limit_price << ","
	//			<< orders[ba[i]].size << "]";
	//		if (i != ba.size() - 1)
	//			tmp << ",";
	//	}
	//	return tmp.str();
	//}

	//std::string GetObOld(const int client_id, cstring instr)
	//{
	//	auto& orderbook = orderbooks.GetOrderBook(instr);
	//	vector<vector<uint>> ba_ob;
	//	ba_ob = orderbook.GetSliceOb();
	//	std::stringstream tmp;

	//	tmp << "{" << "\"asks\":[" << BAjson(ba_ob[1]) << "],"
	//		<< "\"bids\":[" << BAjson(ba_ob[0]) << "]}";

	//	return tmp.str();
	//}

	string BAjson(vector<vector<uint>>& ba)
	{
		stringstream tmp;

		if (ba.size() == 1 && ba[0][0] == maxNum)
			return "[0,0]";

		for (auto& i : ba)
		{
			double temp_size = 0.0;
			for (auto& j : i)
			{
				temp_size = temp_size + orders[j].size - orders[j].filled_size;
			}
			tmp << "[" << std::setprecision(5) << std::fixed << orders[i[0]].limit_price << "," //std::setprecision(5) << fixed 
				<< temp_size << "],";
		}
		std::string t = tmp.str();
		t.pop_back();
		return t;
	}

	std::string GetOb(const int client_id, cstring instr) // 
	{
		auto& orderbook = orderbooks.GetOrderBook(instr);
		vector<vector<uint>> b;
		vector<vector<uint>> a;

		b = orderbook.GetSliceObBids();
		a = orderbook.GetSliceObAsks();

		//ba_ob = orderbook.GetSliceOb();
		std::stringstream tmp;

		tmp << "{" << "\"asks\":[" << BAjson(a) << "],"
			<< "\"bids\":[" << BAjson(b) << "]}";

		return tmp.str();
	}

	std::string FillEvery(vector<string>& fillh)
	{
		stringstream tmp;
		if (fillh.size() == 0)
			return "";
		for (auto& obj : fillh)
		{
			auto res = splitStr(obj, " ");
			tmp << R"(["size":)" << std::setprecision(5) << std::fixed << res[0] << R"(,)"
				<< R"("price":)" << res[1] << R"(,)"
				<< R"("maker":)" << res[2] << R"(,)"
				<< R"("time":")" << res[3] + " " + res[4] << R"("],)";
		}
		std::string t = tmp.str();
		t.pop_back();
		return t;
	}

	std::string FillJson(const int client_id, cstring outer_order_id)
	{
		int ord_id = atoi(outer_order_id.c_str());
		if (ord_id < 0 || ord_id >= orderid_counter)
		{
			return R"({"success":"false","error":"order not found"})";
		}

		if (order_client[ord_id] != client_id)
		{
			return R"({"success":"false","error":"order not found"})";
		}

		auto fillh = allFill[ord_id];
		stringstream tmp;
		tmp << R"({"order id":)" << ord_id << ","
			<< R"("fill history":[)" << FillEvery(fillh)
			<< "]}";
		return tmp.str();
	}

};