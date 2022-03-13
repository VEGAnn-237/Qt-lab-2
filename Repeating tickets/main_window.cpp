#include "main_window.h"

int MainWindow::kMaxSpinboxValue = 1000;
int MainWindow::kWindowMinimumHeight = 500;
int MainWindow::kWindowMinimumWidth = 700;

MainWindow::MainWindow() : QMainWindow(nullptr),
                           layout_(new QGridLayout()),
                           widget_(new QWidget(this)),
                           count_(new QSpinBox(widget_)),
                           view_(new QListWidget(widget_)),
                           question_view_(new QGroupBox("Описание билета", widget_)),
                           question_view_layout_(new QVBoxLayout()),
                           number_(new QLabel("Номер билета: 1", question_view_)),
                           name_(new QLabel("Назавние билета: Билет №1", question_view_)),
                           name_edit_(new QLineEdit("Билет №1", question_view_)),
                           status_(new QComboBox(question_view_)),
                           next_question_(new QPushButton("Следующий билет", widget_)),
                           previous_question_(new QPushButton("Предыдущий билет", widget_)),
                           total_progress_(new QProgressBar(widget_)),
                           green_progress_(new QProgressBar(widget_)) {

  resize(kWindowMinimumWidth, kWindowMinimumHeight);
  setMinimumSize(kWindowMinimumWidth, kWindowMinimumHeight);
  setWindowTitle("Repeating tickets");

  ManageLayout();
}

int MainWindow::GetWidgetItemIndex(QListWidgetItem* item) {
  // 0-indexed
  QListWidget* list_widget = item->listWidget();

  for (int i = 0; i < list_widget->count(); i++) {
    if (list_widget->item(i) == item) {
      return i;
    }
  }
  return -1;
}

void MainWindow::ManageLayout() {
  layout_->addWidget(count_, 0, 0);
  layout_->addWidget(view_, 0, 1);
  layout_->addWidget(next_question_, 0, 2);
  layout_->addWidget(question_view_, 1, 1);
  layout_->addWidget(previous_question_, 1, 2);
  layout_->addWidget(green_progress_, 2, 0);
  layout_->addWidget(total_progress_, 2, 2);

  ManageProgressBars();
  ManageQuestionViewLayout();

  // quantity of tickets changed
  connect(count_, &QSpinBox::valueChanged, view_, [&] {
    RebuildView();
  });
  // double click on item from view_
  connect(view_, &QListWidget::itemDoubleClicked, view_, [&](QListWidgetItem* item) {
    if (item->background() == Qt::green) {
      AddTicket(item);
      item->setBackground(Qt::yellow);
      status_->setCurrentIndex(1);
    } else {
      if (item->background() == Qt::gray) {
        gray_tickets_--;
      }
      EraseTicket(item);
      item->setBackground(Qt::green);
      status_->setCurrentIndex(2);
    }
    green_progress_->setValue(count_->value() - gray_yellow_tickets_.size());
    total_progress_->setValue(count_->value() - gray_tickets_);
  });
  // click on item from view_
  connect(view_, &QListWidget::itemClicked, question_view_, [&](QListWidgetItem* item) {
    previous_tickets_.push(item);
    NewItemSelected(item);
  });
  // new item selected in status_
  connect(status_, &QComboBox::activated, status_, [&](int index) {
    QListWidgetItem* selected_item(*view_->selectedItems().begin());
    if (selected_item->background() == Qt::gray ||
        selected_item->background() == Qt::yellow) {
      if (selected_item->background() == Qt::gray) {
        gray_tickets_--;
      }
      EraseTicket(selected_item);
    }

    if (index == 0) {
      selected_item->setBackground(Qt::gray);
      gray_tickets_++;
      AddTicket(selected_item);
    } else if (index == 1) {
      selected_item->setBackground(Qt::yellow);
      AddTicket(selected_item);
    } else {
      selected_item->setBackground(Qt::green);
    }

    green_progress_->setValue(count_->value() - gray_yellow_tickets_.size());
    total_progress_->setValue(count_->value() - gray_tickets_);
  });
  // Enter pressed
  connect(name_edit_, &QLineEdit::returnPressed, name_, [&] {
    if (!name_edit_->text().isEmpty()) {
      name_->setText(QString("Название билета: ") + name_edit_->text());

      QListWidgetItem* selected_item(*view_->selectedItems().begin());
      selected_item->setText(name_edit_->text());
    }
  });
  // next question pressed
  connect(next_question_, &QPushButton::pressed, view_, [&] {
    if (gray_yellow_tickets_.size() > 0) {
      QListWidgetItem* new_item = GetNewItem();
      view_->setCurrentItem(new_item);
      previous_tickets_.push(new_item);
      NewItemSelected(new_item);
    }
  });
  // previous qusetion pressed
  connect(previous_question_, &QPushButton::pressed, view_, [&] {
    if (previous_tickets_.size() > 1) {
      previous_tickets_.pop();
      QListWidgetItem* new_item = previous_tickets_.top();
      view_->setCurrentItem(new_item);
      NewItemSelected(new_item);
    }
  });

  count_->setPrefix("Всего билетов: ");
  count_->setMaximum(kMaxSpinboxValue);
  count_->setMinimum(1);
  count_->setValue(10);

  layout_->setColumnStretch(0, 1);
  layout_->setColumnMinimumWidth(0, kWindowMinimumHeight/4);
  layout_->setColumnStretch(1, 2);
  layout_->setColumnMinimumWidth(1, kWindowMinimumHeight/2);
  layout_->setColumnStretch(2, 1);
  layout_->setColumnMinimumWidth(2, kWindowMinimumHeight/4);

  layout_->setRowStretch(0, 2);
  layout_->setRowMinimumHeight(0, kWindowMinimumWidth/2);
  layout_->setRowStretch(1, 1);
  layout_->setRowMinimumHeight(1, kWindowMinimumWidth/4);
  layout_->setRowStretch(2, 1);
  layout_->setRowMinimumHeight(2, kWindowMinimumWidth/4);

  widget_->setLayout(layout_);
  setCentralWidget(widget_);
}

