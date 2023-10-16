
template<typename K, typename V>
void erase_a(std::multimap<K, V>& mapOfElemen, V value)
{
	int totalDeletedElements = 0;
	auto it = mapOfElemen.begin();
	while (it != mapOfElemen.end())
	{
		if (it->second == value)
		{
			it = mapOfElemen.erase(it);
			return;
		}
		else
			it++;
	}
	return;
}

class OrderBook
{	
public:
	//string instr;
	vector< multimap<double, uint> > ba_orders;
	//vector< map<uint, double> > ba_sizes;

	void AddOrder(const uint ord_id, const double price, const uint ab_index)
	{
		// if buy (ab_index==1) then order added in bid_orderbook
		ba_orders[1 - ab_index].insert(make_pair(price, ord_id));
	}
	
	//template<typename K, typename V>
	//void erase_b(std::multimap<K, V>& mapOfElemen, V value)
	//{
	//	int totalDeletedElements = 0;
	//	auto it = mapOfElemen.rbegin();
	//	while (it != mapOfElemen.rend())
	//	{
	//		if (it->second == value)
	//		{
	//			it = mapOfElemen.erase(it);
	//			return;
	//		}
	//		else
	//			it++;
	//	}
	//	return;
	//}

	void DeleteOrder(const uint ord_id, const uint ab_index)
	{
		erase_a(this->ba_orders[ab_index], ord_id);

		//if (ab_index == 0)
		//	erase_a(this->ba_orders[ab_index], ord_id);
		//else
		//	erase_b(this->ba_orders[ab_index], ord_id);
	}

	bool IsObNullSize(const uint ab_index)
	{
		if (ba_orders[ab_index].size() == 0) // if buy (ab_index==1) then check ask_orderbook
		{
			//std::cout << "null size" << std::endl;
			return true;
		}
		else
		{
			//std::cout << "not null size" << std::endl;
			return false;
		}
	}

	uint ClosestInd(const uint ab_index, const double price)
	{
		if (ab_index == 1) // 0
		{
			//cout << "sell " << ba_orders[ab_index].begin()->second << endl;
			return ba_orders[ab_index].begin()->second;

		}
		else
		{
			
			//cout << "buy " << it_old->second << endl;

			//if (ba_orders[0].size() == 1)
			//	return el->second;

			//for (auto it = (++it_old); it != ba_orders[0].rend(); it++)
			//{
			//	if (it->first != it_old->first)
			//	{
			//		//cout << "buy " << it_old->second << endl;
			//		return it_old->second;
			//	}
			//	it_old = it;
			//}
			//return it_old->second;

			auto el = ba_orders[0].rbegin();
			uint num = el->second;
			double pric = el->first;
			for (auto& i : boost::adaptors::reverse(ba_orders[0]))
			{
				if (num == i.second)
					continue;
				if (pric == i.first)
				{
					num = i.second;
				}
				else
				{
					return num;
				}
			}
			return num;


			//vector<uint> b;
			////auto it_old = ba_orders[0].rbegin();

			//if (ba_orders[0].size() <= 0)
			//{
			//	b.push_back(maxNum);
			//	return b;
			//}

			//auto el = ba_orders[0].rbegin();
			//b.push_back(el->second);
			//for (auto& i : boost::adaptors::reverse(ba_orders[0]))
			//{
			//	if (el->second == i.second)
			//		continue;
			//	if (el->first == i.first)
			//	{
			//		b.push_back(i.second);
			//	}
			//	else
			//	{
			//		return b;
			//	}
			//}
			//return b;
			
		}
	}

	bool HighLow(const uint ab_index, const double price)
	{
		if (ab_index == 1)
			return (price < ba_orders[1].begin()->first);
		else
			return (price > ba_orders[0].rbegin()->first);

		//return (ab_index == 1 ? price < ba_orders[1].begin()->first // if try to buy ---> and price is too high
		//	: price > ba_orders[0].rbegin()->first); // if try to sell ---> and price is too low
	}

	pair<double, double> GetBba()
	{
		double tmp1 = ba_orders[0].size() > 0 ? ba_orders[0].rbegin()->first : 0.0;
		double tmp2 = ba_orders[1].size() > 0 ? ba_orders[1].begin()->first : 0.0;
		return { tmp1, tmp2 };
	}

	pair<uint, uint> GetSliceBba()
	{
		uint tmp1 = ba_orders[0].size() > 0 ? ba_orders[0].rbegin()->second : maxNum;
		uint tmp2 = ba_orders[1].size() > 0 ? ba_orders[1].begin()->second : maxNum;
		return { tmp1, tmp2 };
	}

