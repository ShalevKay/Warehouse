#include "../include/Volunteer.h"
using namespace std;

/**
 * Begin Volunteer
 */

Volunteer::Volunteer(int id, const string &name) : completedOrderId(NO_ORDER), activeOrderId(NO_ORDER), id(id), name(name) {}

int Volunteer::getId() const {
    return id;
}

const string &Volunteer::getName() const {
    return name;
}

int Volunteer::getActiveOrderId() const {
    return activeOrderId;
}

int Volunteer::getCompletedOrderId() const {
    return completedOrderId;
}

bool Volunteer::isBusy() const {
    return activeOrderId != NO_ORDER;
}

void Volunteer::setCompleteOrderId(int newCompletedOrderId) {
    completedOrderId = newCompletedOrderId;
}

void Volunteer::advanceStatus(Order &order) const {
    if(order.getStatus() == OrderStatus::PENDING){
        order.setStatus(OrderStatus::COLLECTING);
        order.setCollectorId(this->getId());
    }
    else if(order.getStatus() == OrderStatus::COLLECTING){
        order.setStatus(OrderStatus::DELIVERING);
        order.setDriverId(this->getId());
    }
}

Volunteer::~Volunteer() = default;

/**
 * end of Volunteer
 * begin CollectorVolunteer
 */

CollectorVolunteer::CollectorVolunteer(int id, const string &name, int coolDown) : Volunteer(id, name), coolDown(coolDown), timeLeft(NO_ORDER) {}

CollectorVolunteer *CollectorVolunteer::clone() const {
    return new CollectorVolunteer(*this);
}

void CollectorVolunteer::step() {
    decreaseCoolDown();
    if(timeLeft == 0){
        completedOrderId = activeOrderId;
        activeOrderId = NO_ORDER;
    }
}

int CollectorVolunteer::getCoolDown() const {
    return coolDown;
}

int CollectorVolunteer::getTimeLeft() const {
    return timeLeft;
}

bool CollectorVolunteer::decreaseCoolDown() {
    timeLeft--;
    return timeLeft == 0;
}

bool CollectorVolunteer::hasOrdersLeft() const {
    return true;
}

bool CollectorVolunteer::canTakeOrder(const Order &order) const {
    if(activeOrderId == NO_ORDER && order.getStatus() == OrderStatus::PENDING){
        return true;
    }
    return false;
}

void CollectorVolunteer::acceptOrder(const Order &order) {
    activeOrderId = order.getId();
    timeLeft = coolDown;
}

string CollectorVolunteer::toString() const {
//    string activeOrderIdStr = activeOrderId == NO_ORDER ? "None" : to_string(activeOrderId);
//    string completedOrderIdStr = completedOrderId == NO_ORDER ? "None" : to_string(completedOrderId);
//    return "VolunteerID: " + to_string(getId()) + "\n" + "Volunteer name: " + getName() + "\n" + "Active OrderID: " + activeOrderIdStr +
//    "\n" + "Completed OrderId: " + completedOrderIdStr + "\n" + "Cooldown: " + to_string(coolDown) + "\n" +
//    "TimeLeft: " + to_string(getTimeLeft());
    string toString = "VolunteerID: " + to_string(getId()) + "\n" + "isBusy: " + ((isBusy()==0) ? "False" : "True") + "\n" +
    "OrderID: " + (isBusy() ? to_string(activeOrderId) : "None") + "\n" + getStatus() + "\n" +
    "OrdersLeft: " + ordersLeftString();
    return toString;
}

void CollectorVolunteer::setTimeLeft(int timeLeft) {
    this -> timeLeft = timeLeft;
}

string CollectorVolunteer::ordersLeftString() const {
    return "No Limit";
}

string CollectorVolunteer::getStatus() const {
    return "TimeLeft: " + (activeOrderId != NO_ORDER ? to_string(timeLeft) : "None");
}

/**
 * end of CollectorVolunteer
 * begin LimitedCollectorVolunteer
 */

LimitedCollectorVolunteer::LimitedCollectorVolunteer(int id, const string &name, int coolDown, int maxOrders)
: CollectorVolunteer(id, name, coolDown), maxOrders(maxOrders), ordersLeft(maxOrders) {}

LimitedCollectorVolunteer *LimitedCollectorVolunteer::clone() const {
    return new LimitedCollectorVolunteer(*this);
}

bool LimitedCollectorVolunteer::hasOrdersLeft() const {
    return ordersLeft > 0;
}

bool LimitedCollectorVolunteer::canTakeOrder(const Order &order) const{
    if(activeOrderId == NO_ORDER && order.getStatus() == OrderStatus::PENDING && hasOrdersLeft()){
        return true;
    }
    return false;
}

void LimitedCollectorVolunteer::acceptOrder(const Order &order) {
    activeOrderId = order.getId();
    ordersLeft--;
    setTimeLeft(getCoolDown());
}

int LimitedCollectorVolunteer::getMaxOrders() const {
    return maxOrders;
}

int LimitedCollectorVolunteer::getNumOrdersLeft() const {
    return ordersLeft;
}

string LimitedCollectorVolunteer::toString() const {
    string toString = CollectorVolunteer::toString();
    return toString;


}

