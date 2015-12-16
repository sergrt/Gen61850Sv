#include "VirtualKeyboard.h"
#include "mainwindow.h"
#include <QDebug>

const char keyCodeProperty[] = "keyCodeProperty";
const QString BUTTON_QSS = "QPushButton { background: #E0E0E0; border: 1px solid gray; }";

#include <QTextCodec>
QVirtualKeyboard::QVirtualKeyboard(QWidget *parent)
    : QWidget(parent, Qt::Window | Qt::CustomizeWindowHint | Qt::FramelessWindowHint /*| Qt::WindowTitleHint*/ | Qt::WindowStaysOnTopHint) {
    ui.setupUi(this);
    keyLayout = VIRTUALKEYBOARD_LAYOUT_EN;
    QFont font = this->font();
    font.setPixelSize(18);
    this->setFont(font);
    setAllButtonFont(this, font);
    
    shiftPressed = false;

    bnSpecialRestore = new QPushButton(this);
    bnSpecialRestore->setObjectName("bnSpecialRestore");
    QIcon img(":/GenSv/Resources/keyboard.gif");
    bnSpecialRestore->setIcon(img);
    //bnSpecialRestore->setFont(font);
    bnSpecialRestore->setStyleSheet(BUTTON_QSS);

    //bnSpecialRestore->setStyleSheet("QPushButton { border: 1px solid black; background-color: " + palette().color(QPalette::Dark).name() + ";}");
    bnSpecialRestore->setVisible(false);	
    bnSpecialRestore->setProperty(keyCodeProperty, Qt::Key_Escape);

    // Connect all buttorn to onBnClicked
    connectAllButtons(this);
    setAllButtonCodes();
    setAllButtonLabels(this);

    QObject::connect(this, &QVirtualKeyboard::onKeyPressed, (MainWindow*)parent, &MainWindow::onKeyPressed);

    //setFixedSize(size());
}

void QVirtualKeyboard::connectAllButtons(QObject* obj) {
    QObjectList ql = obj->children();
    for (int i = 0; i < ql.count(); i++) {		
        if (ql[i]->isWidgetType()) {
            connectAllButtons(ql[i]);
            QWidget * w = dynamic_cast<QWidget*>(ql[i]);
            if (w) {
                const QString wName = w->metaObject()->className(); 
                //qDebug( w->metaObject()->className() );
                if (wName == "QPushButton") {
                    QObject::connect((QPushButton*)w, &QPushButton::clicked, this, &QVirtualKeyboard::onBnClicked);
                }
            }
        }
    }
}

void QVirtualKeyboard::setAllButtonFont(QObject* obj, const QFont& font) {
    QObjectList ql = obj->children();
    for (int i = 0; i < ql.count(); i++) {		
        if (ql[i]->isWidgetType()) {
            setAllButtonFont(ql[i], font);
            QWidget * w = dynamic_cast<QWidget*>(ql[i]);
            if (w) {
                const QString wName = w->metaObject()->className();
                //qDebug( w->metaObject()->className() );
                if (wName == "QPushButton") {
                    w->setFont(font);
                    w->setStyleSheet(BUTTON_QSS);
                }
            }
        }
    }
}

QVirtualKeyboard::~QVirtualKeyboard() {
    delete bnSpecialRestore;
}

