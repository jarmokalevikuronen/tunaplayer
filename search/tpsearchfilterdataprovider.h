#ifndef TPSEARCHFILTERFACADEINTERFACE_H
#define TPSEARCHFILTERFACADEINTERFACE_H

#include "tpassociative.h"
#include "tpsearchfacadedataproviderinterface.h"

class TPSearchFilterObjectProviderInterface
{
public:

    TPSearchFilterObjectProviderInterface(const QString __scheme)
    {
        _scheme = __scheme;
        Q_ASSERT(_scheme.length());
    }

    virtual ~TPSearchFilterObjectProviderInterface() {;}

    //! Getter for scheme information.
    inline const QString scheme()
    {
        return _scheme;
    }

    //! @brief Sets a list of schemes to whom the count+at should apply.
    //! Clears possibly previously set list.
    void setSchemes(QVariantList listOfSchemes);

    //! amount of items this facade providers
    virtual int count() = 0;
    //! getter for item
    TPAssociativeObject& at(int index) = 0;

    //! @brief Adds a data provider. Ownership transfers.
    void addProvider(TPSearchFacadeDataProviderInterface *provider);

private:

    //! Scheme cached.
    QString _scheme;

    QVector< TPSearchFacadeDataProviderInterface *> providers;
    QVector< TPSearchFacadeDataProviderInterface *> selectedProviders;

};


#endif // TPSEARCHFILTERFACADEINTERFACE_H
