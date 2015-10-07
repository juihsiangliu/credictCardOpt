#include <cstdlib>
#include <cstdio>
#include <vector>
#include <cassert>
#include <cmath>
#include <cstring>

using std::vector;

enum BillType {normal, icash, network, oil};


class Bill
{
    public:
        Bill(int p_year, int p_month, int p_day, int p_amount, enum BillType p_type, const char* p_comment = NULL)
            :m_year(p_year), m_month(p_month), m_day(p_day), m_amount(p_amount), m_type(p_type)
        {
            memset(m_comment, 0, 1024);
            if (p_comment == NULL) {
                strcpy(m_comment, "null");
            }
            else {
                strcpy(m_comment, p_comment);
            }
        }


        int getAmount() { return m_amount; }
        enum BillType getType() { return m_type;}
        void info()
        {
            printf("year = %d, month = %d, day = %d, amount = %d, type = %d, comment = %s\n",
                    m_year, m_month, m_day, m_amount, m_type, m_comment);
        }
    private:
        int m_year;
        int m_month;
        int m_day;
        int m_amount;
        enum BillType m_type;
        char m_comment[1024];
};


class CredictCardBase
{
    public:
        CredictCardBase(const char* p_name, int p_dueDate = 1)
            : m_dueDate(p_dueDate), m_commitDisCount(0)
        {
            memset(m_name, 0, 1024);
            if (p_name == NULL) {
                strcpy(m_name, "null");
            }
            else {
                strcpy(m_name, p_name);
            }
        }

        virtual ~CredictCardBase()
        {}

        virtual int getDisCount() = 0;

        void addPreAssignBill(Bill* b)
        {
            m_preAssignBillList.push_back(b);
        }

        void addAssignBill(Bill* b)
        {
            m_assignBillList.push_back(b);
        }

        void clearAssignBill()
        {
            m_assignBillList.clear();
        }

        int getDisCountForCommit() 
        {
            return m_commitDisCount;
        }
        void commitCurrentAssign()
        {
            m_bestAssignBillList.clear();
            for (size_t i = 0; i < m_preAssignBillList.size(); i++) {
                m_bestAssignBillList.push_back(m_preAssignBillList[i]);
            }
            for (size_t i = 0; i < m_assignBillList.size(); i++) {
                m_bestAssignBillList.push_back(m_assignBillList[i]);
            }
            m_commitDisCount = getDisCount();
        }

        void dumpBestAssign()
        {
            printf("credict card = %s\n", m_name);
            printf("disCount = %d\n", getDisCountForCommit());
            for (size_t i = 0; i < m_bestAssignBillList.size(); i++) {
                m_bestAssignBillList[i]->info();
            }
        }
    protected:

        void _getMergeList(vector<Bill*>& mergeList)
        {
            mergeList.clear();
            for (size_t i = 0; i < m_preAssignBillList.size(); i++) {
                mergeList.push_back(m_preAssignBillList[i]);
            }
            for (size_t i = 0; i < m_assignBillList.size(); i++) {
                mergeList.push_back(m_assignBillList[i]);
            }
        }

        int m_dueDate;
        int m_commitDisCount;
        
        vector<Bill*> m_preAssignBillList;
        vector<Bill*> m_assignBillList;
        vector<Bill*> m_bestAssignBillList;
        char m_name[1024];
};




class CredictCardHN : public CredictCardBase
{
    public:
        CredictCardHN(const char* p_name)
            : CredictCardBase(p_name)
        {}
        int getDisCount();
};


int CredictCardHN::getDisCount()
{
    vector<Bill*> tmpList;
    _getMergeList(tmpList);

    int normalSum = 0;
    const int cnt888 = 5;
    double sum888DisCount = 0;
    double list888[cnt888];
    int idx = 0;
    const int th = 12000;
    for (size_t i = 0; i < tmpList.size(); i++) {
        int val = tmpList[i]->getAmount();
        normalSum += val;
        // for special event
        const double maxEach = 100;
        const double maxTotal = 500;
        if (val >= 888 && val < th) {
            list888[idx] = val;
            idx++;
            if (idx == cnt888) {
                idx = 0;
                double n = 0;
                for (int j = 0; j < cnt888; j++) {
                    n += 0.007 * list888[j];
                    list888[j] = 0;
                }
                if (n > maxEach) {
                    n = maxEach;
                }
                sum888DisCount += n;
            }
            else {
                // check if subtotal > th
                double subtotal = 0;
                for (int j = 0; j < idx; j++) {
                    subtotal += list888[j];
                }
                double n = 0;
                if (subtotal >= th) {
                    n += 0.007 * subtotal;
                    if (n > maxEach) {
                        n = maxEach;
                    }
                    for (int j = 0; j < idx; j++) {
                        list888[j] = 0;
                    }
                    idx = 0;
                }
                sum888DisCount += n;
            }
        }
        else if (val >= th) {
            double n = 0.007 * val;
            if (n > maxEach) {
                n = maxEach;
            }
            sum888DisCount += n;
        }
    
        if (sum888DisCount > maxTotal) {
            sum888DisCount = maxTotal;
        }
    }
    
    double disCount = 0;
    disCount += sum888DisCount;
    disCount += 0.008 * normalSum;

    //   printf("disCount of hn = %g\n", disCount);

    return (int) disCount;
}


