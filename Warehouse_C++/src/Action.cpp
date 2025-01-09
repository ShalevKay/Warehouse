using namespace std;
#include "../include/Action.h"
#include "../include/Volunteer.h"

// BaseAction

extern WareHouse* backup;

BaseAction::BaseAction() : status(ActionStatus::ERROR), errorMsg("") { }

ActionStatus BaseAction::getStatus() const {
    return status;
}

void BaseAction::complete() {
    status = ActionStatus::COMPLETED;
}

void BaseAction::error(std::string errorMsg) {
    status = ActionStatus::ERROR;
    this->errorMsg = errorMsg;
}

std::string BaseAction::getErrorMsg() const {
    return errorMsg;
}

BaseAction::~BaseAction() = default;

// SimulateStep

SimulateStep::SimulateStep(int numOfSteps) : numOfSteps(numOfSteps){}

void SimulateStep::act(WareHouse &wareHouse) {
    for(int i = 0; i < numOfSteps; i++){
        vector<int> completedOrders;
        for(int volunteerId = 0; volunteerId < (int)wareHouse.getVolunteers().size(); volunteerId++){
            Volunteer *volunteer = wareHouse.getVolunteers()[volunteerId];
            if(volunteer -> hasOrdersLeft() || volunteer -> isBusy()){
                if(!volunteer -> isBusy()){
                    int orderIndex = 0;
                    // Loop over all pending orders
                    for(Order *order : wareHouse.getPendingOrders()){
                        if(volunteer -> canTakeOrder(*order)){
                            volunteer -> acceptOrder(*order);
                            wareHouse.transferOrder(order -> getId(), orderIndex);
                            volunteer->advanceStatus(*order);
                            break;
                        }
                        orderIndex++;
                    }
                }
                if(volunteer -> isBusy()){
                    volunteer -> step();
                    if(volunteer -> getCompletedOrderId() != NO_ORDER){
                        completedOrders.push_back(volunteer -> getCompletedOrderId());
                        volunteer ->setCompleteOrderId(NO_ORDER);
                    }
                }
                if (!volunteer->hasOrdersLeft() && !volunteer->isBusy()) {
                    wareHouse.deleteVolunteer(volunteerId);
                }
            }
            else{
                wareHouse.deleteVolunteer(volunteerId);
            }
        }
        for(int orderId : completedOrders){
            int orderIndex = wareHouse.getOrderIndex(orderId);
            wareHouse.transferOrder(orderId, orderIndex);
        }
        completedOrders.clear();
    }
    complete();
}

string SimulateStep::toString() const {
    return "simulateStep " + to_string(numOfSteps) + " " + (getStatus() == ActionStatus::COMPLETED ? "COMPLETED" : "ERROR");
}

SimulateStep *SimulateStep::clone() const{
    return new SimulateStep(*this);
}

// AddOrder

AddOrder::AddOrder(int id) : customerId(id) { }

void AddOrder::act(WareHouse &wareHouse) {

    if(!wareHouse.customerExists(customerId)){
        error("Cannot place this order\n");
        return;
    }

    Customer &customer = wareHouse.getCustomer(customerId);
    if (!customer.canMakeOrder()){
        error("Cannot place this order\n");
        return;
    }

    int orderId = wareHouse.requestNextOrderId();
    Order* order = new Order(orderId, customerId, customer.getCustomerDistance());

    customer.addOrder(orderId);
    wareHouse.addOrder(order);
    complete();
}

string AddOrder::toString() const {
    return "order " + to_string(customerId) + " " + (getStatus() == ActionStatus::COMPLETED ? "COMPLETED" : "ERROR");
}

AddOrder *AddOrder::clone() const{
    return new AddOrder(*this);
}

// AddCustomer

AddCustomer::AddCustomer(const string &customerName, const string &customerType, int distance, int maxOrders)
: customerName(customerName), distance(distance), maxOrders(maxOrders),
customerType(customerType == "Soldier" ? CustomerType::Soldier : CustomerType::Civilian) { }

void AddCustomer::act(WareHouse &wareHouse) {
    Customer *customer = nullptr;

    int customerId = wareHouse.requestNextCustomerId();
    if (customerType == CustomerType::Soldier)
        customer = new SoldierCustomer(customerId, customerName, distance, maxOrders);
    else
        customer = new CivilianCustomer(customerId, customerName, distance, maxOrders);

    wareHouse.addCustomer(customer);
    complete();
}

AddCustomer *AddCustomer::clone() const{
    return new AddCustomer(*this);
}

string AddCustomer::toString() const {
    return "customer " + customerName + " " + customerTypeString() + " " + to_string(distance) + " " + to_string(maxOrders) + " " + (getStatus() == ActionStatus::COMPLETED ? "COMPLETED" : "ERROR");
}

string AddCustomer::customerTypeString() const {
    return customerType == CustomerType::Soldier ? "soldier" : "civilian";
}

// PrintOrderStatus

PrintOrderStatus::PrintOrderStatus(int id) : orderId(id) {}

