#include "carditem.h"
#include "engine.h"

#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QFocusEvent>
#include <QParallelAnimationGroup>

CardItem::CardItem(const Card *card)
    :Pixmap(card->getPixmapPath(), false), card(card), equipped(false)
{
    Q_ASSERT(card != NULL);

    suit_pixmap.load(QString(":/suit/%1.png").arg(card->getSuitString()));
    pixmap = pixmap.scaled(150*0.8, 210*0.8);
    setFlags(ItemIsFocusable);
    setTransformOriginPoint(pixmap.width()/2, pixmap.height()/2);
}

const Card *CardItem::getCard() const{
    return card;
}

void CardItem::setHomePos(QPointF home_pos){
    this->home_pos = home_pos;
}

void CardItem::goBack(bool kieru){
    QPropertyAnimation *goback = new QPropertyAnimation(this, "pos");
    goback->setEndValue(home_pos);
    goback->setEasingCurve(QEasingCurve::OutBounce);

    if(kieru){
        QParallelAnimationGroup *group = new QParallelAnimationGroup;

        QPropertyAnimation *disappear = new QPropertyAnimation(this, "opacity");
        disappear->setEndValue(0.0);

        group->addAnimation(goback);
        group->addAnimation(disappear);

        group->start(QParallelAnimationGroup::DeleteWhenStopped);
    }else
        goback->start(QPropertyAnimation::DeleteWhenStopped);
}

const QPixmap &CardItem::getSuitPixmap() const{
    return suit_pixmap;
}

void CardItem::select(){
    setY(10);
}

void CardItem::unselect(){   
    setY(45);
}

bool CardItem::isEquipped() const{
    return equipped;
}

void CardItem::setEquipped(bool equipped){
    this->equipped = equipped;
}

void CardItem::mousePressEvent(QGraphicsSceneMouseEvent *event){
    if(hasFocus()){
        setOpacity(0.8);
        emit card_selected(this);
    }else if(rotation() != 0.0)
        emit show_discards();
    else
        emit hide_discards();
}

void CardItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){
    if(parentItem())
        emit pending(this, y() < -80);

    setOpacity(1.0);
    goBack();
}

void CardItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event){
    if(hasFocus()){
        setPos(this->mapToParent(event->pos()) - event->buttonDownPos(Qt::LeftButton));
    }
}

void CardItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    Pixmap::paint(painter, option, widget);

    static QRect suit_rect(8,8,18,18);
    static QFont card_number_font("Times", 20, QFont::Bold);

    painter->drawPixmap(suit_rect, suit_pixmap);

    painter->setFont(card_number_font);
    if(card->isRed())
        painter->setPen(Qt::red);
    painter->drawText(8, 50, card->getNumberString());
}
