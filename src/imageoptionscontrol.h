#ifndef IMAGEOPTIONSCONTROL_H
#define IMAGEOPTIONSCONTROL_H

#include <QWidget>

namespace Ui {
    class ImageOptionsControl;
}

class ImageOptionsControl : public QWidget
{
    Q_OBJECT

public:
    explicit ImageOptionsControl(QWidget *parent = 0);
    ~ImageOptionsControl();

private:
    Ui::ImageOptionsControl *ui;
};

#endif // IMAGEOPTIONSCONTROL_H
