#include <stdio.h>
#include <string.h>

#define MAX_STAFF 30
#define MAX_DAYS 31
#define MAX_SHIFT_PER_DAY 10
#define NAME_LEN 50
#define START_HOUR 14
#define END_HOUR 24

typedef struct { // スタッフ情報
    char name[NAME_LEN]; // 名前
    char gender[10];     // 性別
    char role[20];       // 役割（キッチン/ホール/両方）
    char commute[20];    // 通勤手段
    char lastTrain[10];  // 終電時刻
} Staff;

typedef struct { // シフト情報
    int staffIndex;  // 誰か
    int start;       // 開始時刻
    int end;         // 終了時刻
} Shift;

Staff staffList[MAX_STAFF];
int staffCount = 0;

Shift shifts[MAX_DAYS][MAX_SHIFT_PER_DAY];
int shiftCount[MAX_DAYS] = {0};

// 入力バッファクリア
void clearInput(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}


// 整数入力
int inputInt(const char *msg, int min, int max) {
    int x;
    while (1) {
        printf("%s", msg);
        if (scanf("%d", &x) != 1) {
            printf("数値を入力してください。\n");
            clearInput();
            continue;
        }
        clearInput();
        if (x >= min && x <= max) return x;
        printf("範囲外の値です。\n");
    }
}


// 終電入力（00:00～23:59）
void inputLastTrainTime(int *h, int *m, const char *label) {  
    *h = inputInt(label, 0, 23);
    *m = inputInt("分: ", 0, 59);
}


// シフト入力（00:00～29:59）
void inputShiftTime(int *h, int *m, const char *label) {
    while (1) {
        *h = inputInt(label, 0, 29);
        *m = inputInt("分: ", 0, 59);
        
        if (*h > 24 && *m != 0) {
            printf("24時以降は00分のみ有効です。\n");
            continue;
        }
        break;
    }
}


// 性別入力
void inputGender(char *gender) {
    while (1) {
        printf("性別: ");
        fgets(gender, 10, stdin);
        if (strchr(gender, '\n') == NULL) clearInput();
        gender[strcspn(gender, "\n")] = 0;
        if (strcmp(gender, "男") == 0 ||
            strcmp(gender, "女") == 0 ||
            strcmp(gender, "その他") == 0) break;
        printf("無効な入力です。男または女またはその他を入力してください。\n");
    }
}

// 役割入力
void inputRole(char *role) {
    while (1) {
        printf("役割(キッチン/ホール/両方): ");
        fgets(role, 20, stdin);
        if (strchr(role, '\n') == NULL) clearInput();
        role[strcspn(role, "\n")] = 0;
        if (strcmp(role, "キッチン") == 0 ||
            strcmp(role, "ホール") == 0 ||
            strcmp(role, "両方") == 0) break;
        printf("無効な入力です。キッチンまたはホールまたは両方を入力してください。\n");
    }
}

// 通勤手段入力
void inputCommute(char *commute) {
    while (1) {
        printf("通勤手段(徒歩/自転車/バス/電車): ");
        fgets(commute, 20, stdin);
        if (strchr(commute, '\n') == NULL) clearInput();
        commute[strcspn(commute, "\n")] = 0;
        if (strcmp(commute, "徒歩") == 0 ||
            strcmp(commute, "自転車") == 0 ||
            strcmp(commute, "バス") == 0 ||
            strcmp(commute, "電車") == 0) break;
        printf("無効な入力です。通勤手段を入力してください。\n");
    }
}


// スタッフ追加
void addStaff() {
    if (staffCount >= MAX_STAFF) {
        printf("スタッフ数が上限に達しています。\n");
        return;
    }
    
    Staff *s;
    s = &staffList[staffCount];

    // 名前入力
    printf("名前: ");
    fgets(s->name, sizeof(s->name), stdin);
    s->name[strcspn(s->name, "\n")] = 0;

    // 性別入力
    inputGender(s->gender);
    
    // 役割入力
    inputRole(s->role);
    
    // 通勤手段入力
    inputCommute(s->commute);

    if (strcmp(s->commute, "バス") == 0 || strcmp(s->commute, "電車") == 0) {
        int hour, minute;
        inputLastTrainTime(&hour, &minute, "終電（時）: ");
        
        sprintf(s->lastTrain, "%02d:%02d", hour, minute); // "HH:MM" 形式にする
    } else {
        strcpy(s->lastTrain, "なし");
    }

    staffCount++;
}


