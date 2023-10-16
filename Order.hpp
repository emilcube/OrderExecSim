#pragma once

class Order
{
public:

	uint client_order_id;
	string instr;
	double size;
	double limit_price;
	int side; // -1 Sell, +1 Buy
	uint type; // 0 - Limit, 1 - Market
	uint tif; // Time in force, 0 - GTC, 1 - FOK, 2 - IOC
	
	//==========
	boost::posix_time::ptime exch_submit_time;
	boost::posix_time::ptime exch_receive_time;
	string exch_order_id;
	/*uint internal_exch_order_id;*/
	double filled_size;
	double avg_fill_price;
	uint cancelled;
	//uint modified;
	uint maker;

	Order(uint client_order_id_, cstring instr_, double size_, double limit_price_, int side_, uint type_, uint tif_ = 0)
		: client_order_id(client_order_id_), instr(instr_), size(size_), limit_price(limit_price_), side(side_), type(type_),
		exch_submit_time(boost::posix_time::microsec_clock().universal_time()),
		exch_receive_time(boost::posix_time::microsec_clock().universal_time()),
		filled_size(0), avg_fill_price(0.0), cancelled(0), tif(tif_), maker(0) /*, internal_exch_order_id(100000000)*/
	{
		if (type == 1) // if market
		{
			limit_price = (side == 1 ? 999999999.0 : 0.0000001 );
		}
	}

	void SetExchOrderId(const uint number)
	{
		exch_submit_time = boost::posix_time::microsec_clock().universal_time();
		exch_receive_time = boost::posix_time::microsec_clock().universal_time();

		this->exch_order_id = std::to_string(number);
	}

	uint GetExchOrderId()
	{
		return std::stoi(exch_order_id);
	}

	void SetCancel()
	{
		this->cancelled = 1;
	}

	bool IsActive() const
	{
		return ((this->cancelled == 0) && (this->filled_size < this->size));
	}

	double AmountNeedToFill()
	{
		return (this->size - this->filled_size);
	}

	//void SetFilledSize(double now_filled_size)
	//{
	//	this->filled_size += now_filled_size;
	//}

	void SetFilledSize(double now_filled_size, double now_price)
	{
		this->filled_size += now_filled_size;

		this->avg_fill_price = (((this->filled_size - now_filled_size) == 0 ? 0 : (this->avg_fill_price / (this->filled_size / (this->filled_size - now_filled_size))))
			+ now_filled_size * now_price / this->filled_size);
	}

	//void CalcAverageFillPrice(double now_filled_size, double now_price)
	//{
	//	this->avg_fill_price = (((this->filled_size - now_filled_size) == 0 ? 0 : (this->avg_fill_price / (this->filled_size / (this->filled_size - now_filled_size))))
	//		+ now_filled_size * now_price / this->filled_size);
	//}

	bool NotFullyFilled() // for matching in ob
	{
		if (this->size != this->filled_size)
			return true;
		else
			return false;
	}

	uint Calc_ba_index()
	{
		return (1 - (this->side + 1) / 2);
	}

	std::stringstream OrderInfoJson()
	{
		std::stringstream out;
		out << "{" << "\"client_order_id\":" << client_order_id << ","
			<< "\"instr\":" << "\"" << instr << "\"" << ","
			<< "\"size\":" << std::setprecision(5) << std::fixed << size << ","
			<< "\"limit_price\":" << limit_price << ","
			<< "\"side\":" << side << ","
			<< "\"type\":" << type << ","
			<< "\"tif\":" << tif << ","
			<< "\"exch_submit_time\":" << "\"" << exch_submit_time << "\"" << ","
			<< "\"exch_receive_time\":" << "\"" << exch_receive_time << "\"" << ","
			<< "\"exch_order_id\":" << exch_order_id << ","
			<< "\"filled_size\":" << filled_size << ","
			<< "\"avg_fill_price\":" << avg_fill_price << ","
			<< "\"cancelled\":" << cancelled << ","
			<< "\"maker\":" << maker << "}";
		return out;
	}

	void PrintOrder()
	{
		std::stringstream out;
		out << this->client_order_id << " "
			<< this->instr << " " << this->size << " " << this->limit_price << " "
			<< this->side << " " << this->type << " " << this->tif << " " << this->exch_submit_time << " "
			<< this->exch_receive_time << " " << this->exch_order_id << " "
			<< this->filled_size << " " << this->avg_fill_price << " " << this->cancelled;
		std::cout << out.str() << endl;
	}

	bool FOKstate(const double& delta)
	{
		if (this->tif == 1 && delta != AmountNeedToFill())
		{
			cancelled = 1;
			return true;
		}
		return false;
	}

	bool IOCstate(const double& delta)
	{
		if (this->tif == 2 && AmountNeedToFill() != 0)
		{
			cancelled = 1;
			return true;
		}
		return false;
	}

	double Comission(const double& x)
	{
		return -1.0*(maker == 1 ? comission_maker * x : comission_taker * x);
	}

};