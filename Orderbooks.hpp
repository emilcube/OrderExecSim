
class OrderBooks : OrderBook
	{
	public:

		map <string, OrderBook> obs;

		OrderBook& GetOrderBook(cstring instr)
		{
			return obs[instr];
		}

		void PrintObs()
		{
			cout << endl;
			for (const auto& ev : obs)
			{
				auto ob = ev.second;
				if (ob.ba_orders[0].size() == 0 && ob.ba_orders[1].size() == 0)
					continue;
				std::cout << ev.first << endl;
				ob.PrintOb();
			}
		}
	};