void QVirtualKeyboard::setAllButtonCodes() {
    switch(keyLayout) {
        case VIRTUALKEYBOARD_LAYOUT_EN:	
            ui.bnMinus->setProperty(keyCodeProperty, shiftPressed ?  95 :  45); //_-
            ui.bnEqual->setProperty(keyCodeProperty, shiftPressed ?  43 :  61); //+=
            ui.bnTilde->setProperty(keyCodeProperty, shiftPressed ? 126 :  96); //~`
            ui.bn0->setProperty(keyCodeProperty,     shiftPressed ?  41 :  48); //)0
            ui.bn1->setProperty(keyCodeProperty,     shiftPressed ?  33 :  49); //!1
            ui.bn2->setProperty(keyCodeProperty,     shiftPressed ?  64 :  50); //@2
            ui.bn3->setProperty(keyCodeProperty,     shiftPressed ?  35 :  51); //#3
            ui.bn4->setProperty(keyCodeProperty,     shiftPressed ?  36 :  52); //$4
            ui.bn5->setProperty(keyCodeProperty,     shiftPressed ?  37 :  53); //%5
            ui.bn6->setProperty(keyCodeProperty,     shiftPressed ?  94 :  54); //^6
            ui.bn7->setProperty(keyCodeProperty,     shiftPressed ?  38 :  55); //&7
            ui.bn8->setProperty(keyCodeProperty,     shiftPressed ?  42 :  56); //*8
            ui.bn9->setProperty(keyCodeProperty,     shiftPressed ?  40 :  57); //(9

            ui.bnQ->setProperty(keyCodeProperty,     shiftPressed ?  81 : 113); //Qq
            ui.bnW->setProperty(keyCodeProperty,     shiftPressed ?  87 : 119); //Ww
            ui.bnE->setProperty(keyCodeProperty,     shiftPressed ?  69 : 101); //Ee
            ui.bnR->setProperty(keyCodeProperty,     shiftPressed ?  82 : 114); //Rr
            ui.bnT->setProperty(keyCodeProperty,     shiftPressed ?  84 : 116); //Tt
            ui.bnY->setProperty(keyCodeProperty,     shiftPressed ?  89 : 121); //Yy
            ui.bnU->setProperty(keyCodeProperty,     shiftPressed ?  85 : 117); //Uu
            ui.bnI->setProperty(keyCodeProperty,     shiftPressed ?  73 : 105); //Ii
            ui.bnO->setProperty(keyCodeProperty,     shiftPressed ?  79 : 111); //Oo
            ui.bnP->setProperty(keyCodeProperty,     shiftPressed ?  80 : 112); //Pp
            ui.bnLSB->setProperty(keyCodeProperty,   shiftPressed ? 123 :  91); //{[
            ui.bnRSB->setProperty(keyCodeProperty,   shiftPressed ? 125 :  93); //}]
    
            ui.bnBackslash->setProperty(keyCodeProperty,  shiftPressed ? 124 :  92); //|"\"
            ui.bnA->setProperty(keyCodeProperty,          shiftPressed ?  65 :  97); //Aa
            ui.bnS->setProperty(keyCodeProperty,          shiftPressed ?  83 : 115); //Ss
            ui.bnD->setProperty(keyCodeProperty,          shiftPressed ?  68 : 100); //Dd
            ui.bnF->setProperty(keyCodeProperty,          shiftPressed ?  70 : 102); //Ff
            ui.bnG->setProperty(keyCodeProperty,          shiftPressed ?  71 : 103); //Gg
            ui.bnH->setProperty(keyCodeProperty,          shiftPressed ?  72 : 104); //Hh
            ui.bnJ->setProperty(keyCodeProperty,          shiftPressed ?  74 : 106); //Jj
            ui.bnK->setProperty(keyCodeProperty,          shiftPressed ?  75 : 107); //Kk
            ui.bnL->setProperty(keyCodeProperty,          shiftPressed ?  76 : 108); //Ll
            ui.bnSemicolon->setProperty(keyCodeProperty,  shiftPressed ?  58 :  59); //:;
            ui.bnApostrophe->setProperty(keyCodeProperty, shiftPressed ?  34 :  39); //"'
            ui.bnSlash->setProperty(keyCodeProperty,      shiftPressed ?  63 :  47); //?/
    
            //Shift key does not need keyCodeProperty
            ui.bnSpecialLayout->setProperty(keyCodeProperty, Qt::Key_Mode_switch);
            ui.bnZ->setProperty(keyCodeProperty,          shiftPressed ?  90 : 122); //Zz
            ui.bnX->setProperty(keyCodeProperty,          shiftPressed ?  88 : 120); //Xx
            ui.bnC->setProperty(keyCodeProperty,          shiftPressed ?  67 :  99); //Cc
            ui.bnV->setProperty(keyCodeProperty,          shiftPressed ?  86 : 118); //Vv
            ui.bnB->setProperty(keyCodeProperty,          shiftPressed ?  66 :  98); //Bb
            ui.bnN->setProperty(keyCodeProperty,          shiftPressed ?  78 : 110); //Nn
            ui.bnM->setProperty(keyCodeProperty,          shiftPressed ?  77 : 109); //Mm
            ui.bnComma->setProperty(keyCodeProperty,      shiftPressed ?  60 :  44); //<,
            ui.bnDot->setProperty(keyCodeProperty,        shiftPressed ?  62 :  46); //>.
            ui.bnSpecialShift->setProperty(keyCodeProperty, Qt::Key_Shift);

            ui.bnSpecialHide->setProperty(keyCodeProperty, Qt::Key_Escape);
            ui.bnSpace->setProperty(keyCodeProperty, Qt::Key_Space);
            ui.bnLeft->setProperty(keyCodeProperty, Qt::Key_Left);
            ui.bnRight->setProperty(keyCodeProperty, Qt::Key_Right);
            ui.bnBackspace->setProperty(keyCodeProperty, Qt::Key_Backspace);	
            
            break;
        case VIRTUALKEYBOARD_LAYOUT_RU:	
            ui.bnMinus->setProperty(keyCodeProperty,   shiftPressed ?  95 :  45); //_-
            ui.bnEqual->setProperty(keyCodeProperty,   shiftPressed ?  43 :  61); //+=
            ui.bnTilde->setProperty(keyCodeProperty,   shiftPressed ? 126 :  96); //~`
            ui.bn0->setProperty(keyCodeProperty,       shiftPressed ?  41 :  48); //)0
            ui.bn1->setProperty(keyCodeProperty,       shiftPressed ?  33 :  49); //!1
            ui.bn2->setProperty(keyCodeProperty,       shiftPressed ?  64 :  50); //@2
            ui.bn3->setProperty(keyCodeProperty,       shiftPressed ?  35 :  51); //#3
            ui.bn4->setProperty(keyCodeProperty,       shiftPressed ?  36 :  52); //$4
            ui.bn5->setProperty(keyCodeProperty,       shiftPressed ?  37 :  53); //%5
            ui.bn6->setProperty(keyCodeProperty,       shiftPressed ?  94 :  54); //^6
            ui.bn7->setProperty(keyCodeProperty,       shiftPressed ?  38 :  55); //&7
            ui.bn8->setProperty(keyCodeProperty,       shiftPressed ?  42 :  56); //*8
            ui.bn9->setProperty(keyCodeProperty,       shiftPressed ?  40 :  57); //(9

            ui.bnQ->setProperty(keyCodeProperty,       shiftPressed ? 201 : 233); //Йй
            ui.bnW->setProperty(keyCodeProperty,       shiftPressed ? 214 : 246); //Цц
            ui.bnE->setProperty(keyCodeProperty,       shiftPressed ? 211 : 243); //Уу
            ui.bnR->setProperty(keyCodeProperty,       shiftPressed ? 202 : 234); //Кк
            ui.bnT->setProperty(keyCodeProperty,       shiftPressed ? 197 : 229); //Ее
            ui.bnY->setProperty(keyCodeProperty,       shiftPressed ? 205 : 237); //Нн
            ui.bnU->setProperty(keyCodeProperty,       shiftPressed ? 195 : 227); //Гг
            ui.bnI->setProperty(keyCodeProperty,       shiftPressed ? 216 : 248); //Шш
            ui.bnO->setProperty(keyCodeProperty,       shiftPressed ? 217 : 249); //Щщ
            ui.bnP->setProperty(keyCodeProperty,       shiftPressed ? 199 : 231); //Зз
            ui.bnLSB->setProperty(keyCodeProperty,     shiftPressed ? 213 : 245); //Хх
            ui.bnRSB->setProperty(keyCodeProperty,     shiftPressed ? 218 : 250); //Ъъ

            ui.bnBackslash->setProperty(keyCodeProperty,  shiftPressed ? 124 :  92); //|"\"
            ui.bnA->setProperty(keyCodeProperty,          shiftPressed ? 212 : 244); //Фф
            ui.bnS->setProperty(keyCodeProperty,          shiftPressed ? 219 : 251); //Ыы
            ui.bnD->setProperty(keyCodeProperty,          shiftPressed ? 194 : 226); //Вв
            ui.bnF->setProperty(keyCodeProperty,          shiftPressed ? 192 : 224); //Аа
            ui.bnG->setProperty(keyCodeProperty,          shiftPressed ? 207 : 239); //Пп
            ui.bnH->setProperty(keyCodeProperty,          shiftPressed ? 208 : 240); //рр
            ui.bnJ->setProperty(keyCodeProperty,          shiftPressed ? 206 : 238); //Оо
            ui.bnK->setProperty(keyCodeProperty,          shiftPressed ? 203 : 235); //Лл
            ui.bnL->setProperty(keyCodeProperty,          shiftPressed ? 196 : 228); //Дд
            ui.bnSemicolon->setProperty(keyCodeProperty,  shiftPressed ? 198 : 230); //Жж
            ui.bnApostrophe->setProperty(keyCodeProperty, shiftPressed ? 221 : 253); //Ээ
            ui.bnSlash->setProperty(keyCodeProperty,      shiftPressed ?  63 :  47); //?/

            //Shift key does not need keyCodeProperty
            ui.bnSpecialLayout->setProperty(keyCodeProperty, Qt::Key_Mode_switch);
            ui.bnZ->setProperty(keyCodeProperty,          shiftPressed ? 223 : 255); //Яя
            ui.bnX->setProperty(keyCodeProperty,          shiftPressed ? 215 : 247); //Чч
            ui.bnC->setProperty(keyCodeProperty,          shiftPressed ? 209 : 241); //Сс
            ui.bnV->setProperty(keyCodeProperty,          shiftPressed ? 204 : 236); //Мм
            ui.bnB->setProperty(keyCodeProperty,          shiftPressed ? 200 : 232); //Ии
            ui.bnN->setProperty(keyCodeProperty,          shiftPressed ? 210 : 242); //Тт
            ui.bnM->setProperty(keyCodeProperty,          shiftPressed ? 220 : 252); //Ьь
            ui.bnComma->setProperty(keyCodeProperty,      shiftPressed ? 193 : 225); //Бб
            ui.bnDot->setProperty(keyCodeProperty,        shiftPressed ? 222 : 254); //Юю
            ui.bnSpecialShift->setProperty(keyCodeProperty, Qt::Key_Shift);
            
            ui.bnSpecialHide->setProperty(keyCodeProperty, Qt::Key_Escape);
            ui.bnSpace->setProperty(keyCodeProperty, Qt::Key_Space);
            ui.bnLeft->setProperty(keyCodeProperty, Qt::Key_Left);
            ui.bnRight->setProperty(keyCodeProperty, Qt::Key_Right);
            ui.bnBackspace->setProperty(keyCodeProperty, Qt::Key_Backspace);	

            break;
        }

}

