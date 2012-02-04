#include "tpsortinterfacefactory.h"
#include "tpsortinterface.h"
#include "tpsort.h"

static const QString sortOrderAscending("ascending");
static const QString sortOrderDescending("descending");
static const QString sortOrderAscendingCase("ascending-cs");
static const QString sortOrderDescendingCase("descending-cs");
static const QString sortOrderAscendingNum("ascending-num");
static const QString sortOrderDescendingNum("descending-num");


TPSortInterface* TPSortInterfaceFactory::create(const QString _sortOrder, const QString _sortKey)
{
 if (_sortOrder == sortOrderAscending)
     return new TPSortImplementationString(Qt::CaseInsensitive, true, _sortKey);
 else if (_sortOrder == sortOrderDescending)
     return new TPSortImplementationString(Qt::CaseInsensitive, false, _sortKey);
 else if (_sortOrder == sortOrderAscendingCase)
     return new TPSortImplementationString(Qt::CaseSensitive, true, _sortKey);
 else if (_sortOrder == sortOrderDescendingCase)
     return new TPSortImplementationString(Qt::CaseSensitive, false, _sortKey);
 else if (_sortOrder == sortOrderAscendingNum)
     return new TPSortImplementationNum(true, _sortKey);
 else if (_sortOrder == sortOrderDescendingNum)
     return new TPSortImplementationNum(false, _sortKey);

 return NULL;
}


