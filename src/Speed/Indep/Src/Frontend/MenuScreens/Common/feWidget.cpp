#include "feWidget.hpp"

void FEWidget::Act(const char* parent_pkg, unsigned int data) {}
void FEWidget::CheckMouse(const char* parent_pkg, const float mouse_x, const float mouse_y) {}
void FEWidget::Draw() {}
void FEWidget::Position() {}
void FEWidget::Show() {}
void FEWidget::Hide() {}
void FEWidget::SetFocus(const char* parent_pkg) {}
void FEWidget::UnsetFocus() {}

void FEStatWidget::Act(const char* parent_pkg, unsigned int data) {}
void FEStatWidget::CheckMouse(const char* parent_pkg, const float mouse_x, const float mouse_y) {}
void FEStatWidget::SetFocus(const char* parent_pkg) {}
void FEStatWidget::UnsetFocus() {}

void FEToggleWidget::CheckMouse(const char* parent_pkg, const float mouse_x, const float mouse_y) {}
void FEToggleWidget::BlinkArrows(unsigned int data) {}