void QVirtualKeyboard::setAllButtonLabels(QObject* obj) {
    QObjectList ql = obj->children();
    for (int i = 0; i < ql.count(); i++) {		
        if (ql[i]->isWidgetType()) {
            setAllButtonLabels(ql[i]);
            QWidget * w = dynamic_cast<QWidget*>(ql[i]);
            if (w) {
                const QString wName = w->metaObject()->className();
                //qDebug( w->metaObject()->className() );
                if (wName == "QPushButton") {
                    QPushButton* bn = (QPushButton*)w;
                    int propVal = bn->property(keyCodeProperty).toInt();
                    if (propVal == Qt::Key_Escape && bn->objectName() == "bnSpecialHide") {
                        bn->setText(trUtf8("Скрыть"));
                    } else if (propVal == Qt::Key_Escape && bn->objectName() == "bnSpecialRestore") {
                        bn->setText(trUtf8("Клавиатура"));
                    } else if (propVal == Qt::Key_Shift) {
                        bn->setText("Shift");
                    } else if (propVal == Qt::Key_Mode_switch) {
                        bn->setText("En / Ru");
                    } else if (propVal == Qt::Key_Space) {
                        bn->setText("Space");
                    } else if (propVal == Qt::Key_Left) {
                        bn->setText("<-");
                    } else if (propVal == Qt::Key_Right) {
                        bn->setText("->");
                    } else if (propVal == Qt::Key_Backspace) {
                        bn->setText("Backspace");
                    } else { //Any other key
                        std::string stdStr;
                        char charValue = w->property(keyCodeProperty).toUInt();
                        if (charValue == 38) { //& symbol
                            bn->setText("&&");
                        } else {
                            stdStr.push_back(w->property(keyCodeProperty).toUInt());
                            bn->setText(QString(QTextCodec::codecForName("CP1251")->toUnicode(stdStr.data())));
                        }
                        //bn->setText(QString("%1").arg(w->property(keyCodeProperty).toString()));
                    }
                    
                }
            }
        }
    }
}