// スタッフ一覧
void showStaffList() {
    printf("\n--- スタッフ一覧 ---\n");
    for (int i = 0; i < staffCount; i++) {
        printf("%-3d: %-25s (性別：%-6s, 役割：%-12s, 通勤手段：%-10s, 終電：%-6s)\n",
       i + 1,
       staffList[i].name,
       staffList[i].gender,
       staffList[i].role,
       staffList[i].commute,
       staffList[i].lastTrain);

    }
}


// スタッフ削除
void deleteStaff() {
    if (staffCount == 0) {
        printf("スタッフが登録されていません。\n");
        return;
    }

    showStaffList();

    int idx;
    while (1) {
        printf("削除するスタッフ番号 (1-%d): ", staffCount);
        scanf("%d", &idx);
        clearInput();

        if (idx >= 1 && idx <= staffCount) {
            idx -= 1;
            break;
        }
        printf("無効なスタッフ番号です。\n");
    }

    // スタッフ配列を詰める
    for (int i = idx; i < staffCount - 1; i++) {
        staffList[i] = staffList[i + 1];
    }
    staffCount--;

    // 削除したスタッフに紐づくシフトを削除し、残りのスタッフ番号を調整
    for (int day = 0; day < MAX_DAYS; day++) {
        int newCount = 0;
        for (int j = 0; j < shiftCount[day]; j++) {
            if (shifts[day][j].staffIndex == idx) {
                continue;
            }
            if (shifts[day][j].staffIndex > idx) {
                shifts[day][newCount].staffIndex = shifts[day][j].staffIndex - 1;
            } else {
                shifts[day][newCount].staffIndex = shifts[day][j].staffIndex;
            }
            shifts[day][newCount].start = shifts[day][j].start;
            shifts[day][newCount].end = shifts[day][j].end;
            newCount++;
        }
        shiftCount[day] = newCount;
    }

    printf("スタッフを削除しました。\n");
}


// スタッフ情報変更
void editStaff() {
    if (staffCount == 0) {
        printf("スタッフが登録されていません。\n");
        return;
    }

    showStaffList();

    int idx;
    while (1) {
        printf("変更するスタッフ番号 (1-%d): ", staffCount);
        scanf("%d", &idx);
        clearInput();

        if (idx >= 1 && idx <= staffCount) {
            idx -= 1;
            break;
        }
        printf("無効なスタッフ番号です。\n");
    }

    Staff *s = &staffList[idx];
    int choice;

    do {
        printf("\n--- %s の情報を変更 ---\n", s->name);
        printf("1: 名前 (%s)\n", s->name);
        printf("2: 性別 (%s)\n", s->gender);
        printf("3: 役割 (%s)\n", s->role);
        printf("4: 通勤手段 (%s)\n", s->commute);
        printf("5: 終電 (%s)\n", s->lastTrain[0] ? s->lastTrain : "なし");
        printf("0: 完了\n");
        printf("選択: ");
        scanf("%d", &choice);
        clearInput();

        switch (choice) {
            case 1:
                printf("名前: ");
                fgets(s->name, sizeof(s->name), stdin);
                s->name[strcspn(s->name, "\n")] = 0;
                break;

            case 2:
                inputGender(s->gender);
                break;

            case 3:
                inputRole(s->role);
                break;

            case 4:
                inputCommute(s->commute);
                if (strcmp(s->commute, "バス") == 0 || strcmp(s->commute, "電車") == 0) {
                    int hour, minute;
                    inputLastTrainTime(&hour, &minute, "終電（時）: ");
                    sprintf(s->lastTrain, "%02d:%02d", hour, minute);
                } else {
                    strcpy(s->lastTrain, "なし");
                }
                break;

            case 5:
                if (strcmp(s->commute, "バス") == 0 || strcmp(s->commute, "電車") == 0) {
                    int hour, minute;
                    inputLastTrainTime(&hour, &minute, "終電（時）: ");
                    sprintf(s->lastTrain, "%02d:%02d", hour, minute);
                } else {
                    printf("現在の通勤手段では終電情報は不要です。\n");
                }
                break;

            case 0:
                printf("変更を保存しました。\n");
                break;

            default:
                printf("無効な選択です。\n");
        }
    } while (choice != 0);
}