string LimitedCollectorVolunteer::ordersLeftString() const {
    return to_string(ordersLeft);
}

string LimitedCollectorVolunteer::getStatus() const {
    return "TimeLeft: " + (activeOrderId != NO_ORDER ? to_string(getTimeLeft()) : "None");
}

/**
 * end of LimitedCollectorVolunteer
 * begin DriverVolunteer
 */

DriverVolunteer::DriverVolunteer(int id, const string &name, int maxDistance, int distancePerStep)
 : Volunteer(id, name), maxDistance(maxDistance), distancePerStep(distancePerStep), distanceLeft(NO_ORDER) {}

DriverVolunteer *DriverVolunteer::clone() const {
     return new DriverVolunteer(*this);
 }

 int DriverVolunteer::getDistanceLeft() const {
     return distanceLeft;
 }

 int DriverVolunteer::getMaxDistance() const {
     return maxDistance;
 }

 int DriverVolunteer::getDistancePerStep() const {
    return distancePerStep;
 }

bool DriverVolunteer::decreaseDistanceLeft() {
    distanceLeft -= distancePerStep;
    distanceLeft = distanceLeft < 0 ? 0 : distanceLeft;
    return distanceLeft == 0;
 }

bool DriverVolunteer::hasOrdersLeft() const {
    return true;
 }

 bool DriverVolunteer::canTakeOrder(const Order &order) const {
     if(activeOrderId == NO_ORDER && order.getStatus() == OrderStatus::COLLECTING && maxDistance >= order.getDistance()){
         return true;
     }
     return false;
 }

 void DriverVolunteer::acceptOrder(const Order &order) {
     activeOrderId = order.getId();
     distanceLeft = order.getDistance();
 }

 void DriverVolunteer::step() {
     if(decreaseDistanceLeft()){
         completedOrderId = activeOrderId;
         activeOrderId = NO_ORDER;
     }
 }

 string DriverVolunteer::toString() const {
//     string activeOrderIdStr = activeOrderId == NO_ORDER ? "None" : to_string(activeOrderId);
//     string completedOrderIdStr = completedOrderId == NO_ORDER ? "None" : to_string(completedOrderId);
//     return "VolunteerID: " + to_string(getId()) + "\n" + "Volunteer name: " + getName() + "\n" + "Active OrderID: " + activeOrderIdStr +
//            "\n" + "Completed OrderId: " + completedOrderIdStr + "\n" + "Max Distance: " + to_string(maxDistance) + "\n" +
//            "Distance Per Step: " + to_string(distancePerStep) + "\n" + "Distance Left: " + to_string(distanceLeft);
     string toString = "VolunteerID: " + to_string(getId()) + "\n" + "isBusy: " + ((isBusy()==0) ? "False" : "True") + "\n" +
                       "OrderID: " + (isBusy() ? to_string(activeOrderId) : "None") + getStatus() + "\n" +
                       "OrdersLeft: " + ordersLeftString() ;
     return toString;
 }

 void DriverVolunteer::setDistanceLeft(int distanceLeft) {
     DriverVolunteer::distanceLeft = distanceLeft;
 }

 string DriverVolunteer::ordersLeftString() const {
     return "No Limit";
}

string DriverVolunteer::getStatus() const {
    return "DistanceLeft: " + (activeOrderId != NO_ORDER ? to_string(distanceLeft) : "None");
}

/**
 * end of DriverVolunteer
 * begin LimitedDriverVolunteer
 */

LimitedDriverVolunteer::LimitedDriverVolunteer(int id, const string &name, int maxDistance, int distancePerStep, int maxOrders)
: DriverVolunteer(id, name, maxDistance, distancePerStep), maxOrders(maxOrders), ordersLeft(maxOrders) {}

LimitedDriverVolunteer *LimitedDriverVolunteer::clone() const {
    return new LimitedDriverVolunteer(*this);
}

int LimitedDriverVolunteer::getMaxOrders() const {
    return maxOrders;
}

int LimitedDriverVolunteer::getNumOrdersLeft() const {
    return ordersLeft;
}

bool LimitedDriverVolunteer::hasOrdersLeft() const {
    return ordersLeft > 0;
}

bool LimitedDriverVolunteer::canTakeOrder(const Order &order) const {
    if(activeOrderId == -1 && order.getStatus() == OrderStatus::COLLECTING && getMaxDistance() >= order.getDistance() && hasOrdersLeft()){
        return true;
    }
    return false;
}

void LimitedDriverVolunteer::acceptOrder(const Order &order) {
    activeOrderId = order.getId();
    ordersLeft--;
    setDistanceLeft(order.getDistance());
}

string LimitedDriverVolunteer::toString() const {
    string toString = DriverVolunteer::toString();
    return toString;
}

string LimitedDriverVolunteer::ordersLeftString() const {
    return to_string(ordersLeft);
}

string LimitedDriverVolunteer::getStatus() const {
    return "DistanceLeft: " + (activeOrderId != NO_ORDER ? to_string(getDistanceLeft()) : "None");
}

/**
* end of LimitedDriverVolunteer
*/