#include <Promise.hpp>

Promise::Promise()
{
    QObject::connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}