void PrintOrderStatus::act(WareHouse &wareHouse) {
    if(!wareHouse.orderExists(orderId)){
        error("Order doesn't exist\n");
        return;
    }

    Order &order = wareHouse.getOrder(orderId);
    string status_str = order.getStatusString();

    cout << "\n" + order.toString() + "\n" << endl;
    complete();
}

string PrintOrderStatus::toString() const {
    return "orderStatus " + to_string(orderId) + " " + (getStatus() == ActionStatus::COMPLETED ? "COMPLETED" : "ERROR");
}

PrintOrderStatus *PrintOrderStatus::clone() const{
    return new PrintOrderStatus(*this);
}

// PrintCustomerStatus

PrintCustomerStatus::PrintCustomerStatus(int customerId) : customerId(customerId) {}

void PrintCustomerStatus::act(WareHouse &wareHouse) {
    if(!wareHouse.customerExists(customerId)){
        error("Customer doesn't exist\n");
        return;
    }

    Customer &customer = wareHouse.getCustomer(customerId);
    cout << "\nCustomerID: " << customer.getId() << endl;
    for(int orderId : customer.getOrdersIds()){
        Order &order = wareHouse.getOrder(orderId);
        cout << "OrderID: " << order.getId() << endl;
        cout << "OrderStatus: " << order.getStatusString() << endl;
    }
    cout << "numOrdersLeft: " << customer.numOrdersLeft() << "\n" << endl;
    complete();
}

string PrintCustomerStatus::toString() const {
    return "customerStatus " + to_string(customerId) + " " + (getStatus() == ActionStatus::COMPLETED ? "COMPLETED" : "ERROR");
}

PrintCustomerStatus *PrintCustomerStatus::clone() const{
    return new PrintCustomerStatus(*this);
}

// PrintVolunteerStatus

PrintVolunteerStatus::PrintVolunteerStatus(int id) : volunteerId(id){}

void PrintVolunteerStatus::act(WareHouse &wareHouse) {
    if(!wareHouse.volunteerExists(volunteerId)){
        error("\nVolunteer doesn't exist\n");
        return;
    }

    Volunteer &volunteer = wareHouse.getVolunteer(volunteerId);
    cout << "\n" + volunteer.toString() + "\n" << endl;
    complete();
}

PrintVolunteerStatus *PrintVolunteerStatus::clone() const {
    return new PrintVolunteerStatus(*this);
}

string PrintVolunteerStatus::toString() const {
    return "volunteerStatus " + to_string(volunteerId) + " " + (getStatus() == ActionStatus::COMPLETED ? "COMPLETED" : "ERROR");
}

// PrintActionsLog

PrintActionsLog::PrintActionsLog() {}

void PrintActionsLog::act(WareHouse &wareHouse) {
    vector<BaseAction*> actions = wareHouse.getActions();
    cout << endl;
    for(BaseAction *action : actions){
        cout << action -> toString() << endl;
    }
    cout << endl;
    complete();
}

PrintActionsLog *PrintActionsLog::clone() const {
    return new PrintActionsLog(*this);
}

string PrintActionsLog::toString() const {
    return "log COMPLETED";
}

// Close

Close::Close() {}

void Close::act(WareHouse &wareHouse) {
    cout << endl;
    for(Order *order : wareHouse.getPendingOrders()){
        cout << "OrderID: " << order -> getId() << ",CustomerID: " << order -> getCustomerId() << ",OrderStatus: " << order -> getStatusString() << endl;
    }
    for(Order *order : wareHouse.getInProcessOrders()){
        cout << "OrderID: " << order -> getId() << ",CustomerID: " << order -> getCustomerId() << ",OrderStatus: " << order -> getStatusString() << endl;
    }
    for(Order *order : wareHouse.getCompletedOrders()){
        cout << "OrderID: " << order -> getId() << ",CustomerID: " << order -> getCustomerId() << ",OrderStatus: " << order -> getStatusString() << endl;
    }
    wareHouse.close();
    complete();
}

Close *Close::clone() const {
    return new Close(*this);
}

string Close::toString() const {
    return "close COMPLETED";
}

// BackupWareHouse

BackupWareHouse::BackupWareHouse() {}

void BackupWareHouse::act(WareHouse &wareHouse) {
    if(backup != nullptr){
        *backup = wareHouse;
    }
    else{
        backup = new WareHouse(wareHouse);
    }
    complete();
}

BackupWareHouse *BackupWareHouse::clone() const {
    return new BackupWareHouse(*this);
}

string BackupWareHouse::toString() const {
    return "backup COMPLETED";
}

// RestoreWareHouse

RestoreWareHouse::RestoreWareHouse() {}

void RestoreWareHouse::act(WareHouse &wareHouse) {
    if (backup == nullptr){
        error("No backup available\n");
        return;
    }
    wareHouse = *backup;
    complete();
}

RestoreWareHouse *RestoreWareHouse::clone() const {
    return new RestoreWareHouse(*this);
}

string RestoreWareHouse::toString() const {
    string toString = "restore ";
    if(getStatus() == ActionStatus::COMPLETED){
        return toString + "COMPLETED";
    }
    else{
        return toString + "ERROR";
    }
}