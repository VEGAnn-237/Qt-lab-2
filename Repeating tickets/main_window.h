#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMainWindow>
#include <QProgressBar>
#include <QPushButton>
#include <QSpinBox>
#include <QTableWidget>
#include <QVBoxLayout>

#include <chrono>
#include <random>
#include <stack>
#include <string>
#include <vector>

class MainWindow : public QMainWindow {
  Q_OBJECT
 public:
  MainWindow();
  static int GetWidgetItemIndex(QListWidgetItem* item);

 private:
  void ManageLayout();
  void RebuildView();
  void ManageQuestionViewLayout();
  void ManageProgressBars();

  void NewItemSelected(QListWidgetItem*);
  void AddTicket(QListWidgetItem*);
  void EraseTicket(QListWidgetItem*);
  QListWidgetItem* GetNewItem();

 private:
  static int kMaxSpinboxValue;
  static int kWindowMinimumHeight;
  static int kWindowMinimumWidth;

  QGridLayout* layout_;

  QWidget* widget_;
  QSpinBox* count_;
  QListWidget* view_;

  QGroupBox* question_view_;
  QVBoxLayout* question_view_layout_;
  QLabel* number_;
  QLabel* name_;
  QLineEdit* name_edit_;
  QComboBox* status_;
  QPushButton* next_question_;
  QPushButton* previous_question_;
  QProgressBar* total_progress_;
  QProgressBar* green_progress_;

  std::vector<QListWidgetItem*> gray_yellow_tickets_;
  int gray_tickets_;
  std::stack<QListWidgetItem*> previous_tickets_;
};

#endif // MAIN_WINDOW_H