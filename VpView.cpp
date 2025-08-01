#include "VpView.h"

VpView::VpView()
{
}

VpView::VpView(string account, string totaltime, string currentPlaytime, string createtime, string vname, string vkhdPath,
    string pname, string pkhdPath) :account(account), totaltime(totaltime), currentPlaytime(currentPlaytime), createtime(createtime), 
    vname(vname), vkhdPath(vkhdPath), pname(pname), pkhdPath(pkhdPath)
{
}

int VpView::getId() const
{
    return id;
}

void VpView::setId(int id)
{
    this->id = id;
}

string VpView::getAccount() const
{
    return account;
}

void VpView::setAccount(string account)
{
    this->account = account;
}

string VpView::getTotaltime() const
{
    return totaltime;
}

void VpView::setTotaltime(string totaltime)
{
    this->totaltime = totaltime;
}

string VpView::getCurrentPlaytime() const
{
    return currentPlaytime;
}

void VpView::setCurrentPlaytime(string currentPlaytime)
{
    this->currentPlaytime = currentPlaytime;
}

string VpView::getCreatetime() const
{
    return createtime;
}

void VpView::setCreatetime(string createtime)
{
    this->createtime = createtime;
}

string VpView::getVname() const
{
    return vname;
}

void VpView::setVname(string vname)
{
    this->vname = vname;
}

string VpView::getVkhdPath() const
{
    return vkhdPath;
}

void VpView::setVkhdPath(string vkhdPath)
{
    this->vkhdPath = vkhdPath;
}

string VpView::getPname() const
{
    return pname;
}

void VpView::setPname(string pname)
{
    this->pname = pname;
}

string VpView::getPkhdPath() const
{
    return pkhdPath;
}

void VpView::setPkhdPath(string pkhdPath)
{
    this->pkhdPath = pkhdPath;
}

