#include "aiws/gui/workspace_widget.hpp"
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>

namespace aiws::gui {

WorkspaceWidget::WorkspaceWidget(QWidget* parent) : QWidget(parent) {
    document_list_ = new QListWidget(this);

    add_document_button_ = new QPushButton("Add Document", this);
    build_corpus_button_ = new QPushButton("Build Corpus", this);

    corpus_status_label_ = new QLabel(this);

    document_title_label_ = new QLabel(this);

    document_text_ = new QPlainTextEdit(this);
    document_text_->setReadOnly(true);

    auto* workspace_label = new QLabel("Workspace", this);
    auto* selected_document_label = new QLabel("Selected Document", this);

    auto* workspace_top_layout = new QHBoxLayout;
    workspace_top_layout->addWidget(workspace_label);
    workspace_top_layout->addWidget(add_document_button_);
    workspace_top_layout->addWidget(build_corpus_button_);
    workspace_top_layout->addStretch();

    auto* workspace_layout = new QVBoxLayout;
    workspace_layout->addLayout(workspace_top_layout);
    workspace_layout->addWidget(document_list_);
    workspace_layout->addWidget(corpus_status_label_);

    auto* selected_document_layout = new QVBoxLayout;
    selected_document_layout->addWidget(selected_document_label);
    selected_document_layout->addWidget(document_title_label_);
    selected_document_layout->addWidget(document_text_);

    auto* main_layout = new QHBoxLayout(this);
    main_layout->addLayout(workspace_layout);
    main_layout->addLayout(selected_document_layout);
    main_layout->setStretch(0, 1);
    main_layout->setStretch(1, 1);

    connect(add_document_button_,
            &QPushButton::clicked,
            this,
            &WorkspaceWidget::add_document_requested);

    connect(build_corpus_button_,
            &QPushButton::clicked,
            this,
            &WorkspaceWidget::build_corpus_requested);

    connect(document_list_,
            &QListWidget::currentRowChanged,
            this,
            &WorkspaceWidget::document_selected);

    reset();
}

void WorkspaceWidget::add_document(const QString& title) {
    document_list_->addItem(title);
}

void WorkspaceWidget::select_document(int row) {
    document_list_->setCurrentRow(row);
}

void WorkspaceWidget::show_selected_document(const QString& title,
                                              const QString& text) {
    document_title_label_->setText(title);
    document_text_->setPlainText(text);
}

void WorkspaceWidget::clear_selected_document() {
    document_title_label_->clear();
    document_text_->clear();
}

void WorkspaceWidget::set_corpus_status(const QString& text) {
    corpus_status_label_->setText(text);
}

void WorkspaceWidget::set_build_corpus_enabled(bool enabled) {
    build_corpus_button_->setEnabled(enabled);
}

void WorkspaceWidget::reset() {
    document_list_->clear();
    clear_selected_document();
    set_corpus_status("Corpus: Not built");
    set_build_corpus_enabled(false);
}

}  // namespace aiws::gui
