
class ClientPos
{
public:
	map<string, double> pos;

	pair <double,double> GetClientPos(cstring instr)
	{
		//return pos[instr];
		auto all = splitStr(instr, "-");
		return {pos[all[0]], pos[all[1]]};
	}

	void ClientPosChanges(cstring instr, int side, double delta, double price, double comission)
	{
		//client_pos[order_client[stoi(ord.exch_order_id)]][ord.instr] += ord.side * delta;
		auto all = splitStr(instr, "-");
		
		pos[all[0]] += side * delta;  // u_tok - BTC
									  // if buy 2 BTC then pos -> old + 2
									  // if sell 2 BTC then pos -> old - 2
		pos[all[1]] += -side * price + comission; // f_tok - USDT
									  // 2 BTC for 32000 then pos -> old - 32000
									  // 32000 for 2 then pos -> old + 32000
	}

	void PrintClientsPos()
	{
		for (const auto& instr : pos)
		{
			cout << instr.first << ":" << instr.second << ", ";
		}
	}

	std::string PrintClientsPosJson()
	{
		std::stringstream tmp;
		//tmp << "[";
		int i = 0;
		for (const auto& instr : pos)
		{
			tmp << std::setprecision(5) << std::fixed << instr.first << ":" << instr.second;

			if (i != pos.size() - 1)
				tmp << ",";
			++i;
		}
		//tmp << "]";
		return tmp.str();
	}

};