	vector<uint> GetSliceBids()
	{
		vector<uint> b;
		//auto it_old = ba_orders[0].rbegin();

		if (ba_orders[0].size() <= 0)
		{
			b.push_back(maxNum);
			return b;
		}
		
		auto el = ba_orders[0].rbegin();
		b.push_back(el->second);
		for (auto& i : boost::adaptors::reverse(ba_orders[0]))
		{
			if (el->second == i.second)
				continue;
			if (el->first == i.first)
			{
				b.push_back(i.second);
			}
			else
			{
				return b;
			}
		}
		return b;
		//b.push_back(it_old->second);
		//if (ba_orders[0].size() == 1)
		//{
		//	return b;
		//}

		//for (auto it = (++it_old); it != ba_orders[0].rend(); it++)
		//{
		//	if (it->first != it_old->first)
		//	{
		//		return b;
		//	}
		//	b.push_back(it_old->second);
		//	it_old = it;
		//}
	}

	vector<uint> GetSliceAsks()
	{
		vector<uint> a;
		//auto it_old = ba_orders[1].begin();

		if (ba_orders[1].size() <= 0)
		{
			a.push_back(maxNum);
			return a;
		}

		auto el = ba_orders[1].begin();
		a.push_back(el->second);
		for (auto& i : ba_orders[1])
		{
			if (el->second == i.second)
				continue;
			if (el->first == i.first)
			{
				a.push_back(i.second);
			}
			else
			{
				return a;
			}
		}
		return a;
		//if (ba_orders[1].size() == 1)
		//{
		//	a.push_back(it_old->second);
		//	return a;
		//}

		//for (auto it = (++it_old); it != ba_orders[1].end(); it++)
		//{
		//	if (it->first != it_old->first)
		//	{
		//		return a;
		//	}
		//	a.push_back(it_old->second);
		//	it_old = it;
		//}
	}

	void PrintOb()
	{
		//std::cout << std::endl;
		//multimap<double, uint>::iterator it;

		for (auto it = ba_orders[1].rbegin(); it != ba_orders[1].rend(); it++)
			std::cout << "ask " << std::setprecision(5) << std::fixed << it->first << ' ' << it->second << '\n';

		for (auto it = ba_orders[0].rbegin(); it != ba_orders[0].rend(); it++)
			std::cout << "bid " << std::setprecision(5) << std::fixed << it->first << ' ' << it->second << '\n';
		cout << endl;
	}

	vector<vector<uint>> GetSliceOb()
	{
		vector<vector<uint>> ob;
		ob.resize(2);

		for (auto it = ba_orders[1].begin(); it != ba_orders[1].end(); it++)
			ob[1].push_back(it->second);

		for (auto it = ba_orders[0].rbegin(); it != ba_orders[0].rend(); it++)
			ob[0].push_back(it->second);
		return ob;
	}

	vector<vector<uint>> GetSliceObBids()
	{
		vector<vector<uint>> b;

		vector<uint> tmp;

		if (ba_orders[0].size() <= 0)
		{
			tmp.push_back(maxNum);
			b.push_back(tmp);
			return b;
		}

		auto el_price = ba_orders[0].rbegin()->first;
		auto el_num = ba_orders[0].rbegin()->second;
		for (auto& i : boost::adaptors::reverse(ba_orders[0]))
		{
			if (el_num == i.second)
			{
				tmp.push_back(i.second);
				continue;
			}
			if (el_price == i.first)
			{
				tmp.push_back(i.second);
				//b.push_back(i.second);
			}
			else
			{
				//el_price = i.first;
				//el_num = i.second;
				//b.push_back(tmp);
				//tmp.clear();

				b.push_back(tmp);
				el_price = i.first;
				el_num = i.second;
				tmp.clear();

				tmp.push_back(el_num);
				
			}
		}
		b.push_back(tmp);
		return b;
	}

	vector<vector<uint>> GetSliceObAsks()
	{
		vector<vector<uint>> a;

		vector<uint> tmp;

		if (ba_orders[1].size() <= 0)
		{
			tmp.push_back(maxNum);
			a.push_back(tmp);
			return a;
		}

		auto el_price = ba_orders[1].begin()->first;
		auto el_num = ba_orders[1].begin()->second;
		for (auto& i : ba_orders[1])
		{
			if (el_num == i.second)
			{
				tmp.push_back(i.second);
				continue;
			}
			if (el_price == i.first)
			{
				tmp.push_back(i.second);
				//b.push_back(i.second);
			}
			else
			{
				a.push_back(tmp);
				el_price = i.first;
				el_num = i.second;
				tmp.clear();

				tmp.push_back(el_num);
			}
		}
		a.push_back(tmp);
		return a;
	}

	OrderBook() : ba_orders(2) {} //
};