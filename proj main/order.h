#pragma once
#include <iostream>
using namespace std;

class chef;
class scooter;
class table;

enum ORDER_TYPE {
	ODG,
	ODN,
	OT,
	OVC,
	OVG,
	OVN,
};

class order
{
private:
	int ID;
	ORDER_TYPE type;
	int size;
	double price;
	int TQ, TA, TR, TS, TF, TW; // Requested, Assigned to chef, Ready, Service starts, Finished, Wait times
	int distance;
	int seats;
	int duration;
	bool canShare;
	chef* assignedChef;
	scooter* assignedScooter;
	table* assignedTable;

public:
	order();
	order(int id, ORDER_TYPE t, int tq, int Size, double P);
	void setScooter(scooter* s);
	scooter* getScooter() const;

	void setTable(table* t);
	table* getTable() const;

	int getID() const;
	ORDER_TYPE getType() const;

	int getSize() const;
	double getPrice() const;
	int getTQ() const;
	int getTA() const;
	int getTR() const;
	int getTS() const;
	int getTF() const;
	int getTW() const;
	int getDistance() const;
	int getSeats() const;
	int getDuration() const;
	bool getCanShare() const;

	void setType(ORDER_TYPE t);
	void setSize(int s);
	void setPrice(double p);
	void setTQ(int t);
	void setTA(int t);
	void setTR(int t);
	void setTS(int t);
	void setTF(int t);
	void setTW(int t);
	void setDistance(int d);
	void setSeats(int s);
	void setDuration(int d);
	void setCanShare(bool c);

	void setChef(chef* c);
	chef* getChef() const;

	bool isDineIn() const;
	bool isTakeaway() const;
	bool isDelivery() const;
	bool isGrilled() const;

	int getIdleTime() const;
	int getCookPeriod() const;
	int getWaitTime() const;
	int getServiceDuration() const;

	const char* getTypeAsString() const;

	void Print() const;

	double getPriority() const;
	friend ostream& operator<<(ostream& out, const order* pOrd);

	
};
struct OrderIDOnly {
	const order* ord;
	OrderIDOnly(const order* o) : ord(o) {}

	friend ostream& operator<<(ostream& os, const OrderIDOnly& wrapper) {
		if (wrapper.ord) os << wrapper.ord->getID();
		return os;
	}
};