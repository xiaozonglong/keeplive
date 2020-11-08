#ifndef FRMMAIN_H
#define FRMMAIN_H

#include <QWidget>

namespace Ui {
class frmMain;
}

class frmMain : public QWidget
{
    Q_OBJECT

public:
    explicit frmMain(QWidget *parent = 0);
    ~frmMain();

private slots:
    void on_pushButton_released();

    void on_pushButton_2_released();

private:
    Ui::frmMain *ui;
    void initLock();
};

#endif // FRMMAIN_H