void QVirtualKeyboard::collapse() {
    //setMinimumSize(1,1);
    QRect bnHideRect = ui.bnSpecialShift->frameGeometry();
    int posX = this->frameGeometry().left();
    int posY = this->frameGeometry().bottom();
    this->resize(bnHideRect.width(), bnHideRect.height());
    bnSpecialRestore->resize(bnHideRect.width(), bnHideRect.height());
    //bnSpecialRestore->move(0, 0);
    bnSpecialRestore->show();
    this->move(posX, posY - this->frameGeometry().height() + 1);
    //setFixedSize(size());

    //Parent restore
    
    QMainWindow* p = (QMainWindow*)parent();
    p->setMaximumSize(0x000FFFF,0x0000FFFF);
    qDebug() << this->frameGeometry().bottom() << " - " << p->geometry().top();
    p->resize(p->width(), this->frameGeometry().bottom() - p->geometry().top());
    p->setFixedSize(p->size());
    
}

void QVirtualKeyboard::restore() {
    //setMaximumSize(2048,2048);

    bnSpecialRestore->hide();
    int posX = this->frameGeometry().left();
    int posY = this->frameGeometry().bottom();
    this->resize(((QWidget*)this->parent())->frameGeometry().width(), 230);
    
    this->move(posX, posY - this->frameGeometry().height() + 1);
    //setFixedSize(size());

    //Parent shrink
    QMainWindow* p = (QMainWindow*)parent();
    p->setMinimumSize(1,1);
    p->resize(p->width(), p->height() - this->frameGeometry().height() + 1);
    p->setFixedSize(p->size());
    
}