// 日付入力
void inputDay(int *month, int *day) {
    while (1) {
        printf("月（1-12）: ");
        scanf("%d", month);
        clearInput();
        if (*month >= 1 && *month <= 12) break;
        printf("無効な月です。もう一度入力してください。\n");
    }
    int max_day;
    if (*month == 2) {
        max_day = 28;  // 閏年考慮なし
    } else if (*month == 4 || *month == 6 || *month == 9 || *month == 11) {
        max_day = 30;
    } else {
        max_day = 31;
    }
    while (1) {
        printf("日（1-%d）: ", max_day);
        scanf("%d", day);

        clearInput();

        if (*day >= 1 && *day <= max_day) break;
        printf("無効な日です。もう一度入力してください。\n");
    }

    *day -= 1;
}


// シフト一覧表示
void showShiftList(int month, int day) {
    if (shiftCount[day] == 0) {
        printf("この日のシフトは登録されていません。\n");
        return;
    }

    printf("\n--- %d月%d日のシフト一覧 ---\n", month, day + 1);

    for (int j = 0; j < shiftCount[day]; j++) {
        Shift *s = &shifts[day][j];

        int sh = s->start / 60;
        int sm = s->start % 60;

        int eh = s->end / 60;
        int em = s->end % 60;

        printf("%d: スタッフ %s (番号 %d) %02d:%02d-%02d:%02d\n",
               j + 1,
               staffList[s->staffIndex].name,
               s->staffIndex + 1,
               sh, sm,
               eh, em);
    }
}


// シフト追加
void addShift() {
    int month, day;
    inputDay(&month, &day);

    if (shiftCount[day] >= MAX_SHIFT_PER_DAY) {
        printf("この日のシフトはこれ以上登録できません。\n");
        return;
    }

    showShiftList(month, day);

    showStaffList();

    int idx = inputInt("スタッフ番号: ", 1, staffCount) - 1;

    int sh, sm, eh, em;

    printf("開始時間\n");
    inputShiftTime(&sh, &sm, "時: ");

    printf("終了時間\n");
    inputShiftTime(&eh, &em, "時: ");

    int start = sh * 60 + sm;
    int end   = eh * 60 + em;

    // 日付またぎ対応
    if (end <= start) {
        end += 24 * 60;
    }

    // 重複チェック（同じスケールで比較）
    for (int j = 0; j < shiftCount[day]; j++) {
        if (shifts[day][j].staffIndex == idx) {
            int s2 = shifts[day][j].start;
            int e2 = shifts[day][j].end;

            if (!(end <= s2 || start >= e2)) {
                printf("既に重複するシフトがあります。\n");
                return;
            }
        }
    }

    // シフト長さチェック
    int max_end;
    if (sh >= 14 && sh <= 16) { // 14-16時スタートは休憩を挟めるので深夜までOK
        max_end = 24 * 60;
    } else {
        max_end = start + 6 * 60;
        if (max_end > 24 * 60 + 6 * 60) max_end = 24 * 60 + 6 * 60;
    }

    if (end > max_end) {
        printf("シフトの長さが長すぎます。\n");
        return;
    }

    // 終電チェック
    if (strcmp(staffList[idx].lastTrain, "なし") != 0) {
        int th, tm;
        sscanf(staffList[idx].lastTrain, "%d:%d", &th, &tm);

        int last = th * 60 + tm;

        // 日付またぎなら終電も翌日扱い
        if (end > 24 * 60) {
            last += 24 * 60;
        }

        if (end > last) {
            printf("終電に間に合いません。\n");
            return;
        }
    }

    shifts[day][shiftCount[day]].staffIndex = idx;
    shifts[day][shiftCount[day]].start = start;
    shifts[day][shiftCount[day]].end = end;

    shiftCount[day]++;

    printf("シフトを追加しました。\n");
}


