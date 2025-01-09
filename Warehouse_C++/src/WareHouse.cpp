#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cctype>

#include "../include/WareHouse.h"
#include "../include/Action.h"
#include "../include/Volunteer.h"

// Split String By Delimiter
// Copied From https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
vector<string> split(const string &s, const string &delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    string token;
    vector<string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != string::npos) {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
}

// Trim From End (InPlace)
// Taken From https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
inline void rtrim(string &s) {
    s.erase(find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !isspace(ch);
    }).base(), s.end());
}

WareHouse::WareHouse(const string &configFilePath) :
isOpen(false), customerCounter(0), volunteerCounter(0), orderCounter(0),
volunteers(), pendingOrders(), inProcessOrders(), completedOrders(), customers(),
actionsLog(){

    std::ifstream file(configFilePath);

    string line;
    while (std::getline(file, line)) {
        if (line.find('#') != string::npos) {
            line = line.substr(0, line.find('#'));
            rtrim(line);
        }

        vector<string> parts = split(line, " ");
        if (parts.size() <= 1)
            continue;

        if (parts[0] == "volunteer") {
            Volunteer *v = nullptr;
            if (parts[2] == "collector") {
                v = new CollectorVolunteer(volunteerCounter, parts[1], stoi(parts[3]));
            } else if (parts[2] == "limited_collector") {
                v = new LimitedCollectorVolunteer(volunteerCounter, parts[1], stoi(parts[3]), stoi(parts[4]));
            } else if (parts[2] == "driver") {
                v = new DriverVolunteer(volunteerCounter, parts[1], stoi(parts[3]), stoi(parts[4]));
            } else if (parts[2] == "limited_driver") {
                v = new LimitedDriverVolunteer(volunteerCounter, parts[1], stoi(parts[3]), stoi(parts[4]),
                                               stoi(parts[5]));
            }

            addVolunteer(v);
            volunteerCounter++;
        } else if (parts[0] == "customer") {
            AddCustomer addCustomer(parts[1], parts[2], stoi(parts[3]), stoi(parts[4]));
            addCustomer.act(*this);
        }
    }
    file.close();
}

void WareHouse::start() {
    open();
    std::cout << "WareHouse is open!" << std::endl;

    string nextAction = "open";
    while (isOpen) {

        getline(std::cin, nextAction);
        vector<string> actionParts = split(nextAction, " ");

        BaseAction* action = nullptr;

        if (actionParts[0] == "step") {
            int numOfSteps = stoi(actionParts[1]);
            action = new SimulateStep(numOfSteps);
        }

        else if(actionParts[0] == "customer"){
            string customerName = actionParts[1];
            string customerType = actionParts[2];
            int customerDistance = stoi(actionParts[3]);
            int maxOrders = stoi(actionParts[4]);
            action = new AddCustomer(customerName, customerType, customerDistance, maxOrders);
        }

        else if (actionParts[0] == "order") {
            int id = stoi(actionParts[1]);
            action = new AddOrder(id);
        }

        else if (actionParts[0] == "orderStatus") {
            int id = stoi(actionParts[1]);
            action = new PrintOrderStatus(id);
        }

        else if (actionParts[0] == "customerStatus") {
            int customerId = stoi(actionParts[1]);
            action = new PrintCustomerStatus(customerId);
        }

        else if (actionParts[0] == "volunteerStatus") {
            int id = stoi(actionParts[1]);
            action = new PrintVolunteerStatus(id);
        }

        else if (actionParts[0] == "log") {
            action = new PrintActionsLog();
        }

        else if (actionParts[0] == "backup") {
            action = new BackupWareHouse();
        }

        else if (actionParts[0] == "restore") {
            action = new RestoreWareHouse();
        }

        else if (actionParts[0] == "close") {
            action = new Close();
        }

        if (action != nullptr) {
            action->act(*this);
            if (action->getStatus() == ActionStatus::ERROR)
                std::cout << "\nError: " << action->getErrorMsg() << std::endl;
            addAction(action->clone());
            delete action;
        }
    }
}

Customer &WareHouse::getCustomer(int customerId) const {
    for (Customer *customer : customers) {
        if (customer->getId() == customerId) {
            return *customer;
        }
    }
    Customer *c = new CivilianCustomer(-1, "NoOne", -1, -1);
    return *c;
}

