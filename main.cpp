#include <cstdlib>
#include <cstdio>
#include <vector>
#include <cassert>
#include <cmath>

using std::vector;

enum BillType {normal, icash, network, oil};


class Bill
{
    public:
        Bill(int p_year, int p_month, int p_day, int p_amount, enum BillType p_type)
        :m_year(p_year), m_month(p_month), m_day(p_day), m_amount(p_amount), m_type(p_type)
        {}


        int getAmount() { return m_amount; }
        enum BillType getType() { return m_type;}
        void info()
        {
            printf("year = %d, month = %d, day = %d, amount = %d, type = %d\n",
                m_year, m_month, m_day, m_amount, m_type);
        }
    private:
        int m_year;
        int m_month;
        int m_day;
        int m_amount;
        enum BillType m_type;
};


class CredictCardBase
{
    public:
        CredictCardBase(int p_dueDate = 1)
        : m_dueDate(p_dueDate), m_maxDisCount(0)
        {}

        virtual ~CredictCardBase()
        {}

        int getMaxDisCount()
        {
            return m_maxDisCount;
        }
        virtual int getDisCount() = 0;

        void addPreAssignedBill(Bill* b)
        {
            m_preAssignedBillList.push_back(b);
        }
        
        void addAssignedBill(Bill* b)
        {
            m_assignedBillList.push_back(b);
        }

        void clearAssignedBill()
        {
            m_assignedBillList.clear();
        }

        void commitCurrentAssigned()
        {
            m_bestAssignedBillList.clear();
            for (size_t i = 0; i < m_preAssignedBillList.size(); i++) {
                m_bestAssignedBillList.push_back(m_preAssignedBillList[i]);
            }
            for (size_t i = 0; i < m_assignedBillList.size(); i++) {
                m_bestAssignedBillList.push_back(m_assignedBillList[i]);
            }
        }

        void dumpBestAssign()
        {
            for (size_t i = 0; i < m_bestAssignedBillList.size(); i++) {
                m_bestAssignedBillList[i]->info();
            }
        }
    protected:

        void _getMergeList(vector<Bill*>& mergeList)
        {
            mergeList.clear();
            for (size_t i = 0; i < m_preAssignedBillList.size(); i++) {
                mergeList.push_back(m_preAssignedBillList[i]);
            }
            for (size_t i = 0; i < m_assignedBillList.size(); i++) {
                mergeList.push_back(m_assignedBillList[i]);
            }
        }

        int m_dueDate;
        int m_maxDisCount;
        vector<Bill*> m_preAssignedBillList;
        vector<Bill*> m_assignedBillList;
        vector<Bill*> m_bestAssignedBillList;
};




class CredictCardHN : public CredictCardBase
{
    public:
        int getDisCount();
};


int CredictCardHN::getDisCount()
{
    vector<Bill*> tmpList;
    _getMergeList(tmpList);

    double disCount = 0;
    int sum = 0;
    const int cnt888 = 5;
    double list888[cnt888];
    int idx = 0;
    for (size_t i = 0; i < tmpList.size(); i++) {
        int val = tmpList[i]->getAmount();
        sum += val;
        if (val >= 888) {
            list888[idx] = val;
            idx++;
            if (idx == cnt888) {
                idx = 0;
                for (size_t j = 0; j < cnt888; j++) {
                    disCount += 0.007 * list888[idx];
                }
            }
        }
    }
    disCount += 0.008 * sum;

 //   printf("disCount of hn = %g\n", disCount);

    return (int) disCount;
}


class CredictCardYS : public CredictCardBase
{
    public:
        int getDisCount();
    protected:
        int _check699();
};



int CredictCardYS::_check699()
{
    vector<Bill*> tmpList;
    _getMergeList(tmpList);
    int cnt = 0;
    for (size_t i = 0; i < tmpList.size(); i++) {
        if (tmpList[i]->getAmount() >= 699 && tmpList[i]->getType() != icash) {
            cnt++;
        }
    }
    return cnt;
}


int CredictCardYS::getDisCount()
{
    vector<Bill*> tmpList;
    _getMergeList(tmpList);

    int cnt699 = _check699();
    double icashDisCount = 0;
    double netDisCount = 0;
    if (cnt699 >= 5) {
        for (size_t i = 0; i < tmpList.size(); i++) {
            if (tmpList[i]->getType() == icash) {
                icashDisCount += tmpList[i]->getAmount();
            }
            if (tmpList[i]->getType() == network) {
                netDisCount += tmpList[i]->getAmount();
            }
        }
        icashDisCount = 0.05 * icashDisCount;
        if (icashDisCount > 100) {
            icashDisCount = 100;
        }
        netDisCount = 0.05 * netDisCount;
        if (icashDisCount > 500) {
            icashDisCount = 500;
        }
    }
    
    double normalDisCount = 0;
    for (size_t i = 0; i < tmpList.size(); i++) {
        if (tmpList[i]->getType() == oil) {
            continue;
        }
        normalDisCount += tmpList[i]->getAmount();
    }
    normalDisCount = 0.007 * normalDisCount;
/*
    printf("disCount of ys= %g (normal = %g, icash = %g, net = %g)\n", 
        normalDisCount + icashDisCount + netDisCount,
        normalDisCount, icashDisCount, netDisCount);
*/        
    return normalDisCount + icashDisCount + netDisCount;
}



