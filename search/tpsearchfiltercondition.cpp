

#include <QString>
#include "tpsearchfiltercondition.h"

bool parseSearchCondition(const QString cond, TPSearchFilterCondition &condition)
{
    QString c = cond.toLower();

    if (!c.length())
        condition = TPSearchConditionNone;
    else if (c == "contains")
        condition = TPSearchConditionContains;
    else if (c == "contains-cs")
        condition = TPSearchConditionContainsCase;
    else if (c == "notcontains")
        condition = TPSearchConditionNotContains;
    else if (c == "notcontains-cs")
        condition = TPSearchConditionNotContainsCase;
    else if (c == "equals" || c == "==")
        condition = TPSearchConditionEquals;
    else if (c == "equals-cs")
        condition = TPSearchConditionEqualsCase;
    else if (c == "begins")
        condition = TPSearchConditionBegins;
    else if (c == "begins-cs")
        condition = TPSearchConditionBeginsCase;
    else if (c == "ends")
        condition = TPSearchConditionEnds;
    else if (c == "ends-cs")
        condition = TPSearchConditionEndsCase;
    else if (c == "exists")
        condition = TPSearchConditionExists;
    else if (c == "smaller" || c == "smallernum")
        condition = TPSearchConditionSmallerNum;
    else if (c == "bigger" || c == "biggernum")
        condition = TPSearchConditionBiggerNum;
    else if (c == "regexp")
        condition = TPSearchConditionRegExp;
    else
        return false;

    return true;
}

bool parseSearchCriteria(const QString stringCriteria, TPSearchFilterCriteria &criteria)
{
    QString c = stringCriteria.toLower();

    if (c == "any")
        criteria = TPSearchFilterCriteriaAny;
    else if (c == "all")
        criteria = TPSearchFilterCriteriaAll;
    else if (c == "one")
        criteria = TPSearchFilterCriteriaOne;
    else if (c == "none")
        criteria = TPSearchFilterCriteriaNone;
    else
        return false;

    return true;
}
