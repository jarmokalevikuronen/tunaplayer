/*
This file is part of tunaplayer project
Copyright (C) 2012  Jarmo Kuronen <jarmok@iki.fi>

This is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This software is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this software; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "tpstoreprocedurerunner.h"
#include "tpsearchfilterevalargs.h"
#include "tpsearchresults.h"
#include "tplog.h"
#include <QTime>

static const QString keyResultCount("result-count");
static const QString keyResultStart("result-start");

TPStoredProcedureRunner::TPStoredProcedureRunner()
{
}

TPSearchResults* TPStoredProcedureRunner::execute(QVariantMap &args, TPStoredProcedure &sp, TPSearchObjectProvider &op)
{
    // Returns 0 in case some argument that is expected is missing.
    TPStoredProcedureArgs *arguments = sp.createArguments(args);
    if (!arguments)
    {
        ERROR() << "TSP: Invalid args";
        return 0;
    }

    op.setSchemes(sp.getSchemes(), sp.getProviderSelector(), arguments);

    bool ok = false;
    int maxResultCount = arguments->argValue(keyResultCount).toInt(&ok);
    if (!ok)
        maxResultCount = INT_MAX;

    int count = op.count();

    TPSearchResults *results = new TPSearchResults(sp.getSorter(), count+1);

    if (sp.getFilter())
    {
        for (int i=0;i<count;++i)
        {
            TPAssociativeObject *object = op.at(i);
            TPSearchFilterEvalArgs args(op.at(i), arguments);
            if (sp.getFilter()->evaluate(args))
                results->add(object);
        }
    }
    else
    {
        for (int i=0;i<count;++i)
            results->add(op.at(i));
    }

    // Finally, apply the requested sorting to results.
    results->sort();

    // Limit the result set to requested amount.
    results->limitTo(maxResultCount);

    delete arguments;

    return results;
}


