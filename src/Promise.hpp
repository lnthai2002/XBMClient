#include <QObject>

#ifndef PROMISE_HPP_
#define PROMISE_HPP_
class Promise : public QObject
{
    Q_OBJECT

    public:
        Promise();

        signals:
        void finished();
        //void canceled();
};

#endif /* PROMISE_HPP_ */
