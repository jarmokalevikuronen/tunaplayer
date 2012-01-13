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

#ifndef TPSEARCHFILTEREVALARGS_H
#define TPSEARCHFILTEREVALARGS_H

#include "tpassociative.h"
#include "tpstoredprocedureargs.h"

class TPSearchFilterEvalArgs
{
public:

    TPSearchFilterEvalArgs();
    TPSearchFilterEvalArgs(TPAssociativeObject *_object, TPStoredProcedureArgs *_args);

    inline TPAssociativeObject* object() const
    {
        return _object;
    }

    inline TPStoredProcedureArgs* args() const
    {
        return _args;
    }

private:

    Q_DISABLE_COPY(TPSearchFilterEvalArgs);

    //! Object pointer - quaranteed to be not null when evaluated.
    TPAssociativeObject *_object;

    //! Stored procedure arguments - quaranteed not to be null when evaluated.<
    TPStoredProcedureArgs *_args;
};

#endif // TPSEARCHFILTEREVALARGS_H
