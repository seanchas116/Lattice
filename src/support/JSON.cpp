#include "JSON.hpp"
#include <QString>
#include <nlohmann/json.hpp>

void to_json(nlohmann::json &json, const QString &string) {
    json = string.toStdString();
}

void from_json(const nlohmann::json &json, QString &string) {
    string = QString::fromStdString(json);
}