void QVirtualKeyboard::onBnClicked() {
    int bnCode = this->focusWidget()->property(keyCodeProperty).toInt();
    if (bnCode == Qt::Key_Escape) {
        if (bnSpecialRestore->isVisible()) {
            restore();
        } else {
            collapse();
        }
    } else if (bnCode == Qt::Key_Mode_switch) {
        if (keyLayout == VIRTUALKEYBOARD_LAYOUT_EN)
            keyLayout = VIRTUALKEYBOARD_LAYOUT_RU;
        else if (keyLayout == VIRTUALKEYBOARD_LAYOUT_RU)
            keyLayout = VIRTUALKEYBOARD_LAYOUT_EN;

        setAllButtonCodes();
        setAllButtonLabels(this);

    } else if (bnCode == Qt::Key_Shift) {
        shiftPressed = !shiftPressed;
        setAllButtonCodes();
        setAllButtonLabels(this);

        //Highlight shift button
        //disabled - нужно разобраться со свойствами - теряет форму
        /*
        if (shiftPressed) {
            ui.bnSpecialShift->setStyleSheet(QString("font: bold;"));
        } else {
            ui.bnSpecialShift->setStyleSheet(QString("font: regular;"));
        }
        */
    } else if (bnCode == Qt::Key_Space) {
        int key = Qt::Key_Space;
        emit onKeyPressed(key);
    } else if (bnCode == Qt::Key_Backspace) {
        int key = Qt::Key_Backspace;
        emit onKeyPressed(key);
    } else {
        // Get pressed key code
        int key = this->focusWidget()->property(keyCodeProperty).toInt();

        /*
        QString str = this->focusWidget()->property(keyCodeProperty).toString();
        QByteArray ba = str.toLocal8Bit();
        int key = ba[0];
        */
        emit onKeyPressed(key);
        qApp->setActiveWindow((QWidget*)this->parent());
    }
}
