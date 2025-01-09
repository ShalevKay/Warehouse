#include "../include/Order.h"
using namespace std;

Order::Order(int id, int customerId, int distance)
: id(id), customerId(customerId), distance(distance), status(OrderStatus::PENDING), collectorId(NO_VOLUNTEER), driverId(NO_VOLUNTEER) {}

int Order::getId() const { return id; }
int Order::getCustomerId() const { return customerId; }
int Order::getDistance() const { return distance; }
OrderStatus Order::getStatus() const { return status; }

const string Order::getStatusString() const {
    return status == OrderStatus::PENDING ? "Pending" :
           status == OrderStatus::COLLECTING ? "Collecting" :
           status == OrderStatus::DELIVERING ? "Delivering" :
           status == OrderStatus::COMPLETED ? "Completed" : "Unknown";
}

int Order::getCollectorId() const { return collectorId; }
int Order::getDriverId() const { return driverId; }

void Order::setStatus(OrderStatus status) { this -> status = status; }
void Order::setCollectorId(int collectorId) { this -> collectorId = collectorId; }
void Order::setDriverId(int driverId) { this -> driverId = driverId; }

const string Order::toString() const {
    string toString = "OrderId: " + to_string(id) + "\n" + "OrderStatus: " + getStatusString() + "\n" +
            "CustomerID: " + to_string(customerId) + "\n" +
            "Collector: " + (status == OrderStatus::PENDING ? "None" : to_string(collectorId)) + "\n" +
            "Driver: " + (status == OrderStatus::PENDING || status == OrderStatus::COLLECTING ? "None": to_string(driverId));
    return toString;
}