bool WareHouse::customerExists(int customerId) const {
    for (Customer* customer : customers) {
        if (customer->getId() == customerId) {
            return true;
        }
    }
    return false;
}

bool WareHouse::volunteerExists(int volunteerId) const {
    for (Volunteer *volunteer : volunteers) {
        if (volunteer->getId() == volunteerId) {
            return true;
        }
    }
    return false;
}

Volunteer &WareHouse::getVolunteer(int volunteerId) const {
    for (Volunteer *volunteer : volunteers) {
        if (volunteer->getId() == volunteerId) {
            return *volunteer;
        }
    }
    Volunteer *v = new CollectorVolunteer(-1, "NoOne", -1);
    return *v;
}

Order &WareHouse::getOrder(int orderId) const {
    for (Order *order : pendingOrders) {
        if (order->getId() == orderId) {
            return *order;
        }
    }
    for (Order *order : inProcessOrders) {
        if (order->getId() == orderId) {
            return *order;
        }
    }
    for (Order *order : completedOrders) {
        if (order->getId() == orderId) {
            return *order;
        }
    }
    Order *o = new Order(-1, -1, -1);
    return *o;
}

bool WareHouse::orderExists(int orderId) const {
    bool found = false;

    for (Order* order : pendingOrders) {
        if (order->getId() == orderId)
            found = true;
    }
    for (Order* order : inProcessOrders) {
        if (order->getId() == orderId)
            found = true;
    }
    for (Order* order : completedOrders) {
        if (order->getId() == orderId)
            found = true;
    }

    return found;
}

void WareHouse::close() {
    isOpen = false;
}

void WareHouse::open() {
    isOpen = true;
}

void WareHouse::clean() {
    for (BaseAction *action : actionsLog) {
        delete action;
    }
    actionsLog.clear();
    for (Volunteer *volunteer : volunteers) {
        delete volunteer;
    }
    volunteers.clear();
    for (Order *order : pendingOrders) {
        delete order;
    }
    pendingOrders.clear();
    for (Order *order : inProcessOrders) {
        delete order;
    }
    inProcessOrders.clear();
    for (Order *order : completedOrders) {
        delete order;
    }
    completedOrders.clear();
    for (Customer *customer : customers) {
        delete customer;
    }
    customers.clear();
}

int WareHouse::requestNextOrderId() {
    int currentCounter = orderCounter;
    orderCounter++;
    return currentCounter;
}

int WareHouse::requestNextCustomerId() {
    int currentCounter = customerCounter;
    customerCounter++;
    return currentCounter;
}

// Vector Getters
vector<Order*> WareHouse::getPendingOrders() const {
    return pendingOrders;
}

vector<Order*> WareHouse::getInProcessOrders() const {
    return inProcessOrders;
}

vector<Order*> WareHouse::getCompletedOrders() const {
    return completedOrders;
}

vector<Volunteer*> WareHouse::getVolunteers() const {
    return volunteers;
}

const vector<BaseAction*> &WareHouse::getActions() const {
    return actionsLog;
}

int WareHouse::getOrderIndex(int orderId) const{
    int orderIndex = 0;
    for(Order *order : getInProcessOrders()){
        if(order -> getId() != orderId){
            orderIndex++;
        }
        else{
            return orderIndex;
        }
    }
    return NO_ORDER;
}

void WareHouse::transferOrder(int orderId, int orderIndex) {
    Order order = getOrder(orderId);
    if(order.getStatus() == OrderStatus::PENDING){
        vector<Order*>::iterator iterator;
        iterator = pendingOrders.begin() + orderIndex;
        Order *o = pendingOrders[orderIndex];
        pendingOrders.erase(iterator);
        inProcessOrders.push_back(o);
    }
    else if(order.getStatus() == OrderStatus::COLLECTING){
        bool inPending  = false;
        for(Order *ord : pendingOrders){
            if(ord -> getId() == orderId){
                inPending = true;
            }
        }
        if(inPending){
            vector<Order*>::iterator iterator;
            iterator = pendingOrders.begin() + orderIndex;
            Order *o = pendingOrders[orderIndex];
            pendingOrders.erase(iterator);
            inProcessOrders.push_back(o);
        }
        else{
            vector<Order*>::iterator iterator;
            iterator = inProcessOrders.begin() + orderIndex;
            Order *o = inProcessOrders[orderIndex];
            inProcessOrders.erase(iterator);
            pendingOrders.push_back(o);
        }
    }
    else{
        vector<Order*>::iterator iterator;
        iterator = inProcessOrders.begin() + orderIndex;
        Order *o = inProcessOrders[orderIndex];
        inProcessOrders.erase(iterator);
        completedOrders.push_back(o);
        o -> setStatus(OrderStatus::COMPLETED);
    }
}

