//
// Created by mizuk on 2024/10/31.
//

#ifndef HELPER_H
#define HELPER_H

inline bool isDigit(const char ch) {
    return ch >= '0' && ch <= '9';
}

inline bool isDigit1to9(const char ch) {
    return ch >= '1' && ch <= '9';
}

inline bool isWhiteSpace(const char ch) {
    return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r';
}

#endif //HELPER_H
