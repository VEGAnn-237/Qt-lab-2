#include "main_window.h"

int MainWindow::kMaxSpinboxValue = 100;
int MainWindow::kWindowMinimumHeight = 500;
int MainWindow::kWindowMinimumWidth = 500;
bool MainWindow::kNeedReset = true;

MainWindow::MainWindow() : QMainWindow(nullptr),
                           layout_(new QGridLayout()),
                           widget_(new QWidget(this)),
                           date_view_(new QGroupBox("Цели на день", widget_)),
                           date_view_layout_(new QVBoxLayout()),
                           goal_view_(new QGroupBox("Описание цели", date_view_)),
                           goal_view_layout_(new QVBoxLayout()),
                           count_(new QSpinBox(date_view_)),
                           view_(new QListWidget(date_view_)),
                           name_(new QLabel("Цель №1", goal_view_)),
                           name_edit_(new QLineEdit("Цель №1", goal_view_)),
                           status_(new QComboBox(goal_view_)),
                           total_progress_(new QProgressBar(date_view_)),
                           calendar_(new QCalendarWidget(widget_)) {

  resize(kWindowMinimumWidth, kWindowMinimumHeight);
  setMinimumSize(kWindowMinimumWidth, kWindowMinimumHeight);
  setWindowTitle("Calendar reminder");

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

MainWindow::GoalsOnDate::GoalsOnDate() : number_of_goals_(3) {
  ResetGoalNames();
}

MainWindow::GoalsOnDate::GoalsOnDate(int number_of_goals) : number_of_goals_(number_of_goals_) {
  ResetGoalNames();
}

void MainWindow::GoalsOnDate::ResetGoalNames() {
  done_count_ = 0;
  goal_names_.resize(number_of_goals_);
  done_.resize(number_of_goals_);
  for (int i = 0; i < number_of_goals_; i++) {
    goal_names_[i] = "Цель №" + std::to_string(i + 1);
    done_[i] = false;
  }
}

void MainWindow::ManageLayout() {
  layout_->addWidget(calendar_, 0, 0);
  layout_->addWidget(date_view_, 1, 0);
  layout_->addWidget(goal_view_, 2, 0);

  count_->setMinimum(1);
  count_->setMaximum(kMaxSpinboxValue);
  count_->setPrefix("Всего целей на день: ");

  ManageDateViewLayout();
  ManageGoalViewLayout();
  NewDateSelected(calendar_->selectedDate());
  total_progress_->setFormat("0 выполненных целей");

  connect(calendar_, &QCalendarWidget::clicked, this, &MainWindow::NewDateSelected);

  connect(count_, &QSpinBox::valueChanged, view_, [&](int new_value) {
    if (kNeedReset) {
      GoalsOnDate& current_goals = dates_events[calendar_->selectedDate()];
      current_goals.number_of_goals_ = count_->value();
      current_goals.ResetGoalNames();
      UpdateDateView(current_goals);
    }
  });

  connect(view_, &QListWidget::itemClicked, goal_view_, [&](QListWidgetItem* item) {
    NewItemSelected(item);
  });

  connect(status_, &QComboBox::activated, status_, [&](int index) {
    QListWidgetItem* selected_item(*view_->selectedItems().begin());
    int item_id = GetWidgetItemIndex(selected_item);
    GoalsOnDate& current_date = dates_events[calendar_->selectedDate()];

    if (index == 0) {
      selected_item->setBackground(Qt::gray);
      current_date.done_[item_id] = false;
      current_date.done_count_--;
    } else {
      selected_item->setBackground(Qt::green);
      current_date.done_[item_id] = true;
      current_date.done_count_++;
    }

    total_progress_->setValue(current_date.done_count_);
  });

  connect(total_progress_, &QProgressBar::valueChanged, total_progress_, [&] (int value) {
    int remainder = value % 100;
    if (remainder >= 10 && remainder <= 20) {
      total_progress_->setFormat("%v выполненных целей");
    } else {
      remainder %= 10;
      if (remainder == 1) {
        total_progress_->setFormat("%v выполненная цель");
      } else if (remainder >= 2 && remainder <= 4) {
        total_progress_->setFormat("%v выполненные цели");
      } else {
        total_progress_->setFormat("%v выполненных целей");
      }
    }
  });

  connect(name_edit_, &QLineEdit::returnPressed, name_, [&] {
    if (!name_edit_->text().isEmpty()) {
      name_->setText(QString("Цель: ") + name_edit_->text());

      QListWidgetItem* selected_item(*view_->selectedItems().begin());
      selected_item->setText(name_edit_->text());

      int item_id = GetWidgetItemIndex(selected_item);
      dates_events[calendar_->selectedDate()].goal_names_[item_id] = name_edit_->text().toStdString();
    }
  });

  layout_->setColumnStretch(0, 0);

  layout_->setRowStretch(0, 0);
  layout_->setRowStretch(1, 0);
  layout_->setRowStretch(2, 0);

  widget_->setLayout(layout_);
  setCentralWidget(widget_);
}

void MainWindow::ManageDateViewLayout() {
  date_view_layout_->addWidget(count_);
  date_view_layout_->addWidget(total_progress_);
  date_view_layout_->addWidget(view_);

  date_view_->setLayout(date_view_layout_);
}

void MainWindow::ManageGoalViewLayout() {
  goal_view_layout_->addWidget(name_);
  goal_view_layout_->addWidget(name_edit_);

  status_->addItem("Цель предстоит выполнить");
  status_->addItem("Цель выполнена");
  goal_view_layout_->addWidget(status_);

  goal_view_->setLayout(goal_view_layout_);
}

void MainWindow::NewDateSelected(const QDate& date) {
  goal_view_->setVisible(false);

  GoalsOnDate& current_goals = dates_events[date];

  UpdateDateView(current_goals);
}

void MainWindow::UpdateDateView(const GoalsOnDate& current_goals) {
  kNeedReset = false;
  count_->setValue(current_goals.number_of_goals_);
  kNeedReset = true;

  view_->clear();

  for (int i = 0; i < count_->value(); i++) {
    view_->addItem(current_goals.goal_names_[i].c_str());
    if (current_goals.done_[i]) {
      view_->item(i)->setBackground(Qt::green);
    } else {
      view_->item(i)->setBackground(Qt::gray);
    }
  }

  total_progress_->setMinimum(0);
  total_progress_->setMaximum(count_->value());
  total_progress_->setValue(current_goals.done_count_);
}

void MainWindow::NewItemSelected(QListWidgetItem* item) {
  goal_view_->setVisible(true);

  name_->setText(QString("Цель: ") + item->text());
  name_edit_->setText(item->text());

  if (item->background() == Qt::gray) {
    status_->setCurrentIndex(0);
  } else {
    status_->setCurrentIndex(1);
  }
}