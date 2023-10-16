
enum class typeEvent
{
	CreateOrder,
	CancelOrder,
	ModifyOrder,
	FillOrder,
	Bba,
	Ob,
	PosUpdate
};

std::string GetTypeEvent(const typeEvent ev)
{
	std::stringstream mes;
	switch (ev)
	{
	case typeEvent::CreateOrder:
		mes << "CreateOrder";
		break;
	case typeEvent::CancelOrder:
		mes << "CancelOrder";
		break;
	case typeEvent::ModifyOrder:
		mes << "ModifyOrder";
		break;
	case typeEvent::FillOrder:
		mes << "FillOrder";
		break;
	case typeEvent::Bba:
		mes << "Bba";
		break;
	case typeEvent::Ob:
		mes << "Ob";
		break;
	case typeEvent::PosUpdate:
		mes << "PosUpdate";
		break;
	default:
		break;
	}
	return mes.str();
}

struct ExchEv
{
	typeEvent type; // 0 - CreateOrder, 1 - CancelOrder, 2 - ModifyOrder, 3 - FillOrder, 4 - BbaEvent, 5 - ObEvent, 6 - PosUpdateEvent
	uint req_id;
	uint exch_orderID;
	boost::posix_time::ptime extime;

	ExchEv() : req_id(0), extime(boost::posix_time::microsec_clock().universal_time()) {};
	ExchEv(typeEvent type_) : type(type_), req_id(0), extime(boost::posix_time::microsec_clock().universal_time()) {};
	ExchEv(typeEvent type_, uint exch_orderID_) : type(type_), exch_orderID(exch_orderID_), req_id(0), extime(boost::posix_time::microsec_clock().universal_time()) {};

	virtual void Mes() //= 0;
	{
		stringstream tmp;
		tmp << GetTypeEvent(type) << ","
			<< exch_orderID << ","
			<< req_id << ","
			<< extime << ",";
		cout << tmp.str();
		writeLogToFile(tmp.str());
	}
};

struct CreateOrderEvent : public ExchEv
{
	uint success;
	//double fill_size;
	//double avg_fill_price;

	CreateOrderEvent(uint exch_order_id_) // , double fill_size_, double avg_fill_price_
		: ExchEv(typeEvent::CreateOrder, exch_order_id_), success(1) {} // fill_size(0), avg_fill_price(0), 

	void Mes() override
	{
		ExchEv::Mes();
		stringstream tmp;
		tmp << success << endl;
		cout << tmp.str();
		writeLogToFile(tmp.str());
	}
};

struct CancelOrderEvent : public ExchEv
{
	uint success; // 0 - false, 1 - true
	string errmes;

	CancelOrderEvent(uint exch_order_id_, string errmes_= "")
		: ExchEv(typeEvent::CancelOrder, exch_order_id_), errmes(errmes_) 
	{
		success = (errmes_ == "" ? 1 : 0);
	}

	void SuccessCancel()
	{
		this->success = 1;
	}
	void SetErrmes(string errmes_)
	{
		errmes = errmes_;
	}
	void Mes() override
	{
		ExchEv::Mes();
		stringstream tmp;
		tmp << success << ","
			<< errmes << endl; 
		cout << tmp.str();
		writeLogToFile(tmp.str());
	}
};

struct ModifyOrderEvent : public ExchEv
{
	uint success;
	double new_price;
	double new_size;
	string errmes;

	ModifyOrderEvent(uint exch_order_id_, double new_price_, double new_size_, string errmes_ = "")
		: ExchEv(typeEvent::ModifyOrder, exch_order_id_), new_price(new_price_), new_size(new_size_), errmes(errmes_) 
	{
		success = (errmes_ == "" ? 1 : 0);
	}
	void SuccessModify()
	{
		this->success = 1;
	}
	void SetErrmes(string errmes_)
	{
		errmes = errmes_;
	}
	void Mes() override
	{
		ExchEv::Mes();
		stringstream tmp;
		tmp << new_price << ","
			<< new_size << ","
			<< success << ","
			<< errmes << endl;
		cout << tmp.str();
		writeLogToFile(tmp.str());
	}
};

struct FillOrderEvent : public ExchEv
{
	double filled_size;
	double filled_price;
	uint maker;

	FillOrderEvent(uint exch_order_id_, double filled_size_, double filled_price_, uint maker_)
		: ExchEv(typeEvent::FillOrder, exch_order_id_), filled_size(filled_size_), filled_price(filled_price_), maker(maker_) {}
	void Mes() override
	{
		ExchEv::Mes();
		stringstream tmp;
		tmp << filled_size << ","
			<< filled_price << ","
			<< maker << endl;
		cout << tmp.str();

		stringstream tmm;
		tmm << filled_size << " "
			<< filled_price << " "
			<< maker << " "
			<< extime;
		allFill[exch_orderID].push_back(tmm.str());

		writeLogToFile(tmp.str());
	}
};

struct BbaEvent : public ExchEv
{
	pair <double, double> ba;
	string instr;

	BbaEvent(cstring instr_, double b, double a)
		: ExchEv(typeEvent::Bba, 0), ba(ba.first = b, ba.second = a), instr(instr_) {}
	BbaEvent(string instr_, pair <double, double> ba_)
		: ExchEv(typeEvent::Bba, 0), ba(ba_), instr(instr_) {}
	void Mes() override
	{
		ExchEv::Mes();
		stringstream tmp;
		tmp << instr << ","
			<< ba.first  << ","
			<< ba.second << endl;
		cout << tmp.str();
		writeLogToFile(tmp.str());
	}

};

struct ObEvent : public ExchEv
{
	vector <multimap<double, uint>> ba;
	string instr;

	ObEvent(cstring instr_, vector <multimap<double, uint>> ba_)
		: ExchEv(typeEvent::Ob, 0), ba(2), instr(instr_)
	{
		ba = ba_;
	}
	void Mes() override
	{
		ExchEv::Mes();
		stringstream tmp;
		tmp << instr << ",";
		
		multimap<double, uint>::iterator it;
		for (it = ba[0].begin(); it != ba[0].end(); it++)
			tmp << "bid " << it->first << ",";

		for (it = ba[1].begin(); it != ba[1].end(); it++)
			tmp << "ask " << it->first << ",";
		tmp << endl;
		cout << tmp.str();
		writeLogToFile(tmp.str());
	}
};

struct PosUpdateEvent : public ExchEv
{
	//double pos;

	pair<double, double> pos;
	pair<string, string> instr;

	PosUpdateEvent(pair<double,double> pos_, cstring instr_)
		: ExchEv(typeEvent::PosUpdate, 0), pos(pos_) 
	{
		auto tmp = splitStr(instr_, "-");
		instr.first = tmp[0];
		instr.second = tmp[1];
	}
	void Mes() override
	{
		ExchEv::Mes();
		stringstream tmp;
		tmp << instr.first << " " << pos.first << ","
			<< instr.second << " " << pos.second << endl;
		cout << tmp.str();
		writeLogToFile(tmp.str());
	}
};