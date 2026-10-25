#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../source/include/utils.h"

static int tests_passed = 0;
static int tests_total = 0;

#define TEST_ASSERT(cond, desc) do { \
    tests_total++; \
    if (cond) { \
        tests_passed++; \
        printf("  [PASS] %s\n", desc); \
    } else { \
        printf("  [FAIL] %s (Line %d)\n", desc, __LINE__); \
    } \
} while (0)

static void test_whitespace_and_trim() {
    printf("\n=== 1. Test is_empty_or_whitespace & trim_whitespace ===\n");
    TEST_ASSERT(is_empty_or_whitespace(NULL) == 1, "NULL string la empty");
    TEST_ASSERT(is_empty_or_whitespace("") == 1, "Chuoi rong la empty");
    TEST_ASSERT(is_empty_or_whitespace("   \t\n\r") == 1, "Chuoi toan khoang trang la empty");
    TEST_ASSERT(is_empty_or_whitespace("SP001") == 0, "Chuoi co ky tu hop le khong phai empty");

    char str1[64] = "   Lap top Dell   ";
    trim_whitespace(str1);
    TEST_ASSERT(strcmp(str1, "Lap top Dell") == 0, "trim_whitespace cat ca dau va cuoi");

    char str2[64] = "NoSpaces";
    trim_whitespace(str2);
    TEST_ASSERT(strcmp(str2, "NoSpaces") == 0, "trim_whitespace giu nguyen chuoi khong khoang trang");

    char str3[64] = "     ";
    trim_whitespace(str3);
    TEST_ASSERT(strcmp(str3, "") == 0, "trim_whitespace chuoi toan cach tro thanh rong");
}

static void test_safe_string_copy() {
    printf("\n=== 2. Test safe_string_copy (Chong tran bo dem) ===\n");
    char dest[10];

    // Copy vua van
    int st1 = safe_string_copy(dest, sizeof(dest), "SP001");
    TEST_ASSERT(st1 == STATUS_SUCCESS && strcmp(dest, "SP001") == 0, "safe_string_copy thanh cong");

    // Copy vuot qua kich thuoc dich (Buffer Overflow protection)
    int st2 = safe_string_copy(dest, sizeof(dest), "Chuoi nay dai hon 10 bytes rat nhieu");
    TEST_ASSERT(st2 == STATUS_ERR_OVERFLOW, "safe_string_copy phat hien tran va cat an toan");
    TEST_ASSERT(dest[sizeof(dest) - 1] == '\0', "safe_string_copy luon dam bao null-terminator");

    // Copy voi con tro NULL
    int st3 = safe_string_copy(NULL, 10, "test");
    TEST_ASSERT(st3 == STATUS_ERR_NULL_PTR, "safe_string_copy chan dest NULL");
}

static void test_split_line() {
    printf("\n=== 3. Test split_line (Tach chuoi CSV/Pipe) ===\n");
    char line[] = "SP001|Laptop Dell|Laptop|Chiec|35|1499.99";
    char *tokens[8];
    int count = split_line(line, '|', tokens, 8);

    TEST_ASSERT(count == 6, "split_line tach dung 6 truong");
    TEST_ASSERT(strcmp(tokens[0], "SP001") == 0, "Token 0 dung");
    TEST_ASSERT(strcmp(tokens[1], "Laptop Dell") == 0, "Token 1 dung");
    TEST_ASSERT(strcmp(tokens[5], "1499.99") == 0, "Token 5 dung");

    // Test line NULL
    TEST_ASSERT(split_line(NULL, '|', tokens, 8) == 0, "split_line chan line NULL");
}

static void test_safe_conversions() {
    printf("\n=== 4. Test safe_str_to_int & safe_str_to_double ===\n");
    int int_val = 0;
    double dbl_val = 0.0;

    // So nguyen hop le
    TEST_ASSERT(safe_str_to_int("123", &int_val) == STATUS_SUCCESS && int_val == 123, "safe_str_to_int('123') = 123");
    TEST_ASSERT(safe_str_to_int("-45", &int_val) == STATUS_SUCCESS && int_val == -45, "safe_str_to_int('-45') = -45");

    // So nguyen sai dinh dang
    TEST_ASSERT(safe_str_to_int("qưqw", &int_val) == STATUS_ERR_INVALID_QTY, "safe_str_to_int('qưqw') bao loi");
    TEST_ASSERT(safe_str_to_int("12a", &int_val) == STATUS_ERR_INVALID_QTY, "safe_str_to_int('12a') bao loi");
    TEST_ASSERT(safe_str_to_int("999999999999999999", &int_val) == STATUS_ERR_OVERFLOW, "safe_str_to_int chan tran so INT_MAX");

    // So thuc hop le
    TEST_ASSERT(safe_str_to_double("1499.99", &dbl_val) == STATUS_SUCCESS && dbl_val > 1499.0, "safe_str_to_double('1499.99') thanh cong");
    TEST_ASSERT(safe_str_to_double("abc", &dbl_val) == STATUS_ERR_INVALID_PRICE, "safe_str_to_double('abc') bao loi");
}

static void test_case_transformations() {
    printf("\n=== 5. Test to_upper_case & to_lower_case ===\n");
    char s1[] = "sp001_dell";
    to_upper_case(s1);
    TEST_ASSERT(strcmp(s1, "SP001_DELL") == 0, "to_upper_case chuyen thanh chu hoa");

    char s2[] = "LAPTOP_PRO";
    to_lower_case(s2);
    TEST_ASSERT(strcmp(s2, "laptop_pro") == 0, "to_lower_case chuyen thanh chu thuong");
}

int main() {
    printf("=======================================================================\n");
    printf("                  UNIT TEST: MODULE TIEN ICH (UTILS.H / UTILS.C)        \n");
    printf("=======================================================================\n");

    test_whitespace_and_trim();
    test_safe_string_copy();
    test_split_line();
    test_safe_conversions();
    test_case_transformations();

    printf("\n=======================================================================\n");
    printf("KET QUA KIEM THU: Tong so ca: %d | Dat: %d | Loi: %d\n",
           tests_total, tests_passed, tests_total - tests_passed);
    printf("TRANG THAI: %s\n", (tests_passed == tests_total) ? "TAT CA DEU PASS [OK]" : "CO LOI [FAIL]");
    printf("=======================================================================\n");

    return (tests_passed == tests_total) ? 0 : 1;
}
