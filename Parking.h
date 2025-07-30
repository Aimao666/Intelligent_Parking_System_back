#pragma once
#include <string>
using namespace std;
class Parking
{
public:
    enum PARKINGTYPE
    {
        ENTRY=0,//入场
        LEAVE//出场
    };
    Parking() = default;
    //入出场构造
    Parking(PARKINGTYPE parkingType, string account, string carNumber, string time, string position, int picId = -1);
    //完整构造
    Parking(string account, string carNumber, int dueCost, int reallyCost, string entryTime, string entryPosition, int entryPicId,
        string leaveTime, string leavePosition, int leavePicId, int id = -1);
    ~Parking() = default;
private:
	int id;//序号
	int dueCost;//应收金额
	int reallyCost;//实收金额
	string entryTime;//入场时间
	string entryPosition;//入场位置
	int entryPicId;//入场图片id
	string leaveTime;//出场时间
	string leavePosition;//出场位置
	int leavePicId;//出场图片id
	string account;//所属用户
	string carNumber;//车牌号

public:
    int getId() const;
    void setId(int id);

    int getDueCost() const;
    void setDueCost(int dueCost);

    int getReallyCost() const;
    void setReallyCost(int reallyCost);

    string getEntryTime() const;
    void setEntryTime(string entryTime);

    string getEntryPosition() const;
    void setEntryPosition(string entryPosition);

    int getEntryPicId() const;
    void setEntryPicId(int entryPicId);

    string getLeaveTime() const;
    void setLeaveTime(string leaveTime);

    string getLeavePosition() const;
    void setLeavePosition(string leavePosition);

    int getLeavePicId() const;
    void setLeavePicId(int leavePicId);

    string getAccount() const;
    void setAccount(string account);

    string getCarNumber() const;
    void setCarNumber(string carNumber);

};

