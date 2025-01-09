#pragma once
#include <string>
#include <vector>

#include "Order.h"
#include "Customer.h"

class BaseAction;
class Volunteer;

// Warehouse responsible for Volunteers, Customers Actions, and Orders.


class WareHouse {

public:
    WareHouse(const string &configFilePath);
    void start();
    void addOrder(Order* order);
    void addAction(BaseAction* action);
    void addCustomer(Customer* customer);
    void addVolunteer(Volunteer* volunteer);
    Customer &getCustomer(int customerId) const;
    bool customerExists(int customerId) const;
    bool volunteerExists(int volunteerId) const;
    Volunteer &getVolunteer(int volunteerId) const;
    Order &getOrder(int orderId) const;
    bool orderExists(int orderId) const;
    void close();
    void open();
    int requestNextOrderId();
    int requestNextCustomerId();
    vector<Order*> getPendingOrders() const;
    vector<Order*> getInProcessOrders() const;
    vector<Order*> getCompletedOrders() const;
    vector<Volunteer*> getVolunteers() const;
    const vector<BaseAction*> &getActions() const;
    int getOrderIndex(int orderId) const;
    void transferOrder(int orderId, int orderIndex);
    void deleteVolunteer(int volunteerIndex);

    ~WareHouse();
    WareHouse(const WareHouse& other);
    WareHouse& operator=(const WareHouse& other);
    WareHouse(WareHouse&& other) noexcept;
    WareHouse& operator=(WareHouse&& other) noexcept;


private:
    bool isOpen;
    int customerCounter; //For assigning unique customer IDs
    int volunteerCounter; //For assigning unique volunteer IDs
    int orderCounter; //For assigning unique order IDs
    vector<Volunteer*> volunteers;
    vector<Order*> pendingOrders;
    vector<Order*> inProcessOrders;
    vector<Order*> completedOrders;
    vector<Customer*> customers;
    vector<BaseAction*> actionsLog;
    void clean(); //Remember to make BaseAction destructor as virtual
};