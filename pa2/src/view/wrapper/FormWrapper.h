#pragma once

#include "form.h"
#include <string>
#include <vector>

const int FIELD_WIDTH = 60;

/**
 * @brief Wrapper class for form.h
 * A wrapper that satisfies the requirements of this project
 * for working with the form.h library.
 */
class FormWrapper {
protected:
    size_t NUMBER_OF_FIELDS{};
    std::vector<std::string> m_fieldNames{};
    FORM *m_form{};
    FIELD **field{};
    size_t FIELD_OFFSET;
public:
    /**
    * @brief FormWrapper::FormWrapper Constructor
    * @param fields vector of strings to be used as field names, number of elements in vector determines number of m_fieldNames
    */
    explicit FormWrapper(const std::vector<std::string> &fields);

    /**
    * @brief FormWrapper::~FormWrapper Destructor
    * Frees memory allocated for form*, field* and field**.
    */
    ~FormWrapper();

    /**
    * @brief Draws the form with default values that best suit the ui of this program.
    */
    void draw();

    /**
     * @brief Handles basic form navigation.
     * @param key_pressed key to be processed
     * @usage
     * Use after all keys that are not used by the form are handled\n
     * (default keys are written into the active field).
     */
    void handleKey(int key_pressed);

    /**
     * @brief Sets the type of the field at the given index.
     * @param field_index index of the field to be set
     * @param type type of the field to be set
     * @throw std::out_of_range if field_index is out of range
     */
    void setFieldType(size_t field_index, FIELDTYPE *type);

    /**
     * @brief Clears the field at the given index.
     * @param field_index index of the field to be cleared
     * @throw std::out_of_range if field_index is out of range
     */
    void clearField(size_t field_index);

    /**
     * @brief Clears all fields of form.
     * @see clearField(size_t field_index)
     */
    void clearFields();

    /**
     * @return the number of m_fieldNames in the form.
     */
    [[nodiscard]] inline size_t getNumberOfFields() const { return NUMBER_OF_FIELDS; }

    /**
     * @brief Returns the vector of field names.
     * @return vector of field names
     */
    [[nodiscard]] inline const std::vector<std::string> &getFieldNames() const { return m_fieldNames; }

    /**
     * @brief Get the value of the field at the given index.
     * @param field_index index of the field to be returned
     * @return string value of the field
     * @throws std::out_of_range if field_index is out of range
     */
    [[nodiscard]] std::string getFieldValue(size_t field_index) const;

    [[nodiscard]] std::vector<std::string> getFieldValues() const;

    void setFieldValue(size_t field_index, const std::string &value);

    /**
     * @brief Gets value of a numeric field at the given index.
     * @param field_index index of numeric field
     * @return unsigned value of the field
     * @throws std::out_of_range if field_index is out of range
     * @throws std::runtime_error if field is not numeric
     * @see getFieldValue(size_t field_index)
     */
    [[nodiscard]] size_t getUnsignedValue(size_t field_index) const;

    /**
     * @nrief Prints a message at the bottom of the terminal.
     * @param msg to be printed
     */
    void printStatus(const std::string &msg);
};
