
struct EvManager
{
	std::map<int, std::shared_ptr<ExchEv>> subs;
	std::map<int, bool> isSub;

	void AddSub(const int id, std::shared_ptr<ExchEv> sub)
	{
		subs[id] = sub;
	}

	void SendMsg()
	{
		for (auto it = subs.begin(); it != subs.end(); ++it)
		{
			//if (val % 2 == 0 && it->first == 2) continue;
			cout << it->first << endl;
			it->second->Mes();
		}

	}

	void SendMsg(const int id_)
	{
		cout << id_ << ",";
		subs[id_]->Mes();
	}

	void CreateOrder(const int clid_, const uint ord_id)
	{
		if (!isSub[clid_])
			return;
		std::shared_ptr<ExchEv> createlEv = std::make_shared<CreateOrderEvent>(ord_id);
		AddSub(clid_, createlEv);
		SendMsg(clid_);
	}

	void CancelOrder(const int clid_, const uint ord_id, cstring errmes = "")
	{
		if (!isSub[clid_])
			return;
		std::shared_ptr<ExchEv> cancelEv = std::make_shared<CancelOrderEvent>(ord_id, errmes);
		//static_pointer_cast<CancelOrderEvent>(cancelEv)->SuccessCancel();
		AddSub(clid_, cancelEv);
		SendMsg(clid_);
	}

	void ModifyOrder(const int clid_, const uint ord_id, const double new_price_, const double new_size_ ,cstring errmes = "")
	{
		if (!isSub[clid_])
			return;
		std::shared_ptr<ExchEv> modifylEv = std::make_shared<ModifyOrderEvent>(ord_id, new_price_, new_size_, errmes); 
		AddSub(clid_, modifylEv);
		SendMsg(clid_);
	}

	void FillOrder(const int clid_, const uint ord_id, const double filled_size_, const double filled_price_, const uint maker_)
	{
		if (!isSub[clid_])
			return;
		std::shared_ptr<ExchEv> fillEv = std::make_shared<FillOrderEvent>(ord_id, filled_size_, filled_price_, maker_); 
		AddSub(clid_, fillEv);
		SendMsg(clid_);
	}

	void Bba(const int clid_, cstring instr, const pair<double,double> bba_)
	{
		if (!isSub[clid_])
			return;
		std::shared_ptr<ExchEv> bbaEv = std::make_shared<BbaEvent>(instr, bba_);
		AddSub(clid_, bbaEv);
		SendMsg(clid_);
	}

	void Ob(const int clid_, cstring instr, const vector <multimap<double, uint>> ba_)
	{
		if (!isSub[clid_])
			return;
		std::shared_ptr<ExchEv> obEv = std::make_shared<ObEvent>(instr, ba_);
		AddSub(clid_, obEv);
		SendMsg(clid_);
	}

	void PosUpdate(const int clid_, cstring instr, const pair<double,double> pos_)
	{
		if (!isSub[clid_])
			return;
		std::shared_ptr<ExchEv> posupdateEv = std::make_shared<PosUpdateEvent>(pos_, instr);
		AddSub(clid_, posupdateEv);
		SendMsg(clid_);
	}

};