class CredictCardMgr
{
    public:
        CredictCardMgr()
        : m_maxDisCount(0)
        {
            srand(0);
        }
        void addBill(Bill *b, CredictCardBase *card);
        void addCard(CredictCardBase *card);
        void assignCard();
    private:
        vector<CredictCardBase*> m_credictCardList;
        vector<Bill*> m_billList;
        int m_maxDisCount;
};


void CredictCardMgr::addBill(Bill *b, CredictCardBase *card)
{
    if (card == NULL) {
        m_billList.push_back(b);
    }
    else {
        for (size_t i = 0; i < m_credictCardList.size(); i++) {
            if (m_credictCardList[i] == card) {
                m_credictCardList[i]->addPreAssignedBill(b);
                return;
            }
        }

        assert(0);
    }
}

void CredictCardMgr::addCard(CredictCardBase *card)
{
    m_credictCardList.push_back(card);
}


void CredictCardMgr::assignCard()
{
    size_t maxIter = pow(2,m_billList.size());
    for (size_t iter = 0; iter < maxIter ; iter++) {
        for (size_t i = 0; i < m_credictCardList.size(); i++) {
            m_credictCardList[i]->clearAssignedBill();
        }
        for (size_t i = 0; i < m_billList.size(); i++) {

            int randIdx = rand() % m_credictCardList.size();
            m_credictCardList[randIdx]->addAssignedBill(m_billList[i]);
        }
        int totalDisCount = 0;
        for (size_t i = 0; i < m_credictCardList.size(); i++) {
            totalDisCount += m_credictCardList[i]->getDisCount();
        }
        if (totalDisCount > m_maxDisCount) {
            m_maxDisCount = totalDisCount;
            for (size_t i = 0; i < m_credictCardList.size(); i++) {
                m_credictCardList[i]->commitCurrentAssigned();
            }
        }
    }
    printf("max disCount = %d\n", m_maxDisCount);
        
    for (size_t i = 0; i < m_credictCardList.size(); i++) {
        m_credictCardList[i]->dumpBestAssign();
        printf("==================================\n");
    }

}


int main()
{
    CredictCardBase *hnCard = new CredictCardHN;
    CredictCardBase *ysCard = new CredictCardYS;

    Bill b1(2015, 9, 23, 800, normal);
    Bill b2(2015, 9, 25, 1500, normal);
    Bill b3(2015, 9, 25, 500, icash);
    Bill b4(2015, 9, 25, 700, network);

    Bill a1(2015, 9, 23, 1000, normal);
    Bill a2(2015, 9, 24, 1000, normal);
    Bill a3(2015, 9, 25, 1000, normal);
    Bill a4(2015, 9, 26, 1000, normal);
    Bill a5(2015, 9, 27, 1000, normal);
    
    Bill c1(2015, 9, 13, 1000, normal);
    Bill c2(2015, 9, 14, 1000, normal);
    Bill c3(2015, 9, 15, 1000, normal);
    Bill c4(2015, 9, 16, 1000, normal);
    Bill c5(2015, 9, 17, 2000, network);
    Bill c6(2015, 9, 18, 500, icash);
    Bill c7(2015, 9, 19, 1000, oil);
    
    Bill d1(2015, 9, 20, 1000, network);

    CredictCardMgr cardMgr;
    cardMgr.addCard(hnCard);
    cardMgr.addCard(ysCard);
    
#if 0
    cardMgr.addBill(&a1, hnCard);
    cardMgr.addBill(&a2, hnCard);
    cardMgr.addBill(&a3, hnCard);
    cardMgr.addBill(&a4, hnCard);
    cardMgr.addBill(&a5, hnCard);
    
    cardMgr.addBill(&c1, ysCard);
    cardMgr.addBill(&c2, ysCard);
    cardMgr.addBill(&c3, ysCard);
    cardMgr.addBill(&c4, ysCard);
    cardMgr.addBill(&c5, ysCard);
    cardMgr.addBill(&c6, ysCard);
    cardMgr.addBill(&c7, ysCard);

    hnCard->getDisCount();
    ysCard->getDisCount();
#else
    cardMgr.addBill(&a1, hnCard);
    cardMgr.addBill(&a2, NULL);
    cardMgr.addBill(&a3, NULL);
    cardMgr.addBill(&a4, NULL);
    cardMgr.addBill(&a5, NULL);
    
    cardMgr.addBill(&c1, NULL);
    cardMgr.addBill(&c2, NULL);
    cardMgr.addBill(&c3, NULL);
    cardMgr.addBill(&c4, NULL);
    cardMgr.addBill(&c5, ysCard);
    cardMgr.addBill(&c6, ysCard);
    cardMgr.addBill(&c7, NULL);
    
    cardMgr.addBill(&d1, NULL);

    cardMgr.assignCard();
#endif
    delete hnCard;
    delete ysCard;

    return 0;
}
