#pragma once
#include <string>
using namespace std;
class ParkingPictureView
{
public:
	ParkingPictureView() = default;
	~ParkingPictureView() = default;
private:
	int id;//序号
	int dueCost;//应收金额
	int reallyCost;//实收金额
	string entryTime;//入场时间
	string entryPosition;//入场位置
	int entryPicId;//入场图片id
	string entryPicName;//入场图片名称
	string entryPicKhdPath;//入场图片路径
	string leaveTime;//出场时间
	string leavePosition;//出场位置
	int leavePicId;//出场图片id
	string leavePicName;//出场图片名称
	string leavePicKhdPath;//出场图片路径
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

    string getEntryPicName() const;
    void setEntryPicName(string entryPicName);

    string getEntryPicKhdPath() const;
    void setEntryPicKhdPath(string entryPicKhdPath);

    string getLeaveTime() const;
    void setLeaveTime(string leaveTime);

    string getLeavePosition() const;
    void setLeavePosition(string leavePosition);

    int getLeavePicId() const;
    void setLeavePicId(int leavePicId);

    string getLeavePicName() const;
    void setLeavePicName(string leavePicName);

    string getLeavePicKhdPath() const;
    void setLeavePicKhdPath(string leavePicKhdPath);

    string getAccount() const;
    void setAccount(string account);

    string getCarNumber() const;
    void setCarNumber(string carNumber);

};

