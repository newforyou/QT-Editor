#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMenuBar>
#include <QToolBar>
#include <QIcon>
#include <QLabel>
#include <QFontComboBox>
#include <QComboBox>
#include <QToolButton>
#include <QStatusBar>
#include <QDockWidget>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QDialog>
#include <QFileDialog>
#include <QFile>
#include <QDebug>
#include <QTextStream>
#include <QMessageBox>
#include <QPushButton>
#include <QTextCursor>
#include <QClipboard>
#include <QLineEdit>
#include <QDialog>
#include <QVBoxLayout>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QFontDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 初始化文件为未保存状态
    isUnititled = true;
    // 初始化文件名为"未命名.txt"
    curFile = tr("未命名.txt");
    // 初始化窗口标题为文件名
    setWindowTitle(curFile);

    //设置窗口图标
    this->setWindowIcon(QIcon(":/icon/icon/editor.png"));
    //设置窗口大小
    this->resize(900,600);

    tcf = new QTextCharFormat;

    creatMenu();
    creatTool();
    connectImpl();


    //设置状态栏
    QStatusBar* statusBar = this->statusBar();

    QLabel* authorLabel = new QLabel("作者：@newforyyy");
    authorLabel->setAlignment(Qt::AlignLeft);
    statusBar->addPermanentWidget(authorLabel);

    //点击查找后显示的窗口
    findDlg = new QDialog(this);
    findDlg->setWindowTitle(tr("查找"));
    findLineEdit = new QLineEdit(findDlg);
    QPushButton *btn= new QPushButton(tr("查找下一个"), findDlg);
    //垂直布局
    QVBoxLayout *layout= new QVBoxLayout(findDlg);
    layout->addWidget(findLineEdit);
    layout->addWidget(btn);
    //将“查找下一个”按钮与自定义showFindText槽函数连接
    connect(btn, SIGNAL(clicked()), this, SLOT(showFindText()));

}

void MainWindow::creatMenu()
{
    //基本控件设置
    //获取菜单栏
    QMenuBar *menuBar = this->menuBar();

    //定义文件菜单和编辑菜单
    QMenu* fileMenu = menuBar->addMenu("文件");
    QMenu* EditMenu =menuBar->addMenu("编辑");


    //向文件菜单中添加行为
    newfile = fileMenu->addAction(QIcon(":/icon/icon/newfile.png"),"新建");
    openfile = fileMenu->addAction(QIcon(":/icon/icon/openfile.png"),"打开");
    savefile = fileMenu->addAction(QIcon(":/icon/icon/savefile.png"),"保存");
    saveasfile = fileMenu->addAction(QIcon(":/icon/icon/saveas.png"),"另存为");

    //向编辑菜单中添加行为
    copyText = EditMenu->addAction(QIcon(":/icon/icon/copy.png"),"复制");
    cutText = EditMenu->addAction(QIcon(":/icon/icon/cut.png"),"剪切");
    pasteText = EditMenu->addAction(QIcon(":/icon/icon/paste.png"),"粘贴");
    EditMenu->addSeparator();
    seekText = EditMenu->addAction(QIcon(":/icon/icon/seek.png"),"查找文本");
    EditMenu->addSeparator();
    fontSet = EditMenu->addAction(QIcon(":/icon/icon/fontSet.png"),"字体设置");



}
void MainWindow::creatTool()
{
    //添加工具栏
    QToolBar* toolBar = this->addToolBar("tool");

    //工具栏添加新建、打开、保存、另存为
    toolBar->addAction(newfile);
    toolBar->addSeparator();
    toolBar->addAction(openfile);
    toolBar->addSeparator();
    toolBar->addAction(savefile);
    toolBar->addSeparator();
    toolBar->addAction(saveasfile);
    toolBar->addSeparator();

    //工具栏添加复制、剪切、粘贴、查找、字体设置
    toolBar->addAction(copyText);
    toolBar->addSeparator();
    toolBar->addAction(cutText);
    toolBar->addSeparator();
    toolBar->addAction(pasteText);
    toolBar->addSeparator();
    toolBar->addAction(seekText);
    toolBar->addSeparator();

/* 用QToolButton的实现
    //设置文本剪切
    QToolButton* cutBtn = new QToolButton();
    cutBtn->setToolTip("剪切");
    cutBtn->setIcon(QIcon(":/icon/icon/cut.png"));
    toolBar->addWidget(cutBtn);

    toolBar->addSeparator();

    //设置文本复制
    QToolButton* copyBtn = new QToolButton();
    copyBtn->setToolTip("复制");
    copyBtn->setIcon(QIcon(":/icon/icon/copy.png"));
    toolBar->addWidget(copyBtn);

    toolBar->addSeparator();

    //设置文本粘贴
    QToolButton* pasteBtn = new QToolButton();
    pasteBtn->setToolTip("粘贴");
    pasteBtn->setIcon(QIcon(":/icon/icon/paste.png"));
    toolBar->addWidget(pasteBtn);

    toolBar->addSeparator();

    //设置文本查找
    QToolButton* seekBtn = new QToolButton();
    seekBtn->setToolTip("搜索");
    seekBtn->setIcon(QIcon(":/icon/icon/seek.png"));
    toolBar->addWidget(seekBtn);

    toolBar->addSeparator();
*/

    /************ 工具栏的字体设置 ************/

    //设置“字体”标签
    fontTypeLabel = new QLabel("字体：");
    toolBar->addWidget(fontTypeLabel);
    //添加字体框
    fontTypeCmb = new QFontComboBox;
    toolBar->addWidget(fontTypeCmb);

    toolBar->addSeparator();

    //设置“字号”标签
    fontSizeLabel = new QLabel("字号：");
    toolBar->addWidget(fontSizeLabel);
    //添加字号框
    fontSizeCmb = new QComboBox;
    toolBar->addWidget(fontSizeCmb);
    //字号框中添加字号选项
    for(int i=1;i<=72;i++){
        fontSizeCmb->addItem(QString::number(i));
    }

    toolBar->addSeparator();

    //添加加粗按钮
    boldBtn = new QToolButton();
    boldBtn->setToolTip("加粗");
    boldBtn->setIcon(QIcon(":/icon/icon/bold.png"));
    toolBar->addWidget(boldBtn);

    toolBar->addSeparator();

    //添加下划线按钮
    underlineBtn = new QToolButton();
    underlineBtn->setToolTip("下划线");
    underlineBtn->setIcon(QIcon(":/icon/icon/underline.png"));
    toolBar->addWidget(underlineBtn);

    /************ 主体：文本编辑框 ************/

    textEdit = new QTextEdit;
    this->setCentralWidget(textEdit);

}

