#include "widget.h"
#include "ui_widget.h"

#include <QDebug>
#include <QDir>
#include <QScroller>
#include <QEasingCurve>
#include <QScrollEvent>
#include <QScrollBar>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    ui->listWidget->setFocusPolicy(Qt::NoFocus);
    ui->listWidget->setDragDropMode(QAbstractItemView::NoDragDrop);

    ui->listWidget->installEventFilter(this);
    const QFileInfoList iconFiles = QDir(":/icons").entryInfoList();
    for(int i = 0; i < 20; i++)
    {
        for(const QFileInfo & info : iconFiles)
        {
            const QString path = info.absoluteFilePath();
            QIcon icon(path);
            icon.addFile(path, QSize(), QIcon::Selected);
            new QListWidgetItem(QIcon(info.absoluteFilePath()), info.baseName(), ui->listWidget);
        }
    }

    QScrollerProperties scrollerProps;

    // 鼠标按下后，目标收到鼠标按下事件的延迟时间，单位s。如果在延迟时间内开始了拖拽，目标不会收到鼠标事件
    scrollerProps.setScrollMetric(QScrollerProperties::MousePressEventDelay, 1);

    // 鼠标按下后开始移动到被判定为拖拽操作的距离，单位m
    scrollerProps.setScrollMetric(QScrollerProperties::DragStartDistance, 0.003);

    // 似乎是影响速度的计算，平滑拖拽到自动滚动的速度
    scrollerProps.setScrollMetric(QScrollerProperties::DragVelocitySmoothingFactor, 1);

    // 未测试
//    scrollerProps.setScrollMetric(QScrollerProperties::AxisLockThreshold, 0.1);

    // 滚动曲线和减速影响因子，具体应该配合曲线。因为可以自定义曲线，所以曲线必须构造出QEasingCurve，
    scrollerProps.setScrollMetric(QScrollerProperties::ScrollingCurve, QEasingCurve(QEasingCurve::OutQuad));
    scrollerProps.setScrollMetric(QScrollerProperties::DecelerationFactor, 0.3);

    // 当鼠标抬起后，触发自动滚动的最小速度、以及能达到的最大速度，单位m/s
    scrollerProps.setScrollMetric(QScrollerProperties::MinimumVelocity, 0.003);
    scrollerProps.setScrollMetric(QScrollerProperties::MaximumVelocity, 1);

    // 当鼠标抬起后，点击操作会停止当前滚动，滚动速度超过该速度时，鼠标点击不会传递给目标，单位m/s
    scrollerProps.setScrollMetric(QScrollerProperties::MaximumClickThroughVelocity, 0.06);

    // 当鼠标抬起后自动滚动，在该时间内的加速滑动手势会加速当前滚动，单位s, 可以设置加速后速度比例，必须大于1
    scrollerProps.setScrollMetric(QScrollerProperties::AcceleratingFlickMaximumTime, 2);
    scrollerProps.setScrollMetric(QScrollerProperties::AcceleratingFlickSpeedupFactor, 1.5);

    // 不是很理解这两个
//    scrollerProps.setScrollMetric(QScrollerProperties::SnapPositionRatio, 0.1);
//    scrollerProps.setScrollMetric(QScrollerProperties::SnapTime, 4);

    scrollerProps.setScrollMetric(QScrollerProperties::OvershootDragResistanceFactor, 0.2); // 过量拖拽时，实际拖拽距离与鼠标移动距离的比例，越小越阻塞，范围0-1
    scrollerProps.setScrollMetric(QScrollerProperties::OvershootDragDistanceFactor, 0.5);   // 过量拖拽时，最大可拖拽距离占窗口的比例，范围0-1
    scrollerProps.setScrollMetric(QScrollerProperties::OvershootScrollDistanceFactor, 0.2);   // 允许的自动滚动时的过量滚动比例，同上
    scrollerProps.setScrollMetric(QScrollerProperties::OvershootScrollTime, 0.3);   // 过量拖拽后释放，恢复到初始状态的时间，单位s

     // 设置允许过量拖拽的策略，滚动条出现时开启、始终关闭、始终开启
    scrollerProps.setScrollMetric(QScrollerProperties::HorizontalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
    scrollerProps.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, QScrollerProperties::OvershootAlwaysOn);
    scrollerProps.setScrollMetric(QScrollerProperties::FrameRate, QScrollerProperties::Fps60);


    QScroller *scroller = QScroller::scroller(ui->listWidget);
    scroller->grabGesture(ui->listWidget,QScroller::LeftMouseButtonGesture);
    scroller->setScrollerProperties(scrollerProps);
}

Widget::~Widget()
{
    delete ui;
}

bool Widget::eventFilter(QObject *watched, QEvent *event)
{
    return false;
}

