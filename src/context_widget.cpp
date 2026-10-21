#include "aiws/gui/context_widget.hpp"
#include <QHBoxLayout>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSlider>
#include <QString>
#include <QVBoxLayout>

namespace aiws::gui {

ContextWidget::ContextWidget(QWidget* parent) : QWidget(parent) {
    context_budget_slider_ = new QSlider(Qt::Horizontal, this);
    context_budget_slider_->setRange(50, 1000);
    context_budget_slider_->setSingleStep(50);

    budget_value_label_ = new QLabel(this);

    build_context_button_ = new QPushButton("Build Context", this);

    context_text_ = new QPlainTextEdit(this);
    context_text_->setReadOnly(true);

    auto* title_label = new QLabel("Context:", this);
    auto* budget_label = new QLabel("Token budget:", this);

    auto* budget_layout = new QHBoxLayout;
    budget_layout->addWidget(budget_label);
    budget_layout->addWidget(context_budget_slider_);
    budget_layout->addWidget(budget_value_label_);
    budget_layout->addWidget(build_context_button_);

    auto* main_layout = new QVBoxLayout(this);
    main_layout->addWidget(title_label);
    main_layout->addLayout(budget_layout);
    main_layout->addWidget(context_text_);

    connect(build_context_button_,
            &QPushButton::clicked,
            this,
            &ContextWidget::request_context);

    connect(context_budget_slider_,
            &QSlider::valueChanged,
            this,
            &ContextWidget::budget_changed);

    reset();
}

int ContextWidget::token_budget() const {
    return context_budget_slider_->value();
}

void ContextWidget::set_build_context_enabled(bool enabled) {
    build_context_button_->setEnabled(enabled);
}

void ContextWidget::set_context(const std::vector<ContextItem>& items) {
    QString text;

    for (const auto& item : items) {
        text += "Document: ";
        text += QString::fromStdString(item.document_id);
        text += "\nChunk: ";
        text += QString::number(
            static_cast<unsigned long long>(item.chunk_sequence));
        text += "\nTokens: ";
        text += QString::number(
            static_cast<unsigned long long>(item.token_count));
        text += "\nScore: ";
        text += QString::number(item.score);
        text += "\nTruncated: ";
        text += item.truncated ? "Yes" : "No";
        text += "\nText: ";
        text += QString::fromStdString(item.text);
        text += "\n\n";
    }

    context_text_->setPlainText(text);
}

void ContextWidget::clear_context() {
    context_text_->clear();
}

void ContextWidget::reset() {
    context_budget_slider_->setValue(300);
    budget_changed(300);
    clear_context();
    set_build_context_enabled(false);
}

void ContextWidget::request_context() {
    emit build_context_requested(token_budget());
}

void ContextWidget::budget_changed(int value) {
    const int stepped_value = ((value + 25) / 50) * 50;

    if (value != stepped_value) {
        context_budget_slider_->setValue(stepped_value);
        return;
    }

    budget_value_label_->setText(QString::number(value));
}

}  // namespace aiws::gui