void MainWindow::RebuildView() {
  question_view_->setVisible(false);
  view_->clear();
  gray_yellow_tickets_.clear();
  while (previous_tickets_.size() > 0) {
    previous_tickets_.pop();
  }
  green_progress_->setValue(0);
  green_progress_->setMaximum(count_->value());
  total_progress_->setValue(0);
  total_progress_->setMaximum(count_->value());
  gray_tickets_ = count_->value();

  for (int i = 0; i < count_->value(); i++) {
    std::string ticket_id = "Билет №" + std::to_string(i + 1);
    view_->addItem(ticket_id.c_str());

    view_->item(i)->setBackground(Qt::gray);
    gray_yellow_tickets_.push_back(view_->item(i));
  }
}

void MainWindow::ManageQuestionViewLayout() {
  question_view_layout_->addWidget(number_);
  question_view_layout_->addWidget(name_);
  question_view_layout_->addWidget(name_edit_);

  status_->addItem("Билет не повторялся");
  status_->addItem("Билет нужно повторить еще раз");
  status_->addItem("Билет не нужно повторять");
  question_view_layout_->addWidget(status_);

  question_view_->setLayout(question_view_layout_);
}

void MainWindow::ManageProgressBars() {
  green_progress_->setStyleSheet("QProgressBar::chunk {\n"
                                 "    background-color: #00ff00;\n"
                                 " }"
                                 "QProgressBar {"
                                 "    text-align: center;"
                                 "    border: 1px solid gray;"
                                 "}");
  green_progress_->setFormat("Хорошо повторенное");
  green_progress_->setMinimum(0);
  green_progress_->setVisible(true);
  green_progress_->setMinimumHeight(kWindowMinimumHeight / 12);
  green_progress_->setMaximumHeight(kWindowMinimumHeight / 12);

  total_progress_->setStyleSheet("QProgressBar::chunk {\n"
                                 "    background-color: #ffff00;\n"
                                 " }"
                                 "QProgressBar {"
                                 "    text-align: center;"
                                 "    border: 1px solid gray;"
                                 "}");
  total_progress_->setFormat("Повторенное");
  total_progress_->setMinimum(0);
  total_progress_->setVisible(true);
  total_progress_->setMinimumHeight(kWindowMinimumHeight / 12);
  total_progress_->setMaximumHeight(kWindowMinimumHeight / 12);
}

void MainWindow::NewItemSelected(QListWidgetItem* item) {
  question_view_->setVisible(true);

  std::string new_number = "Номер билета: " + std::to_string(GetWidgetItemIndex(item) + 1);
  number_->setText(new_number.c_str());

  name_->setText(QString("Название билета: ") + item->text());
  name_edit_->setText(item->text());

  if (item->background() == Qt::gray) {
    status_->setCurrentIndex(0);
  } else if (item->background() == Qt::yellow) {
    status_->setCurrentIndex(1);
  } else {
    status_->setCurrentIndex(2);
  }
}

void MainWindow::AddTicket(QListWidgetItem* item) {
  gray_yellow_tickets_.push_back(item);
}

void MainWindow::EraseTicket(QListWidgetItem* item) {
  for (auto& ticket : gray_yellow_tickets_) {
    if (ticket == item) {
      std::swap(ticket, gray_yellow_tickets_.back());
      gray_yellow_tickets_.pop_back();
      return;
    }
  }
}

QListWidgetItem* MainWindow::GetNewItem() {
  std::mt19937 random_generator(std::chrono::system_clock().now().time_since_epoch().count());
  int new_id = (random_generator() % gray_yellow_tickets_.size());

  if (gray_yellow_tickets_.size() > 1 && question_view_->isVisible()) {
    QListWidgetItem* selected_item(*view_->selectedItems().begin());
    while (gray_yellow_tickets_[new_id] == selected_item) {
      new_id = (random_generator() % gray_yellow_tickets_.size());
    }
  }
  return gray_yellow_tickets_[new_id];
}