// シフト変更
void editShift() {
    int month, day;
    inputDay(&month, &day);
    if (shiftCount[day] == 0) {
        printf("この日のシフトは登録されていません。\n");
        return;
    }

    showShiftList(month, day);

    int idx;
    while (1) {
        printf("変更するシフト番号: ");
        scanf("%d", &idx);
        clearInput();
        if (idx >= 1 && idx <= shiftCount[day]) {
            idx -= 1;
            break;
        }
        printf("無効なシフト番号です。\n");
    }

    Shift *s = &shifts[day][idx];
    int choice;
    do {
        printf("\n--- シフト編集 ---\n");
        printf("1: 担当スタッフ (%s)\n", staffList[s->staffIndex].name);
        printf("2: 開始／終了時間 (%02d:%02d - %02d:%02d)\n",
               s->start / 60, s->start % 60, s->end / 60, s->end % 60);
        printf("0: 完了\n");
        printf("選択: ");
        scanf("%d", &choice);
        clearInput();

        switch (choice) {
            case 1: {
                showStaffList();
                int staffIdx;
                while (1) {
                    printf("スタッフ番号 (1-%d): ", staffCount);
                    scanf("%d", &staffIdx);
                    clearInput();
                    if (staffIdx >= 1 && staffIdx <= staffCount) {
                        staffIdx -= 1;
                        break;
                    }
                    printf("無効なスタッフ番号です。\n");
                }
                s->staffIndex = staffIdx;
                break;
            }

            case 2: {
                int sh, sm, eh, em;

                printf("開始時間\n");
                inputShiftTime(&sh, &sm, "時: ");
                printf("終了時間\n");
                inputShiftTime(&eh, &em, "時: ");

                int start = sh * 60 + sm;
                int end   = eh * 60 + em;

                // 日付またぎ対応
                if (end <= start) {
                    end += 24 * 60;
                }

                // シフト長チェック
                int max_end;
                if (sh >= 14 && sh <= 16) {
                    max_end = 24 * 60;
                } else {
                    max_end = start + 6 * 60;
                    if (max_end > 24 * 60 + 6 * 60) max_end = 24 * 60 + 6 * 60;
                }

                if (end > max_end) {
                    printf("シフトの長さが長すぎます。\n");
                } else {
                    s->start = start;
                    s->end = end;
                }
                break;
            }

            case 0:
                printf("シフトを更新しました。\n");
                break;

            default:
                printf("無効な選択です。\n");
        }
    } while (choice != 0);
}


// シフト削除
void deleteShift() {
    int month, day;
    inputDay(&month, &day);
    if (shiftCount[day] == 0) {
        printf("この日のシフトは登録されていません。\n");
        return;
    }

    showShiftList(month, day);

    int idx;
    while (1) {
        printf("削除するシフト番号: ");
        scanf("%d", &idx);
        clearInput();
        if (idx >= 1 && idx <= shiftCount[day]) {
            idx -= 1;
            break;
        }
        printf("無効なシフト番号です。\n");
    }

    for (int j = idx; j < shiftCount[day] - 1; j++) {
        shifts[day][j] = shifts[day][j + 1];
    }
    shiftCount[day]--;
    printf("シフトを削除しました。\n");
}


// シフト表示
void showShift() {
    int month, day;
    inputDay(&month, &day);

    printf("\n--- %d月%d日のシフト ---\n", month, day + 1);

    int nameWidth = (int)strlen("名前");
    for (int i = 0; i < staffCount; i++) {
        int len = (int)strlen(staffList[i].name);
        if (len > nameWidth) nameWidth = len;
    }
    nameWidth += 2;

    printf("%-*s", nameWidth, "名前");
    for (int h = START_HOUR; h <= END_HOUR; h++) {
        printf("%02d:00|%02d:30|", h, h);
    }

    printf("\n");

    for (int i = 0; i < staffCount; i++) {
        printf("%-*s", nameWidth, staffList[i].name);

        for (int h = START_HOUR; h <= END_HOUR; h++) {
            for (int m = 0; m < 60; m += 30) {
                int current = h * 60 + m;
                int working = 0;

                for (int j = 0; j < shiftCount[day]; j++) {
                    if (shifts[day][j].staffIndex == i) {
                        if (current >= shifts[day][j].start &&
                            current < shifts[day][j].end) {
                            working = 1;
                        }
                    }
                }

                printf(working ? "　〇　" : "　×　");
            }
            printf(" ");
        }
        printf("\n");
    }
}


// メニュー
int main() {
    int choice;

    while (1) {
        printf("\n--- メニュー ---\n");
        printf("1: スタッフ追加\n");
        printf("2: スタッフ一覧\n");
        printf("3: スタッフ削除\n");
        printf("4: スタッフ情報変更\n");
        printf("5: シフト追加\n");
        printf("6: シフト表示\n");
        printf("7: シフト変更\n");
        printf("8: シフト削除\n");
        printf("0: 終了\n");
        printf("選択: ");

        scanf("%d", &choice);
        clearInput();

        switch (choice) {
            case 1: addStaff(); break;
            case 2: showStaffList(); break;
            case 3: deleteStaff(); break;
            case 4: editStaff(); break;
            case 5: addShift(); break;
            case 6: showShift(); break;
            case 7: editShift(); break;
            case 8: deleteShift(); break;
            case 0: return 0;
            default: printf("無効な入力\n");
        }
    }
}