/************* 信号与槽的连接 *************/

void MainWindow::connectImpl()
{

    //信号与槽-新建文件
    connect(newfile,QAction::triggered,this,MainWindow::newFile);

    //信号与槽-打开文件 //C++11特性：lambda表达式 匿名函数
    connect(openfile,QAction::triggered,[=]{
        if(maybeSave()){
            QString fileName = QFileDialog::getOpenFileName(this);

            if(!fileName.isEmpty()){
                openFile(fileName);
            }
        }
    });

    //信号与槽-保存文件
    connect(savefile,QAction::triggered,this,MainWindow::save);

    //信号与槽-文件另存为
    connect(saveasfile,QAction::triggered,this,MainWindow::saveasFile);

    //信号与槽-复制 QTextEdit自带的copy()槽函数
    connect(copyText,QAction::triggered,textEdit,QTextEdit::copy);

    //信号与槽-剪切 QTextEdit自带的cut()槽函数
    connect(cutText,QAction::triggered,textEdit,QTextEdit::cut);

    //信号与槽-粘贴 QTextEdit自带的paste()槽函数
    connect(pasteText,QAction::triggered,textEdit,QTextEdit::paste);

    //信号与槽-搜索
    connect(seekText,QAction::triggered,[=]{
        findDlg->show();
    });

    //信号与槽-字体设置
    connect(fontSet,QAction::triggered,[=]{
      bool fontSelected;
      QFont font = QFontDialog::getFont(&fontSelected,this);
      if(fontSelected){
          textEdit->setFont(font);
      }
    });

    /************ 工具栏的字体设置 ************/

    //信号与槽-字体改变
    connect(fontTypeCmb,QFontComboBox::currentFontChanged,this,MainWindow::setFont);

    //信号与槽-字号改变
    void (QComboBox::*p)(int)=QComboBox::currentIndexChanged;
    connect(fontSizeCmb,p,this,MainWindow::setFontSize);

    //信号与槽-字体加粗
    connect(boldBtn,QToolButton::clicked,this,MainWindow::setBold);

    //信号与槽-字体下划线
    connect(underlineBtn,QToolButton::clicked,this,MainWindow::setUnderline);
}

    /*************  自定义槽函数的实现 *************/

//槽函数实现-新建文件
void MainWindow::newFile(){
  if(maybeSave()){
      isUnititled = true;
      curFile = tr("未命名.txt");
      setWindowTitle(curFile);
      textEdit->clear();
  }
}