class CredictCardYS : public CredictCardBase
{
    public:
        CredictCardYS(const char* p_name)
            : CredictCardBase(p_name)
        {}
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
        if (netDisCount > 300) {
            netDisCount = 300;
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
//        printf("disCount of ys= %g (normal = %g, icash = %g, net = %g)\n", 
//            normalDisCount + icashDisCount + netDisCount,
//            normalDisCount, icashDisCount, netDisCount);
    return (int) (normalDisCount + icashDisCount + netDisCount);
}


class CredictCardHNICash : public CredictCardBase
{
    public:
        CredictCardHNICash(const char* p_name)
            : CredictCardBase(p_name)
        {}
        int getDisCount();
};


int CredictCardHNICash::getDisCount()
{
    vector<Bill*> tmpList;
    _getMergeList(tmpList);
    double sum = 0;
    for (size_t i = 0; i < tmpList.size(); i++) {
        if (tmpList[i]->getType() != network) {
            continue;
        }
        sum += tmpList[i]->getAmount();
    }
    
    return (int) (0.025 * sum);
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
                m_credictCardList[i]->addPreAssignBill(b);
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
    size_t maxIter = (size_t) (2 * pow(m_credictCardList.size(), m_billList.size()));
    //    printf("maxIter = %d (base = %d, exp = %d)\n", maxIter, m_credictCardList.size(), m_billList.size());

    for (size_t iter = 0; iter < maxIter ; iter++) {
        for (size_t i = 0; i < m_credictCardList.size(); i++) {
            m_credictCardList[i]->clearAssignBill();
        }
        for (size_t i = 0; i < m_billList.size(); i++) {

            int randIdx = rand() % m_credictCardList.size();
            m_credictCardList[randIdx]->addAssignBill(m_billList[i]);
        }
        int totalDisCount = 0;
        for (size_t i = 0; i < m_credictCardList.size(); i++) {
            totalDisCount += m_credictCardList[i]->getDisCount();
        }
        if (totalDisCount > m_maxDisCount) {
            m_maxDisCount = totalDisCount;
            for (size_t i = 0; i < m_credictCardList.size(); i++) {
                m_credictCardList[i]->commitCurrentAssign();
            }
        }
    }
    printf("max disCount = %d\n", m_maxDisCount);

    printf("==================================\n");
    for (size_t i = 0; i < m_credictCardList.size(); i++) {
        m_credictCardList[i]->dumpBestAssign();
        printf("==================================\n");
    }

}


int main()
{
    CredictCardMgr cardMgr;
    
    CredictCardHN hnCard("hnCard");
//    CredictCardHNICash hnICashCard("hnICashCard");
    CredictCardYS ysCard("ys icash");

    cardMgr.addCard(&hnCard);
//    cardMgr.addCard(&hnICashCard);
    cardMgr.addCard(&ysCard);

    Bill a1(2015, 9, 20, 500, icash, "regular icash");
    Bill a2(2015, 9, 25, 1317, network, "pchome - normal");
    Bill a3(2015, 9, 30, 528, normal, "baby1");
    Bill a4(2015, 9, 30, 853, normal, "baby2");
    Bill a5(2015, 10, 4, 1980, normal, "baby-cake");
    Bill a6(2015, 10, 4, 1200, normal, "birthday dinner");
    Bill a7(2015, 10, 4, 725, network, "pchome - baby3");
    Bill a8(2015, 10, 7, 1171, network, "books");

    Bill b1(2015, 10, 100, 1000, oil, "regular oil1");
//    Bill a8(2015, 9, 100, 2450, network, "iphone");

#if 0
    cardMgr.addBill(&a1, &ysCard);
    cardMgr.addBill(&a2, &ysCard);
    cardMgr.addBill(&a3, &ysCard);
    cardMgr.addBill(&a4, &ysCard);
    cardMgr.addBill(&a5, &hnCard);
    cardMgr.addBill(&a6, &ysCard);
    cardMgr.addBill(&a7, &ysCard);
    cardMgr.addBill(&a8, &ysCard);
    cardMgr.addBill(&b1, NULL);
#else    
    cardMgr.addBill(&a1, NULL);
    cardMgr.addBill(&a2, NULL);
    cardMgr.addBill(&a3, NULL);
    cardMgr.addBill(&a4, NULL);
    cardMgr.addBill(&a5, NULL);
    cardMgr.addBill(&a6, NULL);
    cardMgr.addBill(&a7, NULL);
    cardMgr.addBill(&a8, NULL);
    cardMgr.addBill(&b1, NULL);
#endif
    cardMgr.assignCard();

    return 0;
}

