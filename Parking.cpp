#include "Parking.h"

//入出场构造
Parking::Parking(PARKINGTYPE parkingType, string account, string carNumber, string time, string position, int picId)
    :account(account),carNumber(carNumber)
{
    if (parkingType == PARKINGTYPE::ENTRY) {
        this->entryTime = time;
        this->entryPicId = picId;
        this->entryPosition = position;
    }
    else if (parkingType == PARKINGTYPE::LEAVE) {
        this->leaveTime = time;
        this->leavePicId = picId;
        this->leavePosition = position;
    }
}
//完整构造
Parking::Parking(string account, string carNumber, int dueCost, int reallyCost, string entryTime, string entryPosition, int entryPicId, 
    string leaveTime, string leavePosition, int leavePicId, int id) :account(account), carNumber(carNumber), dueCost(dueCost),
    reallyCost(reallyCost), entryTime(entryTime), entryPosition(entryPosition), entryPicId(entryPicId), leaveTime(leaveTime),
    leavePosition(leavePosition), leavePicId(leavePicId), id(id)
{
}

int Parking::getId() const
{
    return this->id;
}

void Parking::setId(int id)
{
    this->id = id;
}

int Parking::getDueCost() const
{
    return this->dueCost;
}

void Parking::setDueCost(int dueCost)
{
    this->dueCost = dueCost;
}

int Parking::getReallyCost() const
{
    return reallyCost;
}

void Parking::setReallyCost(int reallyCost)
{
    this->reallyCost = reallyCost;
}

string Parking::getEntryTime() const
{
    return entryTime;
}

void Parking::setEntryTime(string entryTime)
{
    this->entryTime = entryTime;
}

string Parking::getEntryPosition() const
{
    return entryPosition;
}

void Parking::setEntryPosition(string entryPosition)
{
    this->entryPosition = entryPosition;
}

int Parking::getEntryPicId() const
{
    return entryPicId;
}

void Parking::setEntryPicId(int entryPicId)
{
    this->entryPicId = entryPicId;
}

string Parking::getLeaveTime() const
{
    return leaveTime;
}

void Parking::setLeaveTime(string leaveTime)
{
    this->leaveTime = leaveTime;
}

string Parking::getLeavePosition() const
{
    return leavePosition;
}

void Parking::setLeavePosition(string leavePosition)
{
    this->leavePosition = leavePosition;
}

int Parking::getLeavePicId() const
{
    return leavePicId;
}

void Parking::setLeavePicId(int leavePicId)
{
    this->leavePicId = leavePicId;
}

string Parking::getAccount() const
{
    return account;
}

void Parking::setAccount(string account)
{
    this->account = account;
}

string Parking::getCarNumber() const
{
    return carNumber;
}

void Parking::setCarNumber(string carNumber)
{
    this->carNumber = carNumber;
}

