#include "aiws/gui/term_inspector_widget.hpp"
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

namespace aiws::gui {

TermInspectorWidget::TermInspectorWidget(QWidget* parent) : QWidget(parent) {
    term_edit_ = new QLineEdit(this);
    inspect_button_ = new QPushButton("Inspect", this);

    document_frequency_value_ = new QLabel(this);
    chunk_frequency_value_ = new QLabel(this);

    auto* title_label = new QLabel("Term Inspector:", this);
    auto* term_label = new QLabel("Term:", this);
    auto* document_frequency_label =
        new QLabel("Document Frequency:", this);
    auto* chunk_frequency_label =
        new QLabel("Selected Chunk TF:", this);

    auto* term_layout = new QHBoxLayout;
    term_layout->addWidget(term_label);
    term_layout->addWidget(term_edit_);
    term_layout->addWidget(inspect_button_);

    auto* statistics_layout = new QHBoxLayout;
    statistics_layout->addWidget(document_frequency_label);
    statistics_layout->addWidget(document_frequency_value_);
    statistics_layout->addStretch();
    statistics_layout->addWidget(chunk_frequency_label);
    statistics_layout->addWidget(chunk_frequency_value_);

    auto* main_layout = new QVBoxLayout(this);
    main_layout->addWidget(title_label);
    main_layout->addLayout(term_layout);
    main_layout->addLayout(statistics_layout);

    connect(inspect_button_,
            &QPushButton::clicked,
            this,
            &TermInspectorWidget::request_inspection);

    reset();
}

void TermInspectorWidget::set_inspection_enabled(bool enabled) {
    inspect_button_->setEnabled(enabled);
}

void TermInspectorWidget::set_selected_chunk(const QString& chunk_id) {
    selected_chunk_id_ = chunk_id;
}

void TermInspectorWidget::set_statistics(std::size_t document_frequency,
                                         bool has_chunk_frequency,
                                         std::size_t chunk_frequency) {
    document_frequency_value_->setText(
        QString::number(document_frequency));

    if (has_chunk_frequency) {
        chunk_frequency_value_->setText(
            QString::number(chunk_frequency));
    } else {
        chunk_frequency_value_->setText("-");
    }
}

void TermInspectorWidget::clear_statistics() {
    document_frequency_value_->setText("-");
    chunk_frequency_value_->setText("-");
}

void TermInspectorWidget::reset() {
    term_edit_->clear();
    selected_chunk_id_.clear();
    clear_statistics();
    set_inspection_enabled(false);
}

void TermInspectorWidget::request_inspection() {
    const QString term = term_edit_->text();

    if (term.trimmed().isEmpty()) {
        return;
    }

    emit term_inspection_requested(term, selected_chunk_id_);
}

}  // namespace aiws::gui
