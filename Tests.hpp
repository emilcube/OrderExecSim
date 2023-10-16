
void test()
{
	Exchange ord;

	Order ord1(1, "BTC-USDT", 0.5, 600000, 1, 0, 0); // side, type, tif
	Order ord2(2, "BTC-USDT", 1.0, 15000, 1, 0, 0);
	Order ord3(3, "ETH-USDT", 20.0, 3600, 1, 0, 0);

	Order ord4(1, "ETH-USDT", 1.0, 3200, -1, 0, 0);
	Order ord5(2, "ETH-USDT", 2.0, 3300, -1, 0, 0);
	Order ord6(4, "ETH-USDT", 5.0, 3600, -1, 0, 0);
	Order ord7(5, "ETH-USDT", 6.0, 4000, -1, 0, 0);

	Order ord8(4, "BTC-USDT", 0.1, 51000, 1, 0, 0);

	Order ord9(1, "SOL-USDT", 2.0, 100, 1, 0, 0);
	Order ord10(2, "ETH-USDT", 10.0, 4100, 1, 0, 0);
	Order ord11(4, "SOL-USDT", 1.0, 90, -1, 0, 0);

	ord.Subscribe(1376);
	ord.Subscribe(7879);
	ord.Subscribe(8801);

	ord.AddOrder(ord1, 1376);
	ord.AddOrder(ord2, 1376);
	ord.AddOrder(ord3, 1376);

	ord.AddOrder(ord4, 7879);
	ord.AddOrder(ord5, 7879);
	ord.AddOrder(ord6, 7879);
	ord.AddOrder(ord7, 7879);

	ord.AddOrder(ord8, 1376);
	ord.AddOrder(ord9, 8801);
	ord.AddOrder(ord10, 8801);
	ord.AddOrder(ord11, 7879);

	ord.PrintOrders();
	ord.PrintOrderbooks();
	ord.PrintClientsPos();
	cout << endl;
}