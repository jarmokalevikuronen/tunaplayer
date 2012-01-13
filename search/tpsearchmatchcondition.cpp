
#include "tpsearchmatchcondition.h"
#include <QString>
#include <QRegExp>

bool matchCondition(TPSearchFilterCondition condition, const QString value1, const QString value2)
{
    bool result = true;

    switch (condition)
    {
        case TPSearchConditionContains:
            result = value1.contains(value2, Qt::CaseInsensitive);
            break;

        case TPSearchConditionContainsCase:
            result = value1.contains(value2, Qt::CaseSensitive);
            break;

        case TPSearchConditionEquals:
            result = value1.compare(value2, Qt::CaseInsensitive) == 0;
            break;

        case TPSearchConditionEqualsCase:
            result = value1.compare(value2, Qt::CaseSensitive);
            break;

        case TPSearchConditionBegins:
            result = value1.startsWith(value2, Qt::CaseInsensitive);
            break;

        case TPSearchConditionBeginsCase:
            result = value1.startsWith(value2, Qt::CaseSensitive);
            break;

        case TPSearchConditionEnds:
            result = value1.endsWith(value2, Qt::CaseInsensitive);
            break;

        case TPSearchConditionEndsCase:
            result = value1.endsWith(value2, Qt::CaseSensitive);
            break;

        case TPSearchConditionExists:
            result = value1.length() > 0;
            break;

        case TPSearchConditionSmallerNum:
        {
            bool ok1;
            bool ok2;
            qlonglong number1 = value1.toLongLong(&ok1);
            qlonglong number2 = value2.toLongLong(&ok2);

            if (ok1 && ok2)
                result = number1 < number2;
            else if (ok1)
                result = true;
            else
                result = false;
            }
            break;

        case TPSearchConditionBiggerNum:
        {
            bool ok1;
            bool ok2;
            qlonglong number1 = value1.toLongLong(&ok1);
            qlonglong number2 = value2.toLongLong(&ok2);

            if (ok1 && ok2)
                result = number1 > number2;
            else if (ok2)
                result = true;
            else
                result = false;
            }
            break;

        case TPSearchConditionRegExp:
        {
            // TODO: Very poor performance -> do remove or implement better.
            QRegExp re(value1);
            result = (re.indexIn(value2) >= 0);
        }
        break;
    }

    return result;
}
