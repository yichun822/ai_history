//
// Created by yichun822 on 2026/3/26.
//

#ifndef AI_HISTORY_MESSAGE_STRUCT_H
#define AI_HISTORY_MESSAGE_STRUCT_H
#include <QString>
#include <utility>


class message_struct {
public:
    int time;
    QString happened;
    QString content;
    bool operator>(const message_struct &other) const {
        return time > other.time;
    }
    bool operator<(const message_struct &other) const {
        return time < other.time;
    }
};



#endif //AI_HISTORY_MESSAGE_STRUCT_H