#include "FormWrapper.h"
#include "../../model/Helper.h"
#include <memory>
#include <climits>
#include <stdexcept>

FormWrapper::FormWrapper(const std::vector<std::string> &fields) {
    this->NUMBER_OF_FIELDS = fields.size();
    this->m_fieldNames = fields;
    this->field = new FIELD *[this->NUMBER_OF_FIELDS + 1];
    this->FIELD_OFFSET = 0;
    // iterate over vector and set FIELD_OFFSET to the longest string length
    for (size_t i = 0; i < this->NUMBER_OF_FIELDS; i++) {
        if (fields[i].length() > this->FIELD_OFFSET) {
            this->FIELD_OFFSET = fields[i].length();
        }
    }
    FIELD_OFFSET += 5;
}

FormWrapper::~FormWrapper() {
    unpost_form(this->m_form);
    free_form(this->m_form);
    for (size_t i = 0; i < this->NUMBER_OF_FIELDS; i++) {
        free_field(this->field[i]);
    }
    delete[] this->field;

}

// https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/
void FormWrapper::draw() {
    field[NUMBER_OF_FIELDS] = nullptr;
    for (int i = 0; i < static_cast<int>(NUMBER_OF_FIELDS); i++) {
        mvprintw(2 * i + 2, 10, m_fieldNames[i].c_str());
        field[i] = new_field(1, FIELD_WIDTH, 2 * i + 2, static_cast<int>(FIELD_OFFSET), 0, 0);
        set_field_back(field[i], A_UNDERLINE);
        field_opts_off(field[i], O_AUTOSKIP);
    }
    // create and post form
    m_form = new_form(field);
    post_form(m_form);
    // write field names
    for (int i = 0; i < static_cast<int>(NUMBER_OF_FIELDS); i++) {
        mvprintw(2 * i + 2, 2, m_fieldNames[i].c_str());
    }
    set_current_field(m_form, field[0]);
}

void FormWrapper::setFieldType(size_t field_index, FIELDTYPE *type) {
    if (field_index >= NUMBER_OF_FIELDS) {
        throw std::out_of_range("Field index out of range");
    }
    set_field_type(field[field_index], type, 0, 0, INT_MAX);
}

void FormWrapper::handleKey(int key_pressed) {
    switch (key_pressed) {
        case KEY_DOWN:
            form_driver(m_form, REQ_NEXT_FIELD);
            form_driver(m_form, REQ_END_LINE);
            break;
        case KEY_UP:
            form_driver(m_form, REQ_PREV_FIELD);
            form_driver(m_form, REQ_END_LINE);
            break;
        case KEY_LEFT:
            form_driver(m_form, REQ_PREV_CHAR);
            break;
        case KEY_RIGHT:
            form_driver(m_form, REQ_NEXT_CHAR);
            break;
        case KEY_BACKSPACE:
        case 127:
        case '\b':
            form_driver(m_form, REQ_DEL_PREV);
            break;
        case KEY_DC:
            form_driver(m_form, REQ_DEL_CHAR);
            break;
        case KEY_HOME:
            form_driver(m_form, REQ_BEG_FIELD);
            break;
        case KEY_END:
            form_driver(m_form, REQ_END_FIELD);
            break;
        default:
            form_driver(m_form, key_pressed);
            break;
    }
    refresh();
}


void FormWrapper::clearField(size_t field_index) {
    if (field_index >= NUMBER_OF_FIELDS) {
        throw std::out_of_range("Field index out of range");
    }
    set_field_buffer(field[field_index], 0, "");
}

void FormWrapper::clearFields() {
    for (size_t i = 0; i < NUMBER_OF_FIELDS; i++) {
        clearField(i);
    }
}

std::string FormWrapper::getFieldValue(size_t field_index) const {
    if (field_index >= NUMBER_OF_FIELDS) {
        throw std::out_of_range("Field index out of range");
    }
    form_driver(m_form, REQ_VALIDATION);
    return Helper::trim(field_buffer(field[field_index], 0));
}

size_t FormWrapper::getUnsignedValue(size_t field_index) const {
    if (field_index >= NUMBER_OF_FIELDS) {
        throw std::out_of_range("Field index out of range");
    }
    form_driver(m_form, REQ_VALIDATION);
    auto fieldValue = getFieldValue(field_index);
    if (!fieldValue.empty() && fieldValue[0] == '-') {
        throw (std::invalid_argument("unsignedValue can't be negative"));
    }
    auto result = strtoull(fieldValue.c_str(), nullptr, 10);
    if (result == ULLONG_MAX) { return 0; }
    return result;
}

void FormWrapper::printStatus(const std::string &msg) {
    // clear line at the bottom of the screen
    mvhline(LINES - 1, 0, ' ', COLS);
    // print a message at the bottom of the screen
    mvprintw(LINES - 1, 0, msg.c_str());
    refresh();
}

void FormWrapper::setFieldValue(size_t field_index, const std::string &value) {
    if (field_index >= NUMBER_OF_FIELDS) {
        throw std::out_of_range("Field index out of range");
    }
    set_field_buffer(field[field_index], 0, value.c_str());
}

std::vector<std::string> FormWrapper::getFieldValues() const {
    std::vector<std::string> result;
    for (size_t i = 0; i < NUMBER_OF_FIELDS; i++) {
        result.emplace_back(getFieldValue(i));
    }
    return result;
}