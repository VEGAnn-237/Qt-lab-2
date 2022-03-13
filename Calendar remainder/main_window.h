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
#include <QCalendarWidget>
#include <QDate>

#include <map>
#include <string>
#include <vector>

class MainWindow : public QMainWindow {
  Q_OBJECT
 public:
  MainWindow();

  static int GetWidgetItemIndex(QListWidgetItem* item);

 private:
  struct GoalsOnDate {
    int number_of_goals_;
    std::vector<std::string> goal_names_;
    std::vector<bool> done_;
    int done_count_;

    GoalsOnDate();
    GoalsOnDate(int number_of_goals);
    void ResetGoalNames();
  };

  void ManageLayout();
  void ManageDateViewLayout();
  void ManageGoalViewLayout();
  void NewDateSelected(const QDate&);
  void RebuildView();
  void UpdateDateView(const GoalsOnDate&);
  void NewItemSelected(QListWidgetItem*);

 private:
  static int kMaxSpinboxValue;
  static int kWindowMinimumHeight;
  static int kWindowMinimumWidth;
  static bool kNeedReset;

  QGridLayout* layout_;

  QWidget* widget_;
  QGroupBox* date_view_;
  QVBoxLayout* date_view_layout_;
  QGroupBox* goal_view_;
  QVBoxLayout* goal_view_layout_;
  QSpinBox* count_;
  QListWidget* view_;
  QLabel* name_;
  QLineEdit* name_edit_;
  QComboBox* status_;
  QProgressBar* total_progress_;
  QCalendarWidget* calendar_;

  std::map<QDate, GoalsOnDate> dates_events;
};

#endif // MAIN_WINDOW_H