//判断是否保存过
bool MainWindow::maybeSave(){
    //如果文档被更改了
    if(textEdit->document()->isModified()){
        //自定义一个警告对话框
        QMessageBox box;
        box.setWindowTitle(tr("警告"));
        box.setIcon(QMessageBox::Warning);
        box.setText(curFile+tr(" 尚未保存，是否保存？"));
        QPushButton *yesBtn = box.addButton(tr("是(&Y)"),QMessageBox::YesRole);
        box.addButton(tr("否(&N)"),QMessageBox::NoRole);
        QPushButton *cancelBtn = box.addButton(tr("取消"),QMessageBox::RejectRole);
        box.exec();
        if(box.clickedButton()==yesBtn)
            return save();
        else if(box.clickedButton()==cancelBtn)
            return false;
    }
    //如果文档没被修改，则返回true
    return true;
}

//Todo: bug写入数据保存文件之后再点击保存按钮会提示未保存！！！！！！
//如果文档以前没有保存过，那么执行另存为操作saveasFile()
//如果已经保存过，那么调用saveFile()执行文件保存操作。
bool MainWindow::save(){
    if(isUnititled){
        return saveasFile();
    }else {
        return saveFile(curFile);
    }
}

//槽函数实现-另存为操作，传入文件名调用真正的保存文件操作
bool MainWindow::saveasFile(){
    QString fileName = QFileDialog::getSaveFileName(this,tr("另存为"),curFile);
    if(fileName.isEmpty())
        return false;
    return saveFile(fileName);
}

//槽函数实现-真正的保存文件操作
bool MainWindow::saveFile(const QString &fileName){
    QFile f(fileName);
    if(f.open(QIODevice::ReadWrite)){
           QTextStream fin(&f);
           fin<<textEdit->toPlainText();
     }else{
           qDebug()<<"save file failed!";
           return false;
    }
    isUnititled = false;
    //获得文件的标准路径
    curFile = QFileInfo(fileName).canonicalFilePath();
    setWindowTitle(curFile);
    return true;
}

//槽函数实现-打开文件
bool MainWindow::openFile(const QString &fileName)
{
    QFile f(fileName);
    if(f.open(QIODevice::ReadWrite)){
           QTextStream fout(&f);
           textEdit->setText(fout.readAll());
     }else{
           qDebug()<<"open file failed!";
           return false;
    }
    curFile = QFileInfo(fileName).canonicalFilePath();
    setWindowTitle(curFile);
    return true;

}

//槽函数实现-显示查找到的文本
void MainWindow::showFindText()
{
    //获得对话框的内容
    QString findtext = findLineEdit->text();
    //查找下一个
    if(textEdit->find(findtext,QTextDocument::FindBackward))
    {
        //找到后高亮显示
        QPalette palette = textEdit->palette();
        palette.setColor(QPalette::Highlight,palette.color(QPalette::Active,QPalette::Highlight));
        textEdit->setPalette(palette);
    }else {
        QMessageBox::information(this,tr("注意"),tr("没有找到内容"),QMessageBox::Ok);
    }
}

//槽函数实现-字体改变
void MainWindow::setFont(const QFont &font)
{
    //该段代码没有存储字号属性，所以有bug：先改变字号再改变字体会使得字号重置。
//    tcf->setFont(font);
//    textEdit->mergeCurrentCharFormat(*tcf);

   QTextCursor cursor = textEdit->textCursor();

   // 获取当前选中文字的字体和字号
   QTextCharFormat currentFormat = cursor.charFormat();
   QFont currentFont = currentFormat.font();
   qreal currentFontSize = currentFont.pointSizeF();

   // 设置新的字体
   QFont newFont = font;
   newFont.setPointSizeF(currentFontSize);
   tcf->setFont(newFont);
   textEdit->mergeCurrentCharFormat(*tcf);
}

//槽函数实现-字号改变
void MainWindow::setFontSize(int index)
{
    tcf->setFontPointSize(double(index));
    textEdit->mergeCurrentCharFormat(*tcf);
}

//槽函数实现-字体加粗
void MainWindow::setBold()
{
    if(textEdit->fontWeight()>QFont::Normal){
        tcf->setFontWeight(QFont::Normal);
    }else{
        tcf->setFontWeight(QFont::Bold);
    }
    textEdit->mergeCurrentCharFormat(*tcf);
}

//槽函数实现-字体下划线
void MainWindow::setUnderline()
{
    if(textEdit->fontUnderline()){
         tcf->setFontUnderline(false);
    }else {
        tcf->setFontUnderline(true);
    }
    textEdit->mergeCurrentCharFormat(*tcf);
}

MainWindow::~MainWindow()
{
    delete ui;
}

