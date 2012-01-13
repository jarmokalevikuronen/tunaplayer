#ifndef TPSEARCHFILTER_H
#define TPSEARCHFILTER_H

#include "tpsearchfiltercondition.h"
#include "tpsearchmatchcondition.h"
#include "tpassociative.h"
#include "tpassociativemeta.h"
#include "tpschemes.h"
#include "tpstoredprocedureargs.h"

class TPSearchFilter
{
private:

    class MatchObject
    {
    public:

        MatchObject()
        {
            matchKeyTarget = ValueTargetInvalid;
        }

        enum MatchValueTarget
        {
            ValueTargetInvalid,
            // value://
            ValueTargetValue,
            // parameter://
            ValueTargetParameter,
            // object://lastplayed
            ValueTargetObject
        } matchKeyTarget;

        QString matchKey;

        bool parseFrom(const QString line)
        {
            if (line.startsWith(schemeValue))
            {
                matchKey = line.mid(schemeValue.length());
                matchKeyTarget = ValueTargetValue;
            }
            else if (line.startsWith(schemeParameter))
            {
                matchKey = line.mid(schemeParameter.length());
                matchKeyTarget = ValueTargetParameter;
            }
            else if (line.startsWith(schemeObject))
            {
                matchKey = line.mid(schemeObject.length());
                matchKeyTarget = ValueTargetObject;
            }

            return (matchKey.length() > 0);
        }

        bool isValid() const
        {
            return (matchKeyTarget != ValueTargetInvalid);
        }
    };

public:



    TPSearchFilter()
    {
        condition = TPSearchConditionNone;
    }

    ~TPSearchFilter()
    {
        QVector<TPSearchFilter*>::iterator it = subFilters.begin();
        while (it != subFilters.end())
        {
            delete *it;
            ++it;
        }
    }

    inline const QString evaluateValue(const TPSearchFilter::MatchObject &matchObject, TPAssociativeObject &assocObject, TPStoredProcedureArgs &params) const
    {
        if (matchObject.matchKeyTarget == MatchObject::ValueTargetObject)
            return assocObject.getString(matchObject.matchKey);
        else if (matchObject.matchKeyTarget == MatchObject::ValueTargetParameter)
            return params.argValue(matchObject.matchKey);
        else if (matchObject.matchKeyTarget == MatchObject::ValueTargetValue)
            return matchObject.matchKey;

        return QString();
    }

    inline bool match(TPAssociativeObject &object, TPStoredProcedureArgs &parameters) const
    {
        const QString mv1 = evaluateValue(match1, object, parameters);
        const QString mv2 = evaluateValue(match2, object, parameters);

        return matchCondition(condition, mv1, mv2);
    }

    inline void addSubFilter(TPSearchFilter *filter)
    {
        if (filter && subFilters.indexOf(filter) < 0)
            subFilters.append(filter);
    }

    inline bool parseCondition(const QString conditionString)
    {
        return parseSearchCondition(conditionString, condition);
    }

    inline bool addMatchValue(const QString line)
    {
        if (!match1.isValid())
            return match1.parseFrom(line);
        else if (!match2.isValid())
            return match2.parseFrom(line);

        return false;
    }

    inline bool isComplete() const
    {
        return match1.isValid() && match2.isValid();
    }

    //! @brief Evaluates a given object
    //! @param object object to be evaluated
    //! @return true if matched, false if no match.
    bool evaluate(TPAssociativeObject &object, TPStoredProcedureArgs &parameters)
    {
        if (subFilters.count())
        {
            int trues = 0;
            int falses = 0;

            for (int i=0;i<subFilters.count();++i)
            {
                TPSearchFilter *filter = subFilters.at(i);

                if (filter->evaluate(object, parameters))
                {
                    if (criteria == TPSearchFilterCriteriaNone)
                        // None must pass but this does -> can be safely left away.
                        return false;
                    else if (criteria == TPSearchFilterCriteriaOne && trues > 0)
                        // At least two failures -> can be safely left away.
                        return false;
                    else if (criteria == TPSearchFilterCriteriaAny)
                        // At least one needed -> can be safely return true as one was in deed matched.
                        return true;

                    ++trues;
                }
                else
                {
                    if (criteria == TPSearchFilterCriteriaAll)
                        // All must match and this one failed -> can be safely left away
                        return false;

                    ++falses;
                }
            }

            if (criteria == TPSearchFilterCriteriaAll)
                return falses == 0;
            else if (criteria == TPSearchFilterCriteriaAny)
                return trues > 0;
            else if (criteria == TPSearchFilterCriteriaOne)
                return trues == 1;
            else if (criteria == TPSearchFilterCriteriaNone)
                return trues == 0;
            else
                return false;
        }

        return isComplete() ? match(object, parameters) : false;
    }


private:

    //! Condition as parsed
    TPSearchFilterCondition condition; // contains, starts, ..
    TPSearchFilterCriteria criteria; // all, one, none, any, ..

    MatchObject match1;
    MatchObject match2;

    QVector<TPSearchFilter *> subFilters;
};

#endif // TPSEARCHFILTER_H
