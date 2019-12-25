#ifndef CTKEVENTBUSDEMOMAINWINDOW_H
#define CTKEVENTBUSDEMOMAINWINDOW_H

#include <QMainWindow>
#include <QVariant>

//class ctkEventAdminBus;

namespace Ui {
    class ctkAppMainWindow;
}

class ctkEventDemo : public QObject {
    Q_OBJECT

Q_SIGNALS:
    void receiveEventSignal(QVariantList l);
    void updateMessageSignal(QString message);

public Q_SLOTS:
    void receiveEvent(QVariantList l);
};

class ctkAppMainWindow : public QMainWindow {
    Q_OBJECT
public:
    ctkAppMainWindow(QWidget *parent = 0);
   // ctkAppMainWindow(ctkEventAdminBus *bus, QWidget *parent = 0);
    ~ctkAppMainWindow();

public Q_SLOTS:
    void sendEvent();
    void updateMessage(QString message);
    void connectClient();

protected:
    void changeEvent(QEvent *e);
 //   void connectEvents();

private:
    Ui::ctkAppMainWindow *ui;
   // ctkEventAdminBus *m_EventBus;

    ctkEventDemo *handler;
};

#endif // CTKEVENTBUSDEMOMAINWINDOW_H
