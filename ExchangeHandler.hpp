#pragma once
//#include "config.hpp"
static Exchange ex;

const std::string all_comands = R"(
create_order 1397 1 BTC-USDT 0.1 45000 1 0 0
{"success":"true","order_id":0}
input params: command client_id client_order_id pair size price buy/sell(1/-1) limit/market(0/1) tif(0-GTC,1-FOK,2-IOC)

modify_order 1366 1 40001 0.51
{"success":"true"}
input params: command client_id client_order_id new_price new_size

cancel_order 137799 0
{"success":"true"}
input params: command client_id exchange_order_id

order_info 1397 0
{"client_order_id":1,"instr":BTC-USDT,"size":0.1,"limit_price":45000,"side":1,"type":0,"tif":0,"exch_submit_time":2021-Sep-27 09:44:41.524073,
"exch_submit_time":2021-Sep-27 09:44:41.524073,"exch_order_id":0,"filled_size":0,"avg_fill_price":0,"cancelled":0}
input params: command client_id exchange_order_id

active_orders 1397
{"active_orders":[0]}
input params: command client_id

account_balances 1397
{"balances":[]}
input params: command client_id

fill_history 1397 0
{"order id":0,"fill history":[]}
input params: command cliend_id exchange_order_id

get_bba 1397 BTC-USDT
{"bid_price":45000,"bid_size":0.1,"ask_price":0,"ask_size":0}
input params: command client_id pair

get_ob 1397 BTC-USDT
{"asks":[[0,0]],"bids":[[45000,0.1]]}
input params: command client_id pair
)";

const std::string accountBalances = "account_balances";
const std::string createOrder = "create_order";
const std::string cancelOrder = "cancel_order";
const std::string activeOrders = "active_orders";
const std::string fillHistory = "fill_history";
const std::string orderInfo = "order_info";
const std::string modifyOrder = "modify_order";
const std::string getBba = "get_bba";
const std::string getOb = "get_ob";

enum class COMMAND
{
	ACCOUNT_BALANCES,
	CREATE_ORDER,
	CANCEL_ORDER,
	ACTIVE_ORDERS,
	FILL_HISTORY,
	ORDER_INFO,
	MODIFY_ORDER,
	GET_BBA,
	GET_OB
};

std::map<std::string, COMMAND> commands =
{
	{accountBalances, COMMAND::ACCOUNT_BALANCES },
	{createOrder, COMMAND::CREATE_ORDER },
	{cancelOrder, COMMAND::CANCEL_ORDER },
	{activeOrders, COMMAND::ACTIVE_ORDERS},
	{fillHistory, COMMAND::FILL_HISTORY },
	{orderInfo, COMMAND::ORDER_INFO},
	{modifyOrder, COMMAND::MODIFY_ORDER},
	{getBba, COMMAND::GET_BBA},
	{getOb, COMMAND::GET_OB}
};

std::map<COMMAND, int> amount_arg_err =
{
	{COMMAND::ACCOUNT_BALANCES, 2},
	{COMMAND::CREATE_ORDER, 9},
	{COMMAND::CANCEL_ORDER, 3},
	{COMMAND::ACTIVE_ORDERS, 2},
	{COMMAND::FILL_HISTORY, 3},
	{COMMAND::ORDER_INFO, 3},
	{COMMAND::MODIFY_ORDER, 5},
	{COMMAND::GET_BBA, 3},
	{COMMAND::GET_OB, 3}
};

std::wstring s2ws(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

std::string ExchangeHandler(const string& data_)
{
	if (data_.size() == 0)
		return R"({"success":"false","error":"command not found"})";

	auto data = splitStr(data_, " ");
	string cmd = data[0];

	auto it = std::find_if(commands.begin(), commands.end(), [&cmd](const std::pair<std::string, COMMAND>& el) {
		return el.first == cmd;
	});
	if (cmd == std::string("ping"))
	{
		return R"({pong})";
	}
	else if (cmd == std::string("close"))
	{
		system("taskkill /F /IM OrderExecSimBot.exe"); // /T  /F 
		return R"({"success":"true","info":"bot is over"})";
	}
	else if (cmd == std::string("bot"))
	{
		string tmp;
		tmp = "<FILEPATH>\\OrderExecSimBot.exe " + data_;
		//system("<FILEPATH>\\OrderExecSimBot.exe bot 0.001 1.0 40000.0 50000.0");
		system(tmp.c_str());
		return R"({"success":"true","info":"bot is created"})";
	}
	else if (cmd == std::string("restart"))
	{
		string programName = "<FILEPATH>\\OrderExecSim.exe"; // 
		std::wstring stemp = s2ws(programName);
		LPCWSTR result = stemp.c_str();
		ShellExecute(0, 0, result, 0, 0, SW_SHOW); //Запускаем новую копию
		//Close(); //Закрываем старую
		exit(666);
	}
	else if (cmd == std::string("all_commands"))
	{
		return all_comands;
	}
	
	if (it == commands.end())
	{
		std::cout << cmd << " - command not found" << std::endl;
		return R"({"success":"false","error":"command not found"})";
	}

	if (data.size() != amount_arg_err[it->second])
	{
		cout << "incorrect amount of arguments" << endl;
		return R"({"success":"false","error":"incorrect amount of arguments"})";
	}

	int clid;
	try {
		clid = stoi(data[1]);
	}
	catch (...) {
		return R"({"success":"false","error":"incorrect parsing clid"})";
	}
	
	std::stringstream reply;
	reply << R"({"success":"true"})";

	switch (it->second)
	{
	case COMMAND::CREATE_ORDER:
	{
		ex.Subscribe(clid);
		boost::to_upper(data[3]);
		Order ord(stoi(data[2]), data[3], stod(data[4]), stod(data[5]), stoi(data[6]), stoi(data[7]), stoi(data[8]));
		reply.str(std::string());
		reply << ex.AddOrder(ord, clid);
		break;
	}
	case COMMAND::CANCEL_ORDER:
		ex.Subscribe(clid);
		reply.str(std::string());
		reply << ex.CancelOrder(clid, data[2]);
		break;
	case COMMAND::MODIFY_ORDER:
		ex.Subscribe(clid);
		reply.str(std::string());
		reply << ex.ModifyOrder(clid, data[2], stod(data[3]), stod(data[4]));
		break;
	case COMMAND::ACCOUNT_BALANCES:
		ex.Subscribe(clid);
		reply.str(std::string());
		reply << ex.AccountBalancesJson(clid);
		break;
	case COMMAND::ORDER_INFO:
		ex.Subscribe(clid);
		reply.str(std::string());
		reply << ex.QueryOrderInfo(clid, data[2]);
		break;
	case COMMAND::ACTIVE_ORDERS:
		reply.str(std::string());
		reply << ex.ActiveOrdersJson(clid);
		break;
	case COMMAND::FILL_HISTORY:
		reply.str(std::string());
		reply << ex.FillJson(clid, data[2]);
		break;
	case COMMAND::GET_BBA:
		boost::to_upper(data[2]);
		reply.str(std::string());
		reply << ex.GetBba(clid, data[2]);
		break;
	case COMMAND::GET_OB:
		boost::to_upper(data[2]);
		reply.str(std::string());
		reply << ex.GetOb(clid, data[2]);
		break;
	default:
		cout << "invalid type" << endl;
		exit(1);
		break;
	}
	return reply.str();
}