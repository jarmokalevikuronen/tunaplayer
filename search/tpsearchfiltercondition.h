#ifndef TPSEARCHFILTERCONDITION_H__
#define TPSEARCHFILTERCONDITION_H__

// FORWARD DECLARATIONS
class QString;


typedef enum TPSearchFilterCriteria
{
    TPSearchFilterCriteriaAll,
    TPSearchFilterCriteriaAny,
    TPSearchFilterCriteriaOne,
    TPSearchFilterCriteriaNone
}TPSearchFilterCriteria;

typedef enum TPSearchFilterCondition
{
    TPSearchConditionNone,
    TPSearchConditionContains,
    TPSearchConditionContainsCase,
    TPSearchConditionNotContains,
    TPSearchConditionNotContainsCase,
    TPSearchConditionEquals,
    TPSearchConditionEqualsCase,
    TPSearchConditionBegins,
    TPSearchConditionBeginsCase,
    TPSearchConditionEnds,
    TPSearchConditionEndsCase,
    TPSearchConditionExists,
    TPSearchConditionSmallerNum,
    TPSearchConditionBiggerNum,
    TPSearchConditionRegExp
}TPSearchFilterCondition;

bool parseSearchCondition(const QString stringCondition, TPSearchFilterCondition &condition);
bool parseSearchCriteria(const QString stringCriteria, TPSearchFilterCriteria &clause);


#endif
