#include "aiws/gui/search_widget.hpp"
#include "aiws/gui/search_result_model.hpp"
#include <QAbstractItemView>
#include <QHBoxLayout>
#include <QItemSelectionModel>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QTableView>
#include <QVBoxLayout>
#include <utility>

namespace aiws::gui {

SearchWidget::SearchWidget(QWidget* parent) : QWidget(parent) {
    query_edit_ = new QLineEdit(this);

    result_count_spin_ = new QSpinBox(this);
    result_count_spin_->setRange(1, 100);
    result_count_spin_->setValue(3);

    search_button_ = new QPushButton("Search", this);

    result_model_ = new SearchResultModel(this);

    results_view_ = new QTableView(this);
    results_view_->setModel(result_model_);
    results_view_->setSelectionBehavior(QAbstractItemView::SelectRows);
    results_view_->setEditTriggers(QAbstractItemView::NoEditTriggers);

    selected_result_text_ = new QPlainTextEdit(this);
    selected_result_text_->setReadOnly(true);

    view_source_button_ = new QPushButton("View Source", this);

    auto* query_label = new QLabel("Query:", this);
    auto* results_label = new QLabel("Results:", this);
    auto* selected_result_label = new QLabel("Selected Result:", this);

    auto* query_layout = new QHBoxLayout;
    query_layout->addWidget(query_label);
    query_layout->addWidget(query_edit_);
    query_layout->addWidget(results_label);
    query_layout->addWidget(result_count_spin_);
    query_layout->addWidget(search_button_);

    auto* selected_result_layout = new QHBoxLayout;
    selected_result_layout->addWidget(selected_result_label);
    selected_result_layout->addStretch();
    selected_result_layout->addWidget(view_source_button_);

    auto* main_layout = new QVBoxLayout(this);
    main_layout->addLayout(query_layout);
    main_layout->addWidget(results_view_);
    main_layout->addLayout(selected_result_layout);
    main_layout->addWidget(selected_result_text_);

    connect(search_button_,
            &QPushButton::clicked,
            this,
            &SearchWidget::request_search);

    connect(query_edit_,
            &QLineEdit::textChanged,
            this,
            &SearchWidget::query_changed);

    connect(results_view_->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this,
            &SearchWidget::show_selected_result);

    connect(view_source_button_,
            &QPushButton::clicked,
            this,
            &SearchWidget::request_source_document);

    reset();
}

QString SearchWidget::query() const {
    return query_edit_->text();
}

int SearchWidget::result_count() const {
    return result_count_spin_->value();
}

bool SearchWidget::has_query() const {
    return !query().trimmed().isEmpty();
}

void SearchWidget::set_search_enabled(bool enabled) {
    search_button_->setEnabled(enabled);
}

void SearchWidget::set_results(std::vector<SearchResult> results) {
    result_model_->set_results(std::move(results));
    results_view_->clearSelection();
    selected_result_text_->clear();
    view_source_button_->setEnabled(false);
    emit selected_chunk_changed(QString());
}

void SearchWidget::clear_results() {
    result_model_->clear();
    results_view_->clearSelection();
    selected_result_text_->clear();
    view_source_button_->setEnabled(false);
    emit selected_chunk_changed(QString());
}

void SearchWidget::reset() {
    query_edit_->clear();
    result_count_spin_->setValue(3);
    clear_results();
    set_search_enabled(false);
}

void SearchWidget::request_search() {
    emit search_requested(query(), result_count());
}

void SearchWidget::show_selected_result() {
    const int row = results_view_->currentIndex().row();
    const SearchResult* result = result_model_->result_at(row);

    if (result == nullptr) {
        selected_result_text_->clear();
        view_source_button_->setEnabled(false);
        emit selected_chunk_changed(QString());
        return;
    }

    QString details;
    details += "Document: ";
    details += QString::fromStdString(result->document_id);
    details += "\nChunk: ";
    details += QString::number(static_cast<int>(result->chunk_sequence));
    details += "\nScore: ";
    details += QString::number(result->score);
    details += "\nText:\n";
    details += QString::fromStdString(result->text);

    selected_result_text_->setPlainText(details);
    view_source_button_->setEnabled(true);

    emit selected_chunk_changed(
        QString::fromStdString(result->chunk_id));
}

void SearchWidget::request_source_document() {
    const int row = results_view_->currentIndex().row();
    const SearchResult* result = result_model_->result_at(row);

    if (result == nullptr) {
        return;
    }

    emit source_document_requested(
        QString::fromStdString(result->document_id));
}

}  // namespace aiws::gui
