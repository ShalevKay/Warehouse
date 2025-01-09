#include "../include/Customer.h"

Customer::Customer(int id, const string &name, int locationDistance, int maxOrders)
: id(id), name(name), locationDistance(locationDistance), maxOrders(maxOrders), ordersId() { }

int Customer::getId() const { return id; }
const string &Customer::getName() const { return name; }
int Customer::getCustomerDistance() const { return locationDistance; }
int Customer::getMaxOrders() const { return maxOrders; }
const vector<int> &Customer::getOrdersIds() const { return ordersId; }

int Customer::getNumOrders() const { return ordersId.size(); }
int Customer::numOrdersLeft() const { return maxOrders - this->getNumOrders(); }
bool Customer::canMakeOrder() const { return maxOrders > this->getNumOrders(); }

int Customer::addOrder(int orderId) {
    if (!this -> canMakeOrder())
        return -1;

    ordersId.push_back(orderId);
    return orderId;
}

Customer::~Customer() = default;

SoldierCustomer::SoldierCustomer(int id, const string &name, int locationDistance, int maxOrders)
: Customer(id, name, locationDistance, maxOrders) { }

SoldierCustomer* SoldierCustomer::clone() const {
    return new SoldierCustomer(*this);
}

CivilianCustomer::CivilianCustomer(int id, const std::string &name, int locationDistance, int maxOrders)
: Customer(id, name, locationDistance, maxOrders) { }

CivilianCustomer* CivilianCustomer::clone() const {
    return new CivilianCustomer(*this);
}