void WareHouse::deleteVolunteer(int volunteerIndex) {
    Volunteer *v = volunteers[volunteerIndex];
    delete v;
    v = nullptr;

    vector<Volunteer*>::iterator iterator;
    iterator = volunteers.begin() + volunteerIndex;
    volunteers.erase(iterator);
}

// Vector Adders
void WareHouse::addOrder(Order *order) {
    pendingOrders.push_back(order);
}

void WareHouse::addAction(BaseAction *action) {
    actionsLog.push_back(action);
}

void WareHouse::addCustomer(Customer *customer) {
    customers.push_back(customer);
}

void WareHouse::addVolunteer(Volunteer *volunteer) {
    volunteers.push_back(volunteer);
}

//Rule of 5

WareHouse::~WareHouse(){
    clean();
}

WareHouse::WareHouse(const WareHouse &other) :
isOpen(other.isOpen), customerCounter(other.customerCounter),
volunteerCounter(other.volunteerCounter), orderCounter(other.orderCounter),
volunteers(), pendingOrders(), inProcessOrders(), completedOrders(), customers(), actionsLog() {
    for(BaseAction *action : other.actionsLog){
        actionsLog.push_back(action -> clone());
    }
    for(Volunteer *volunteer : other.volunteers){
        volunteers.push_back(volunteer -> clone());
    }
    for(Order *order : other.pendingOrders){
        pendingOrders.push_back(new Order(*order));
    }
    for(Order *order : other.inProcessOrders){
        inProcessOrders.push_back(new Order(*order));
    }
    for(Order *order : other.completedOrders){
        completedOrders.push_back(new Order(*order));
    }
    for(Customer *customer : other.customers){
        customers.push_back(customer -> clone());
    }
}

WareHouse& WareHouse::operator=(const WareHouse& other) {
    if(this != &other){
        clean();

        isOpen = other.isOpen;
        customerCounter = other.customerCounter;
        volunteerCounter = other.volunteerCounter;
        orderCounter = other.orderCounter;
        for(BaseAction *action : other.actionsLog){
            actionsLog.push_back(action -> clone());
        }
        for(Volunteer *volunteer : other.volunteers){
            volunteers.push_back(volunteer -> clone());
        }
        for(Order *order : other.pendingOrders){
            pendingOrders.push_back(new Order(*order));
        }
        for(Order *order : other.inProcessOrders){
            inProcessOrders.push_back(new Order(*order));
        }
        for(Order *order : other.completedOrders){
            completedOrders.push_back(new Order(*order));
        }
        for(Customer *customer : other.customers){
            customers.push_back(customer -> clone());
        }
    }
    return *this;
}

WareHouse::WareHouse(WareHouse &&other) noexcept :
isOpen(other.isOpen), customerCounter(other.customerCounter), volunteerCounter(other.volunteerCounter),
orderCounter(other.orderCounter), volunteers(std::move(other.volunteers)), pendingOrders(std::move(other.pendingOrders)),
inProcessOrders(std::move(other.inProcessOrders)), completedOrders(std::move(other.completedOrders)),
customers(std::move(other.customers)), actionsLog(std::move(other.actionsLog)) { }

WareHouse& WareHouse::operator=(WareHouse &&other) noexcept{
    if(this != &other){
        clean();
        isOpen = other.isOpen;
        customerCounter = other.customerCounter;
        volunteerCounter = other.volunteerCounter;
        orderCounter = other.orderCounter;
        actionsLog = std::move(other.actionsLog);
        volunteers = std::move(other.volunteers);
        pendingOrders = std::move(other.pendingOrders);
        inProcessOrders = std::move(other.inProcessOrders);
        completedOrders = std::move(other.completedOrders);
        customers = std::move(other.customers);
    }
    return *this;
}