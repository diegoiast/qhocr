#include "imageoptionscontrol.h"
#include "ui_imageoptionscontrol.h"

ImageOptionsControl::ImageOptionsControl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ImageOptionsControl)
{
    ui->setupUi(this);
}

ImageOptionsControl::~ImageOptionsControl()
{
    delete ui;
}
